// src/interpreter/tests/test_interpreter.c

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include "interpreter.h"
#include "fake_memory.h"
#include "memory_allocator.h"
#include "string_utils.h"
#include "internal/runtime_env_internal.h"
#include "internal/runtime_env_ctx.h"
#include "ast.h"
#include "symtab.h"



//-----------------------------------------------------------------------------
// GLOBALS NOT DOUBLES, MAGIC NUMBER KILLERS
//-----------------------------------------------------------------------------

static const int A_INT = 7;
static runtime_env_value **out = NULL;
static runtime_env *env = NULL;
static const char *A_CONSTANT_STRING = "a string";
static ast_error_type AN_AST_ERROR_TYPE = AST_ERROR_CODE_BINDING_NODE_CREATION_FAILED;
static const char *A_CONSTANT_ERROR_MESSAGE = "an error message";



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

int mock_hashtable_add(hashtable *ht, const void *key, void *value) {
    check_expected(ht);
    check_expected(key);
    check_expected(value);
    return mock_type(int);
}


// spies

static runtime_env *spy_set_local_arg_e = NULL;
static const struct symbol *spy_set_local_arg_key = NULL;
static const runtime_env_value *spy_set_local_arg_value = NULL;
static bool spy_set_local_has_been_called = false;

bool spy_runtime_env_set_local(runtime_env *e, const struct symbol *key, const runtime_env_value *value) {
    spy_set_local_has_been_called = true;
    spy_set_local_arg_e = e;
    spy_set_local_arg_key = key;
    spy_set_local_arg_value = value;
    return mock_type(bool);
}



// stubs


// fakes

#define FAKABLE_MALLOC(n) (get_current_malloc()(n))
#define FAKABLE_FREE(p) (get_current_free()(p))
#define FAKABLE_STRDUP(s) (get_current_string_duplicate()(s))



//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------


static ast *an_empty_function_node_with_dummy_name() {
    // function name
    ast *dummy_function_name = ast_create_symbol_node(DUMMY_SYMBOL_P);

    // list of params
    ast *empty_list_of_params =
        ast_create_children_node_var(
            AST_TYPE_LIST_OF_PARAMETERS,
            0 );

    // body
    ast *empty_body =
        ast_create_children_node_var(
            AST_TYPE_BLOCK,
            0 );

    // function
    return
        ast_create_children_node_var(
            AST_TYPE_FUNCTION,
            3,
            dummy_function_name,
            empty_list_of_params,
            empty_body );
}

static ast *a_function_definition_node_with_empty_function() {
    return
    ast_create_children_node_var(
        AST_TYPE_FUNCTION_DEFINITION,
        1,
        an_empty_function_node_with_dummy_name() );
}

typedef enum {
    ILL_CHILDREN_NULL,
    ILL_NO_CHILD,
    ILL_ONE_CHILD,
    ILL_TWO_CHILDREN,
    ILL_THREE_CHILDREN,
} illness_type;

static ast *make_shallow_node(ast_type type) {
    ast *ret = fake_malloc(sizeof(ast));
    memset(ret, 0, sizeof(ast));
    ret->type = type;
    return ret;
}

static ast *make_ill_formed_node(ast_type type, illness_type how) {
    switch (how) {
        case ILL_CHILDREN_NULL: {
            ast *ret = make_shallow_node(type);
            if (ret) ret->children = NULL;
            return ret;
        }
        case ILL_NO_CHILD:
            return ast_create_children_node_var(type, 0);

        case ILL_ONE_CHILD:
            return ast_create_children_node_var(
                type,
                1,
                ast_create_int_node(A_INT) );

        case ILL_TWO_CHILDREN:
            return ast_create_children_node_var(
                type,
                2,
                ast_create_int_node(A_INT),
                ast_create_int_node(A_INT) );

        case ILL_THREE_CHILDREN:
            return ast_create_children_node_var(
                type,
                3,
                ast_create_int_node(A_INT),
                ast_create_int_node(A_INT),
                ast_create_int_node(A_INT) );
    }
    assert(!"invalid illness_type");
    return NULL;
}

static ast *make_unary_with_string(ast_type type, const char *s) {
    return ast_create_children_node_var(type, 1, ast_create_string_node(s));
}
static ast *make_unary_with_int(ast_type type, int i) {
    return ast_create_children_node_var(type, 1, ast_create_int_node(i));
}

