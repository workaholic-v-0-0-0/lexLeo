// src/interpreter/tests/test_interpreter.c

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "interpreter.h"
#include "fake_memory.h"
#include "memory_allocator.h"
#include "string_utils.h"
#include "internal/runtime_env_internal.h"
#include "ast.h"
#include "symtab.h"



//-----------------------------------------------------------------------------
// GLOBALS NOT DOUBLES, MAGIC NUMBER KILLERS
//-----------------------------------------------------------------------------

static const int A_INT = 7;
static runtime_env_value **out = NULL;
static runtime_env *env = NULL;
static const char *A_CONSTANT_STRING = "a_string";



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


// dummies

static runtime_env DUMMY_RUNTIME_ENV = {.bindings = NULL, .refcount = 2, .is_root = false, .parent = NULL};
static runtime_env *const DUMMY_RUNTIME_ENV_P = &DUMMY_RUNTIME_ENV;
static ast DUMMY_AST;
static ast *DUMMY_AST_P = &DUMMY_AST;
static ast *DUMMY_AST_CHILDREN_ARR[1] = { &DUMMY_AST };
static ast_children_t DUMMMY_AST_CHILDREN = {.children_nb = 1, .capacity = 1, .children = DUMMY_AST_CHILDREN_ARR,};
static ast_children_t *DUMMY_AST_CHILDREN_P = &DUMMMY_AST_CHILDREN;
static symbol DUMMY_SYMBOL = {.name = "symbol",};
static symbol *DUMMY_SYMBOL_P = &DUMMY_SYMBOL;


// mocks
// spies
// stubs


// fakes

#define FAKABLE_MALLOC(n) (get_current_malloc()(n))
#define FAKABLE_FREE(p) (get_current_free()(p))
#define FAKABLE_STRDUP(s) (get_current_string_duplicate()(s))



//-----------------------------------------------------------------------------
// TESTS interpreter_status interpreter_eval(struct runtime_env *env, const struct ast *root, struct runtime_env_value **out);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
    interpreter_status interpreter_eval(
        struct runtime_env *env,
        const struct ast *root,
        struct runtime_env_value **out );
other elements of the isolated unit:
 - module runtime_env
 - module ast
 - module symtab
 - module hashtable
 - module list
*/

// fake:
//  - functions of standard libray which are used:
//    - malloc, free, strdup



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int eval_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    env = runtime_env_wind(NULL);
    out = fake_malloc(sizeof(runtime_env_value *));
    *out = NULL;

    return 0;
}

static int eval_teardown(void **state) {
    (void)state;
    if (out) {
        if (*out) {
            runtime_env_value_destroy(*out);
            *out = NULL;
        }
        fake_free(out);
        out = NULL;
    }

    if (env) {
        runtime_env_unwind(env);
        env = NULL;
    }

    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
    fake_memory_reset();
    return 0;
}

static int eval_fake_memory_only_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    fake_memory_reset();

    return 0;
}