static ast *a_ill_formed_negation_node(illness_type how) {
    return make_ill_formed_node(AST_TYPE_NEGATION, how);
}
static ast *a_well_formed_negation_of_string(void) {
    return make_unary_with_string(AST_TYPE_NEGATION, A_CONSTANT_STRING);
}
static ast *a_negation_node_with_a_number(int i) {
    return make_unary_with_int(AST_TYPE_NEGATION, i);
}

static ast *a_ill_formed_addition_node(illness_type how) {
    return make_ill_formed_node(AST_TYPE_ADDITION, how);
}
static ast *a_well_formed_addition_of_number_and_string() {
    return ast_create_children_node_var(
        AST_TYPE_ADDITION,
        2,
        ast_create_int_node(A_INT),
        ast_create_string_node(A_CONSTANT_STRING) );
}
static ast *a_well_formed_addition_of_two_numbers() {
    return ast_create_children_node_var(
        AST_TYPE_ADDITION,
        2,
        ast_create_int_node(A_INT),
        ast_create_int_node(A_INT) );
}


//-----------------------------------------------------------------------------
// TESTS interpreter_status interpreter_eval(struct runtime_env *env, const struct ast *root, struct runtime_env_value **out);
//-----------------------------------------------------------------------------


// At every test:
// Expected:
//  - no invalid free
//  - no double free
//  - no memory leak



//-----------------------------------------------------------------------------
// TESTS interpreter_status interpreter_eval(struct runtime_env *env, const struct ast *root, struct runtime_env_value **out);
// FOR INVALID ARGUMENTS MANAGEMENT
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
  - interpreter_status interpreter_eval(
        struct runtime_env *env,
        const struct ast *root,
        struct runtime_env_value **out );
other elements of the isolated unit:
  - out

doubles:
  - dummy:
    - env
    - root
  - fake:
    - functions of standard libray which are used:
      - malloc, free
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int eval_with_invalid_args_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    fake_memory_reset();

    return 0;
}

static int eval_with_invalid_args_teardown(void **state) {
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



//-----------------------------------------------------------------------------
// TESTS interpreter_status interpreter_eval(struct runtime_env *env, const struct ast *root, struct runtime_env_value **out);
// FOR EVALUATION OF AST OF TYPES AST_TYPE_DATA_WRAPPER AND AST_TYPE_ERROR
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
  - interpreter_status interpreter_eval(
        struct runtime_env *env,
        const struct ast *root,
        struct runtime_env_value **out );
other elements of the isolated unit:
  - root
  - out
  - from the runtime_env module:
    - runtime_env_make_number
    - runtime_env_make_string
    - runtime_env_make_symbol
    - runtime_env_make_error
    - runtime_env_value_destroy
- from the ast module:
    - ast_create_int_node
    - ast_create_string_node
    - ast_create_symbol_node
    - ast_create_error_node
    - ast_error_sentinel
    - ast_destroy
  - module hashtable
  - module list

doubles:
  - dummy:
    - env
    - argument "symbol *sym" of ast_create_symbol_node
  - functions of standard libray which are used:
    - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int eval_leaf_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    // dummy
    env = DUMMY_RUNTIME_ENV_P;

    // real
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
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
    fake_memory_reset();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - args are valid
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
//  - args are valid
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
//    && string_equal((*out)->as.s, A_CONSTANT_STRING)
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
//  - args are valid
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

// Given:
//  - args are valid
//  - root is an error node distinct from the sentinel
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static void eval_error_oom_when_error_node_not_sentinel_and_malloc_fails(void **state) {
    (void)state;
    ast *error_node_not_sentinel = ast_create_error_node(AST_ERROR_CODE_BINDING_NODE_CREATION_FAILED, A_CONSTANT_ERROR_MESSAGE);
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    // Simulate a total memory allocation failure
    fake_memory_fail_on_all_call();

    interpreter_status status = interpreter_eval(env, error_node_not_sentinel, out);

    // Restore normal allocation behavior
    fake_memory_fail_on_calls(0, NULL);

    assert_int_equal(status, INTERPRETER_STATUS_OOM);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(error_node_not_sentinel);
}

// Given:
//  - args are valid
//  - root is an error node distinct from the sentinel
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  -     *out != NULL
//    && (*out)->type == RUNTIME_VALUE_ERROR
//    && (*out)->as.err.code == root->error->code
//    && string_equal((*out)->as.err.msg, root->error->message)
//  - returns INTERPRETER_STATUS_OK
static void eval_success_when_error_node_not_sentinel_and_malloc_succeeds(void **state) {
    (void)state;
    ast *error_node_not_sentinel = ast_create_error_node(AST_ERROR_CODE_BINDING_NODE_CREATION_FAILED, A_CONSTANT_ERROR_MESSAGE);

    interpreter_status status = interpreter_eval(env, error_node_not_sentinel, out);

    assert_int_equal(status, INTERPRETER_STATUS_OK);
    assert_non_null(*out);
    assert_int_equal((*out)->type, RUNTIME_VALUE_ERROR);
    assert_int_equal((*out)->as.err.code, error_node_not_sentinel->error->code);
    assert_string_equal((*out)->as.err.msg, error_node_not_sentinel->error->message);

    ast_destroy(error_node_not_sentinel);
}

// Given:
//  - args are valid
//  - root is the error node sentinel
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static void eval_error_oom_when_error_node_sentinel_and_malloc_fails(void **state) {
    (void)state;
    ast *error_node_sentinel = ast_error_sentinel();
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    // Simulate a total memory allocation failure
    fake_memory_fail_on_all_call();

    interpreter_status status = interpreter_eval(env, error_node_sentinel, out);

    // Restore normal allocation behavior
    fake_memory_fail_on_calls(0, NULL);

    assert_int_equal(status, INTERPRETER_STATUS_OOM);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
}

// Given:
//  - args are valid
//  - root is the error node sentinel
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_ERROR
//    && (*out)->as.err.code == root->error->code
//    && string_equal((*out)->as.err.msg, root->error->message)
//  - returns INTERPRETER_STATUS_OK
static void eval_success_when_error_node_sentinel_and_malloc_succeeds(void **state) {
    (void)state;
    ast *error_node_sentinel = ast_error_sentinel();

    interpreter_status status = interpreter_eval(env, error_node_sentinel, out);

    assert_int_equal(status, INTERPRETER_STATUS_OK);
    assert_non_null(*out);
    assert_int_equal((*out)->type, RUNTIME_VALUE_ERROR);
    assert_int_equal((*out)->as.err.code, error_node_sentinel->error->code);
    assert_string_equal((*out)->as.err.msg, error_node_sentinel->error->message);
}



//-----------------------------------------------------------------------------
// TESTS interpreter_status interpreter_eval(struct runtime_env *env, const struct ast *root, struct runtime_env_value **out);
// FOR EVALUATION OF AST OF TYPE AST_TYPE_FUNCTION
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
  - interpreter_status interpreter_eval(
        struct runtime_env *env,
        const struct ast *root,
        struct runtime_env_value **out );
other elements of the isolated unit:
  - env (actually not doubled runtime_env_make_function and runtime_env_value_destroy retain/release it)
  - root
  - out
  - from the runtime_env module:
    - runtime_env_make_function
    - runtime_env_value_destroy
  - from the ast module:
    - ast_create_symbol_node
    - ast_create_children_node_var
    - ast_destroy

doubles:
  - dummy:
    - argument "symbol *sym" of ast_create_symbol_node
  - spy:
    - runtime_env_set_local (to check no binding)
    - spy_set_local_has_been_called
  - fake:
    - functions of standard libray which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int eval_function_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    // mock/spy
    runtime_env_set_set_local(spy_runtime_env_set_local);
    spy_set_local_has_been_called = false;

    // real
    env = runtime_env_wind(NULL);
    out = fake_malloc(sizeof(runtime_env_value *));
    *out = NULL;

    return 0;
}

static int eval_function_teardown(void **state) {
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
    runtime_env_set_set_local(NULL);
    fake_memory_reset();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - args are valid
//  - root is an empty function node
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - no binding in env->binding
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static void eval_error_oom_when_function_node_and_malloc_fails(void **state) {
    (void)state;
    ast *function_node = an_empty_function_node_with_dummy_name();
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    // Simulate a total memory allocation failure
    fake_memory_fail_on_all_call();

    interpreter_status status = interpreter_eval(env, function_node, out);

    // Restore normal allocation behavior
    fake_memory_fail_on_calls(0, NULL);

    assert_int_equal(status, INTERPRETER_STATUS_OOM);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);
    assert_false(spy_set_local_has_been_called);

    *out = NULL;
    ast_destroy(function_node);
}