static int eval_fake_memory_only_teardown(void **state) {
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
    set_allocators(NULL, NULL);
    fake_memory_reset();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// At every test:
// Given:
//  -
// Expected:
//  - no invalid free
//  - no double free
//  - no memory leak


// Given:
//  - env == NULL
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_ERROR
static void eval_error_when_env_null(void **state) {
    (void)state;
    out = fake_malloc(sizeof(runtime_env_value *));
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    interpreter_status status = interpreter_eval(NULL, DUMMY_AST_P, out);

    assert_int_equal(status, INTERPRETER_STATUS_ERROR);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    fake_free(out);
    out = NULL;
}

// Given:
//  - root == NULL
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_ERROR
static void eval_error_when_root_null(void **state) {
    (void)state;
    out = fake_malloc(sizeof(runtime_env_value *));
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    interpreter_status status = interpreter_eval(DUMMY_RUNTIME_ENV_P, NULL, out);

    assert_int_equal(status, INTERPRETER_STATUS_ERROR);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    fake_free(out);
    out = NULL;
}

// Given:
//  - out == NULL
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_ERROR
static void eval_error_when_out_null(void **state) {
    (void)state;

    interpreter_status status = interpreter_eval(DUMMY_RUNTIME_ENV_P, DUMMY_AST_P, NULL);

    assert_int_equal(status, INTERPRETER_STATUS_ERROR);
}

// Given:
//  - args are not NULL
//  - root->type is not supported
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_ERROR
static void eval_error_when_unsupported_root_type(void **state) {
    (void)state;
    out = fake_malloc(sizeof(runtime_env_value *));
    ast *unsupported_ast = fake_malloc(sizeof(ast));
    memset(unsupported_ast, 0, sizeof(ast));
    unsupported_ast->type = AST_TYPE_NB_TYPES;
    unsupported_ast->children = DUMMY_AST_CHILDREN_P;
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    interpreter_status status = interpreter_eval(DUMMY_RUNTIME_ENV_P, unsupported_ast, out);

    assert_int_equal(status, INTERPRETER_STATUS_ERROR);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    fake_free(out);
    fake_free(unsupported_ast);
}

// Given:
//  - args are not NULL
//  - root is a number node
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static void eval_error_oom_when_int_node_and_malloc_fails(void **state) {
    (void)state;
    ast *number_node = ast_create_int_node(A_INT);
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    // Simulate a total memory allocation failure
    fake_memory_fail_on_all_call();

    interpreter_status status = interpreter_eval(env, number_node, out);

    // Restore normal allocation behavior
    fake_memory_fail_on_calls(0, NULL);

    assert_int_equal(status, INTERPRETER_STATUS_OOM);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(number_node);
}

// Given:
//  - args are valid
//  - root is a number node registering integer value A_INT
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - *out != NULL && (*out)->type == RUNTIME_VALUE_NUMBER && (*out)->as.i == A_INT
//  - returns INTERPRETER_STATUS_OK
static void eval_success_when_int_node_and_malloc_succeeds(void **state) {
    (void)state;
    ast *number_node = ast_create_int_node(A_INT);

    interpreter_status status = interpreter_eval(env, number_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_OK);
    assert_non_null(*out);
    assert_int_equal((*out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal((*out)->as.i, A_INT);

    ast_destroy(number_node);
}

// Given:
//  - args are not NULL
//  - root is a string node registering the value of the constant string A_CONSTANT_STRING
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static void eval_error_oom_when_string_node_and_malloc_fails(void **state) {
    (void)state;
    ast *string_node = ast_create_string_node(A_CONSTANT_STRING);
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    // Simulate a total memory allocation failure
    fake_memory_fail_on_all_call();

    interpreter_status status = interpreter_eval(env, string_node, out);

    // Restore normal allocation behavior
    fake_memory_fail_on_calls(0, NULL);

    assert_int_equal(status, INTERPRETER_STATUS_OOM);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(string_node);
}

// Given:
//  - args are valid
//  - root is a string node registering the value of the constant string A_CONSTANT_STRING
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - *out != NULL
//    && (*out)->type == RUNTIME_VALUE_STRING
//    && (assert_string_equal((*out)->as.s, A_CONSTANT_STRING)
//  - returns INTERPRETER_STATUS_OK
static void eval_success_when_string_node_and_malloc_succeeds(void **state) {
    (void)state;
    ast *string_node = ast_create_string_node(A_CONSTANT_STRING);

    interpreter_status status = interpreter_eval(env, string_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_OK);
    assert_non_null(*out);
    assert_int_equal((*out)->type, RUNTIME_VALUE_STRING);
    assert_string_equal((*out)->as.s, A_CONSTANT_STRING);

    ast_destroy(string_node);
}

// Given:
//  - args are not NULL
//  - root is a symbol node registering DUMMY_SYMBOL_P
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static void eval_error_oom_when_symbol_node_and_malloc_fails(void **state) {
    (void)state;
    ast *symbol_node = ast_create_symbol_node(DUMMY_SYMBOL_P);
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    // Simulate a total memory allocation failure
    fake_memory_fail_on_all_call();

    interpreter_status status = interpreter_eval(env, symbol_node, out);

    // Restore normal allocation behavior
    fake_memory_fail_on_calls(0, NULL);

    assert_int_equal(status, INTERPRETER_STATUS_OOM);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(symbol_node);
}

// Given:
//  - args are valid
//  - root is a symbol node registering DUMMY_SYMBOL_P
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - *out != NULL
//    && (*out)->type == RUNTIME_VALUE_SYMBOL
//    && (*out)->as.sym == DUMMY_SYMBOL_P
//  - returns INTERPRETER_STATUS_OK
static void eval_success_when_symbol_node_and_malloc_succeeds(void **state) {
    (void)state;
    ast *symbol_node = ast_create_symbol_node(DUMMY_SYMBOL_P);

    interpreter_status status = interpreter_eval(env, symbol_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_OK);
    assert_non_null(*out);
    assert_int_equal((*out)->type, RUNTIME_VALUE_SYMBOL);
    assert_ptr_equal((*out)->as.sym, DUMMY_SYMBOL_P);

    ast_destroy(symbol_node);
}


// here


/* draft
env->bindings is dummy
*/




//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
    const struct CMUnitTest _tests[] = {

        // invalid args
        cmocka_unit_test_setup_teardown(
            eval_error_when_env_null,
            eval_fake_memory_only_setup, eval_fake_memory_only_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_when_root_null,
            eval_fake_memory_only_setup, eval_fake_memory_only_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_when_out_null,
            eval_fake_memory_only_setup, eval_fake_memory_only_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_when_unsupported_root_type,
            eval_fake_memory_only_setup, eval_fake_memory_only_teardown),

        // AST_TYPE_DATA_WRAPPER ; TYPE_INT
        cmocka_unit_test_setup_teardown(
            eval_error_oom_when_int_node_and_malloc_fails,
            eval_setup, eval_teardown),
        cmocka_unit_test_setup_teardown(
            eval_success_when_int_node_and_malloc_succeeds,
            eval_setup, eval_teardown),

        // AST_TYPE_DATA_WRAPPER ; TYPE_STRING
        cmocka_unit_test_setup_teardown(
            eval_error_oom_when_string_node_and_malloc_fails,
            eval_setup, eval_teardown),
        cmocka_unit_test_setup_teardown(
            eval_success_when_string_node_and_malloc_succeeds,
            eval_setup, eval_teardown),

        // AST_TYPE_DATA_WRAPPER ; TYPE_SYMBOL
        cmocka_unit_test_setup_teardown(
            eval_error_oom_when_symbol_node_and_malloc_fails,
            eval_setup, eval_teardown),
        cmocka_unit_test_setup_teardown(
            eval_success_when_symbol_node_and_malloc_succeeds,
            eval_setup, eval_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(_tests, NULL, NULL);

    return failed;
}