// Given:
//  - args are valid
//  - root is an empty function node
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - no binding in env->binding
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_FUNCTION
//    && (*out)->as.fn.function_node == root
//    && (*out)->as.fn.closure == env
//  - returns INTERPRETER_STATUS_OK
static void eval_success_when_function_node_and_malloc_succeeds(void **state) {
    (void)state;
    ast *function_node = an_empty_function_node_with_dummy_name();

    interpreter_status status = interpreter_eval(env, function_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_OK);
    assert_non_null(*out);
    assert_int_equal((*out)->type, RUNTIME_VALUE_FUNCTION);
    assert_ptr_equal((*out)->as.fn.function_node, function_node);
    assert_ptr_equal((*out)->as.fn.closure, env);
    assert_false(spy_set_local_has_been_called);

    ast_destroy(function_node);
}



//-----------------------------------------------------------------------------
// TESTS interpreter_status interpreter_eval(struct runtime_env *env, const struct ast *root, struct runtime_env_value **out);
// FOR EVALUATION OF AST OF TYPE AST_TYPE_FUNCTION_DEFINITION
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
  - env
  - root
  - out
  - from the runtime_env module:
    - runtime_env_make_function
    - runtime_env_value_destroy
    - runtime_env_set_local
  - from the ast module:
    - ast_create_symbol_node
    - ast_create_children_node_var
    - ast_destroy

doubles:
  - dummy:
    - argument "symbol *sym" of ast_create_symbol_node
  - spy:
    - runtime_env_set_local (to check no binding)
    - spy_set_local_has_been_called
    - runtime_env *spy_set_local_arg_e
    - spy_set_local_arg_key
    - spy_set_local_arg_value
  - fake:
    - functions of standard libray which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int eval_function_definition_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    // mock/spy
    runtime_env_set_set_local(spy_runtime_env_set_local);
    spy_set_local_has_been_called = false;
    spy_set_local_arg_e = NULL;
    spy_set_local_arg_key = NULL;
    spy_set_local_arg_value = NULL;

    env = runtime_env_wind(NULL);
    out = fake_malloc(sizeof(runtime_env_value *));
    *out = NULL;

    return 0;
}

static int eval_function_definition_teardown(void **state) {
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
    runtime_env_set_set_local(NULL);
    fake_memory_reset();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - args are valid
//  - root is a function definition node and its child is an empty function node
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - no binding in env->binding
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static void eval_error_oom_when_function_definition_node_and_malloc_fails(void **state) {
    (void)state;
    ast *function_definition_node = a_function_definition_node_with_empty_function();
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    // Simulate a total memory allocation failure
    fake_memory_fail_on_all_call();

    interpreter_status status = interpreter_eval(env, function_definition_node, out);

    // Restore normal allocation behavior
    fake_memory_fail_on_calls(0, NULL);

    assert_int_equal(status, INTERPRETER_STATUS_OOM);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);
    assert_false(spy_set_local_has_been_called);

    *out = NULL;
    ast_destroy(function_definition_node);
}

// Given:
//  - args are valid
//  - root is a function definition node and its child is an empty function node
//  - all allocations will succeed during interpreter_eval call
//  - symbol binding will fail
// Expected:
//  - build v, a runtime_env_value * such as
//    - v != NULL
//    - v->type == RUNTIME_VALUE_FUNCTION
//    - v->as.fn.function_node == root->children->children[0]
//    - v->as.fn.closure == env
//  - calls runtime_env_set_local with:
//    - e: env
//    - key: root->children->children[0]->children->children[0]->data->data.symbol_value
//    - value: v
//  - destroys v
//  - no binding in env->binding
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_BINDING_ERROR
static void eval_binding_error_when_function_definition_node_and_binding_fails(void **state) {
    (void)state;
    ast *function_definition_node = a_function_definition_node_with_empty_function();
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;
    will_return(spy_runtime_env_set_local, false);

    interpreter_status status = interpreter_eval(env, function_definition_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_BINDING_ERROR);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);
    assert_true(spy_set_local_has_been_called);
    assert_ptr_equal(spy_set_local_arg_e, env);
    assert_ptr_equal(
        spy_set_local_arg_key,
        function_definition_node->children->children[0]->children->children[0]->data->data.symbol_value );
    assert_non_null(spy_set_local_arg_value);
    assert_int_equal(
        spy_set_local_arg_value->type,
        RUNTIME_VALUE_FUNCTION );
    assert_ptr_equal(
        spy_set_local_arg_value->as.fn.function_node,
        function_definition_node->children->children[0] );
    assert_ptr_equal(
        spy_set_local_arg_value->as.fn.closure,
        env );

    *out = NULL;
    ast_destroy(function_definition_node);
}

// Given:
//  - args are valid
//  - root is a function definition node and its child is an empty function node
//  - all allocations will succeed during interpreter_eval call
//  - symbol binding will succeed
// Expected:
//  - build v, a runtime_env_value * such as
//    - v != NULL
//    - v->type == RUNTIME_VALUE_FUNCTION
//    - v->as.fn.function_node == root->children->children[0]
//    - v->as.fn.closure == env
//  - calls runtime_env_set_local with:
//    - e: env
//    - key: root->children->children[0]->children->children[0]->data->data.symbol_value
//    - value: v
//  - the symbol interned by resolver while function symbol name is bound to a deep copy of v
//  - *out == v
//  - returns INTERPRETER_STATUS_OK
static void eval_success_when_function_definition_node_and_binding_succeeds(void **state) {
    (void)state;
    ast *function_definition_node = a_function_definition_node_with_empty_function();
    will_return(spy_runtime_env_set_local, true);

    interpreter_status status = interpreter_eval(env, function_definition_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_OK);
    assert_true(spy_set_local_has_been_called);
    assert_ptr_equal(spy_set_local_arg_e, env);
    assert_ptr_equal(
        spy_set_local_arg_key,
        function_definition_node->children->children[0]->children->children[0]->data->data.symbol_value );
    assert_non_null(spy_set_local_arg_value);
    assert_int_equal(
        spy_set_local_arg_value->type,
        RUNTIME_VALUE_FUNCTION );
    assert_ptr_equal(
        spy_set_local_arg_value->as.fn.function_node,
        function_definition_node->children->children[0] );
    assert_ptr_equal(
        spy_set_local_arg_value->as.fn.closure,
        env );
    assert_ptr_equal(*out, spy_set_local_arg_value);

    ast_destroy(function_definition_node);
}



//-----------------------------------------------------------------------------
// TESTS interpreter_status interpreter_eval(struct runtime_env *env, const struct ast *root, struct runtime_env_value **out);
// FOR EVALUATION OF AST OF TYPE AST_TYPE_NEGATION
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
  - root
  - out
  - from the runtime_env module:
    - runtime_env_make_number
    - runtime_env_make_string
    - runtime_env_value_destroy
  - from the ast module:
    - ast_create_int_node
    - ast_create_string_node
    - ast_create_children_node_var

doubles:
  - dummy:
    - env
  - spy:
    - runtime_env_set_local (to check no binding)
    - spy_set_local_has_been_called
  - fake:
    - functions of standard libray which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int eval_negation_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    // mock/spy
    runtime_env_set_set_local(spy_runtime_env_set_local);
    spy_set_local_has_been_called = false;

    // dummy
    env = DUMMY_RUNTIME_ENV_P;

    // real
    out = fake_malloc(sizeof(runtime_env_value *));
    *out = NULL;

    return 0;
}

static int eval_negation_teardown(void **state) {
    (void)state;
    if (out) {
        if (*out) {
            runtime_env_value_destroy(*out);
            *out = NULL;
        }
        fake_free(out);
        out = NULL;
    }
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
    runtime_env_set_set_local(NULL);
    fake_memory_reset();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - env and out are valid
//  - root is a ill-formed negation node because:
//    - root->children == NULL
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static void eval_error_invalid_ast_when_negation_node_ill_formed_cause_children_null(void **state) {
    (void)state;
    ast *negation_node = a_ill_formed_negation_node(ILL_CHILDREN_NULL);
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    interpreter_status status = interpreter_eval(env, negation_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_INVALID_AST);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(negation_node);
}

// Given:
//  - env and out are valid
//  - root is a ill-formed negation node because:
//    - root->children != NULL && root->children->children == NULL
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static void eval_error_invalid_ast_when_negation_node_ill_formed_cause_no_child(void **state) {
    (void)state;
    ast *negation_node = a_ill_formed_negation_node(ILL_NO_CHILD);
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    interpreter_status status = interpreter_eval(env, negation_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_INVALID_AST);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(negation_node);
}

// Given:
//  - env and out are valid
//  - root is a ill-formed negation node because:
//    - root->children != NULL && root->children->children_nb == 2
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static void eval_error_invalid_ast_when_negation_node_ill_formed_cause_two_children(void **state) {
    (void)state;
    ast *negation_node = a_ill_formed_negation_node(ILL_TWO_CHILDREN);
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    interpreter_status status = interpreter_eval(env, negation_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_INVALID_AST);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(negation_node);
}

// Given:
//  - env and out are valid
//  - root is a well-formed negation node with 1 child
//  - but the child evaluates to a non-number
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_TYPE_ERROR
static void eval_error_type_error_when_negation_child_not_number(void **state) {
    (void)state;
    ast *negation_node = a_well_formed_negation_of_string();
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    interpreter_status status = interpreter_eval(env, negation_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_TYPE_ERROR);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(negation_node);
}

// Given:
//  - args are well-formed
//  - root is a negation node with one child
//  - root->children->children[0]->data->type == TYPE_INT
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static void eval_error_oom_when_negation_node_and_malloc_fails(void **state) {
    (void)state;
    ast *negation_node = a_negation_node_with_a_number(A_INT);
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    // Simulate a total memory allocation failure
    fake_memory_fail_on_all_call();

    interpreter_status status = interpreter_eval(env, negation_node, out);

    // Restore normal allocation behavior
    fake_memory_fail_on_calls(0, NULL);

    assert_int_equal(status, INTERPRETER_STATUS_OOM);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(negation_node);
}

// Given:
//  - args are well-formed
//  - root is a negation node
//  - root->children->children[0]->data->type == TYPE_INT
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - no binding in env->binding
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_NUMBER
//    && (*out)->as.i == - root->children->children[0]->data->data->int_value
//  - returns INTERPRETER_STATUS_OK
static void eval_success_when_negation_node_and_malloc_succeeds(void **state) {
    (void)state;
    ast *negation_node = a_negation_node_with_a_number(A_INT);

    interpreter_status status = interpreter_eval(env, negation_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_OK);
    assert_non_null(*out);
    assert_int_equal((*out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal(
        (*out)->as.i,
        - negation_node->children->children[0]->data->data.int_value );
    assert_false(spy_set_local_has_been_called);

    ast_destroy(negation_node);
}



//-----------------------------------------------------------------------------
// TESTS interpreter_status interpreter_eval(struct runtime_env *env, const struct ast *root, struct runtime_env_value **out);
// FOR EVALUATION OF AST OF TYPE AST_TYPE_ADDITION
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
  - root
  - out
  - from the runtime_env module:
    - runtime_env_make_number
    - runtime_env_make_string
    - runtime_env_value_destroy
  - from the ast module:
    - ast_create_int_node
    - ast_create_string_node
    - ast_create_children_node_var

doubles:
  - dummy:
    - env
  - spy:
    - runtime_env_set_local (to check no binding)
    - spy_set_local_has_been_called
  - fake:
    - functions of standard libray which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int eval_addition_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    // mock/spy
    runtime_env_set_set_local(spy_runtime_env_set_local);
    spy_set_local_has_been_called = false;

    // dummy
    env = DUMMY_RUNTIME_ENV_P;

    // real
    out = fake_malloc(sizeof(runtime_env_value *));
    *out = NULL;

    return 0;
}

static int eval_addition_teardown(void **state) {
    (void)state;
    if (out) {
        if (*out) {
            runtime_env_value_destroy(*out);
            *out = NULL;
        }
        fake_free(out);
        out = NULL;
    }
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
    runtime_env_set_set_local(NULL);
    fake_memory_reset();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - env and out are valid
//  - root is a ill-formed addition node because:
//    - root->children == NULL
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static void eval_error_invalid_ast_when_addition_node_ill_formed_cause_children_null(void **state) {
    (void)state;
    ast *addition_node = a_ill_formed_addition_node(ILL_CHILDREN_NULL);
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    interpreter_status status = interpreter_eval(env, addition_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_INVALID_AST);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(addition_node);
}

// Given:
//  - env and out are valid
//  - root is a ill-formed addition node because:
//    - root->children->children == NULL
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static void eval_error_invalid_ast_when_addition_node_ill_formed_cause_no_child(void **state) {
    (void)state;
    ast *addition_node = a_ill_formed_addition_node(ILL_NO_CHILD);
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    interpreter_status status = interpreter_eval(env, addition_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_INVALID_AST);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(addition_node);
}

// Given:
//  - env and out are valid
//  - root is a ill-formed addition node because:
//    - root->children->children_nb == 1
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static void eval_error_invalid_ast_when_addition_node_ill_formed_cause_one_child(void **state) {
    (void)state;
    ast *addition_node = a_ill_formed_addition_node(ILL_ONE_CHILD);
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    interpreter_status status = interpreter_eval(env, addition_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_INVALID_AST);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(addition_node);
}

// Given:
//  - env and out are valid
//  - root is a ill-formed addition node because:
//    - root->children->children_nb == 3
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static void eval_error_invalid_ast_when_addition_node_ill_formed_cause_three_children(void **state) {
    (void)state;
    ast *addition_node = a_ill_formed_addition_node(ILL_THREE_CHILDREN);
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    interpreter_status status = interpreter_eval(env, addition_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_INVALID_AST);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(addition_node);
}

// Given:
//  - env and out are valid
//  - root is a well-formed addition node
//  - but the second child evaluates to a non-number
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_TYPE_ERROR
static void eval_error_type_error_when_addition_and_second_child_not_number(void **state) {
    (void)state;
    ast *addition_node = a_well_formed_addition_of_number_and_string();
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    interpreter_status status = interpreter_eval(env, addition_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_TYPE_ERROR);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(addition_node);
}

// Given:
//  - args are well-formed
//  - root is a well-formed addition node
//  - the two children evaluate to numbers
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static void eval_error_oom_when_addition_node_and_malloc_fails(void **state) {
    (void)state;
    ast *addition_node = a_well_formed_addition_of_two_numbers();
    runtime_env_value *sentinel = (runtime_env_value *)0x1;
    *out = sentinel;
    runtime_env_value **old_out = out;

    // Simulate a total memory allocation failure
    fake_memory_fail_on_all_call();

    interpreter_status status = interpreter_eval(env, addition_node, out);

    // Restore normal allocation behavior
    fake_memory_fail_on_calls(0, NULL);

    assert_int_equal(status, INTERPRETER_STATUS_OOM);
    assert_ptr_equal(out, old_out);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    ast_destroy(addition_node);
}

// Given:
//  - args are well-formed
//  - root is a well-formed addition node
//  - the two children evaluate to numbers
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - no binding in env->binding
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_NUMBER
//    && (*out)->as.i ==
//           root->children->children[0]->data->data->int_value
//           +
//           root->children->children[1]->data->data->int_value
//  - returns INTERPRETER_STATUS_OK
static void eval_success_when_addition_node_and_malloc_succeeds(void **state) {
    (void)state;
    ast *addition_node = a_well_formed_addition_of_two_numbers();

    interpreter_status status = interpreter_eval(env, addition_node, out);

    assert_int_equal(status, INTERPRETER_STATUS_OK);
    assert_non_null(*out);
    assert_int_equal((*out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal(
        (*out)->as.i,
        addition_node->children->children[0]->data->data.int_value
        +
        addition_node->children->children[1]->data->data.int_value );
    assert_false(spy_set_local_has_been_called);

    ast_destroy(addition_node);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
    const struct CMUnitTest _tests[] = {

        // invalid args
        cmocka_unit_test_setup_teardown(
            eval_error_when_env_null,
            eval_with_invalid_args_setup, eval_with_invalid_args_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_when_root_null,
            eval_with_invalid_args_setup, eval_with_invalid_args_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_when_out_null,
            eval_with_invalid_args_setup, eval_with_invalid_args_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_when_unsupported_root_type,
            eval_with_invalid_args_setup, eval_with_invalid_args_teardown),

        // AST_TYPE_DATA_WRAPPER ; TYPE_INT
        cmocka_unit_test_setup_teardown(
            eval_error_oom_when_int_node_and_malloc_fails,
            eval_leaf_setup, eval_teardown),
        cmocka_unit_test_setup_teardown(
            eval_success_when_int_node_and_malloc_succeeds,
            eval_leaf_setup, eval_teardown),

        // AST_TYPE_DATA_WRAPPER ; TYPE_STRING
        cmocka_unit_test_setup_teardown(
            eval_error_oom_when_string_node_and_malloc_fails,
            eval_leaf_setup, eval_teardown),
        cmocka_unit_test_setup_teardown(
            eval_success_when_string_node_and_malloc_succeeds,
            eval_leaf_setup, eval_teardown),

        // AST_TYPE_DATA_WRAPPER ; TYPE_SYMBOL
        cmocka_unit_test_setup_teardown(
            eval_error_oom_when_symbol_node_and_malloc_fails,
            eval_leaf_setup, eval_teardown),
        cmocka_unit_test_setup_teardown(
            eval_success_when_symbol_node_and_malloc_succeeds,
            eval_leaf_setup, eval_teardown),

        // AST_TYPE_ERROR
        cmocka_unit_test_setup_teardown(
            eval_error_oom_when_error_node_not_sentinel_and_malloc_fails,
            eval_leaf_setup, eval_teardown),
        cmocka_unit_test_setup_teardown(
            eval_success_when_error_node_not_sentinel_and_malloc_succeeds,
            eval_leaf_setup, eval_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_oom_when_error_node_sentinel_and_malloc_fails,
            eval_leaf_setup, eval_teardown),
        cmocka_unit_test_setup_teardown(
            eval_success_when_error_node_sentinel_and_malloc_succeeds,
            eval_leaf_setup, eval_teardown),

        // AST_TYPE_FUNCTION
        cmocka_unit_test_setup_teardown(
            eval_error_oom_when_function_node_and_malloc_fails,
            eval_function_setup, eval_function_teardown),
        cmocka_unit_test_setup_teardown(
            eval_success_when_function_node_and_malloc_succeeds,
            eval_function_setup, eval_function_teardown),

        // AST_TYPE_FUNCTION_DEFINITION
        cmocka_unit_test_setup_teardown(
            eval_error_oom_when_function_definition_node_and_malloc_fails,
            eval_function_definition_setup, eval_function_definition_teardown),
        cmocka_unit_test_setup_teardown(
            eval_binding_error_when_function_definition_node_and_binding_fails,
            eval_function_definition_setup, eval_function_definition_teardown),
        cmocka_unit_test_setup_teardown(
            eval_success_when_function_definition_node_and_binding_succeeds,
            eval_function_definition_setup, eval_function_definition_teardown),

        // AST_TYPE_NEGATION
        cmocka_unit_test_setup_teardown(
            eval_error_invalid_ast_when_negation_node_ill_formed_cause_children_null,
            eval_negation_setup, eval_negation_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_invalid_ast_when_negation_node_ill_formed_cause_no_child,
            eval_negation_setup, eval_negation_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_invalid_ast_when_negation_node_ill_formed_cause_no_child,
            eval_negation_setup, eval_negation_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_invalid_ast_when_negation_node_ill_formed_cause_two_children,
            eval_negation_setup, eval_negation_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_type_error_when_negation_child_not_number,
            eval_negation_setup, eval_negation_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_oom_when_negation_node_and_malloc_fails,
            eval_negation_setup, eval_negation_teardown),
        cmocka_unit_test_setup_teardown(
            eval_success_when_negation_node_and_malloc_succeeds,
            eval_negation_setup, eval_negation_teardown),

        // AST_TYPE_ADDITION
        cmocka_unit_test_setup_teardown(
            eval_error_invalid_ast_when_addition_node_ill_formed_cause_children_null,
            eval_addition_setup, eval_addition_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_invalid_ast_when_addition_node_ill_formed_cause_no_child,
            eval_addition_setup, eval_addition_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_invalid_ast_when_addition_node_ill_formed_cause_one_child,
            eval_addition_setup, eval_addition_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_invalid_ast_when_addition_node_ill_formed_cause_three_children,
            eval_addition_setup, eval_addition_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_type_error_when_addition_and_second_child_not_number,
            eval_addition_setup, eval_addition_teardown),
        cmocka_unit_test_setup_teardown(
            eval_error_oom_when_addition_node_and_malloc_fails,
            eval_addition_setup, eval_addition_teardown),
        cmocka_unit_test_setup_teardown(
            eval_success_when_addition_node_and_malloc_succeeds,
            eval_addition_setup, eval_addition_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(_tests, NULL, NULL);

    return failed;
}
