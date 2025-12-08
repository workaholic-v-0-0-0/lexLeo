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

#include "internal/interpreter_internal.h"
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


static const int A_INT_NOT_ZERO = 7;
static const runtime_env_value **out = NULL;
static runtime_env *env = NULL;
static const char *A_CONSTANT_STRING = "abc";
static ast_error_type AN_AST_ERROR_TYPE = AST_ERROR_CODE_BINDING_NODE_CREATION_FAILED;
static const char *A_CONSTANT_ERROR_MESSAGE = "an error message";
static runtime_env_value A_NUMBER_RUNTIME_ENV_VALUE = {.type = RUNTIME_VALUE_NUMBER, .refcount = 2, .as.i = A_INT_NOT_ZERO};
static runtime_env_value *A_NUMBER_RUNTIME_ENV_VALUE_P = &A_NUMBER_RUNTIME_ENV_VALUE;
static runtime_env_value A_STRING_RUNTIME_ENV_VALUE = {.type = RUNTIME_VALUE_STRING, .refcount = 2, .as.s = "a string"};
static runtime_env_value *A_STRING_RUNTIME_ENV_VALUE_P = &A_STRING_RUNTIME_ENV_VALUE;
static runtime_env_value *SENTINEL_TO_BE_REPLACED_BY_NUMBER = (runtime_env_value*)0xDEADF00D;



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
static symbol DUMMY_SYMBOL = {.name = "dummy_symbol_name",};
static symbol *DUMMY_SYMBOL_P = &DUMMY_SYMBOL;
static symbol DUMMY_OTHER_SYMBOL = {.name = "an_other_symbol",};
static symbol *DUMMY_OTHER_SYMBOL_P = &DUMMY_OTHER_SYMBOL;
static runtime_env_value DUMMY_RUNTIME_ENV_VALUE = {.type = RUNTIME_VALUE_NUMBER, .refcount = 2, .as.i = A_INT_NOT_ZERO};
static runtime_env_value *const DUMMY_RUNTIME_ENV_VALUE_P = &DUMMY_RUNTIME_ENV_VALUE;
static symbol DUMMY_SYMBOL_FOR_FUNCTION_NAME = {.name = "fonction_name",};
static symbol *DUMMY_SYMBOL_FOR_FUNCTION_NAME_P = &DUMMY_SYMBOL_FOR_FUNCTION_NAME;
static void *const dummy_runtime_session = (void*)0xDEADBEEF;


// mocks


// spies

typedef enum {
    ENV_ARR_NONE = 0,
	ENV_ARR_GET_RET_NULL,
    ENV_ARR_GET_RET_STR_ABC,
	ENV_ARR_GET_RET_NUM_42,
	ENV_ARR_GET_RET_NUM_42_RCNT_2,
	ENV_ARR_GET_RET_NUM_0,
	ENV_ARR_GET_RET_QUOTED_ADD_1_PLUS_2,
	ENV_ARR_SET_LOCAL_RET_FALSE,
	ENV_ARR_SET_LOCAL_RET_TRUE
} interpreter_runtime_env_arrange_kind_t;

typedef enum {
    ENV_ASRT_NONE = 0,
	ENV_ASRT_EXPECT_GET_CALLED_ON_ROOT_SYMBOL,
    ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
    ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL,
	ENV_ASRT_EXPECT_SET_LOCAL_DEF_FCT,
	ENV_ASRT_EXPECT_SET_LOCAL_BND_CHILD_TO_READ,
	ENV_ASRT_EXPECT_SET_LOCAL_BND_LHS_TO_NUM_42
} interpreter_runtime_env_assert_kind_t;

typedef struct test_interpreter_runtime_value_snapshot {
	const runtime_env_value *address;
	runtime_env_value value;
} test_interpreter_runtime_value_snapshot;

static void make_runtime_env_value_snapshot(const runtime_env_value *v, test_interpreter_runtime_value_snapshot *snapshot) {
	if (!v) return;
	snapshot->address = v;
	memcpy(&snapshot->value, v, sizeof(runtime_env_value));
}

typedef struct {
    runtime_env *set_local_arg_e;
    const struct symbol *set_local_arg_key;
    const runtime_env_value *set_local_arg_value;
	test_interpreter_runtime_value_snapshot set_local_arg_value_snapshot;
	bool set_local_ret;

    const runtime_env *get_local_arg_e;
    const struct symbol *get_local_arg_key;
	runtime_env_value *get_local_ret;
	test_interpreter_runtime_value_snapshot get_local_ret_snapshot;

    const runtime_env *get_arg_e;
    const struct symbol *get_arg_key;
	runtime_env_value *get_ret;
	test_interpreter_runtime_value_snapshot get_ret_snapshot;
} interpreter_runtime_spy_t;

static interpreter_runtime_spy_t *g_runtime_spy = NULL;

bool spy_runtime_env_set_local(runtime_env *e, const struct symbol *key, const runtime_env_value *value) {
    assert_non_null(g_runtime_spy);
    g_runtime_spy->set_local_arg_e = e;
    g_runtime_spy->set_local_arg_key = key;
    g_runtime_spy->set_local_arg_value = value;// useless?
	make_runtime_env_value_snapshot(value, &g_runtime_spy->set_local_arg_value_snapshot);
    if (g_runtime_spy->set_local_ret) runtime_env_value_retain(value);
	return g_runtime_spy->set_local_ret;
}

const runtime_env_value *spy_runtime_env_get_local(const runtime_env *e, const struct symbol *key) {
    assert_non_null(g_runtime_spy);
    g_runtime_spy->get_local_arg_e = e;
    g_runtime_spy->get_local_arg_key = key;
    make_runtime_env_value_snapshot(g_runtime_spy->get_ret, &g_runtime_spy->get_local_ret_snapshot);
	return g_runtime_spy->get_local_ret;
}

const runtime_env_value *spy_runtime_env_get(const runtime_env *e, const struct symbol *key) {
    assert_non_null(g_runtime_spy);
    g_runtime_spy->get_arg_e = e;
    g_runtime_spy->get_arg_key = key;
    make_runtime_env_value_snapshot(g_runtime_spy->get_ret, &g_runtime_spy->get_ret_snapshot);
	return g_runtime_spy->get_ret;
}

typedef enum {
    READ_AST_ARR_NONE = 0,
	READ_AST_ARR_READ_FAIL,
	READ_AST_ARR_READ_NUM_42
} interpreter_read_ast_arrange_kind_t;

typedef enum {
    READ_AST_ASRT_NONE = 0,
	READ_AST_ASRT_CALL
} interpreter_read_ast_assert_kind_t;

typedef struct {
    const struct interpreter_ctx *read_ast_fn_arg_ctx;
    ast *read_ast_ret;
} interpreter_read_ast_spy_t;

static interpreter_read_ast_spy_t *g_read_ast_spy = NULL;

static ast *spy_read_ast_fn (const struct interpreter_ctx *ctx) {
    assert_non_null(g_read_ast_spy);
    g_read_ast_spy->read_ast_fn_arg_ctx = ctx;
    return g_read_ast_spy->read_ast_ret;
}

typedef enum {
    WRITE_RUNTIME_VALUE_ARR_NONE = 0,
	WRITE_RUNTIME_VALUE_ARR_WRITE_FAIL,
	WRITE_RUNTIME_VALUE_ARR_WRITE_SUCCESS,
} interpreter_write_runtime_value_arrange_kind_t;

typedef enum {
    WRITE_RUNTIME_VALUE_ASRT_NONE = 0,
	WRITE_RUNTIME_VALUE_ASRT_WRITE_NUM_42,
	WRITE_RUNTIME_VALUE_ASRT_WRITE_NUM_3,
	WRITE_RUNTIME_VALUE_ASRT_WRITE_NUM_42_RCNT_2,
	WRITE_RUNTIME_VALUE_ASRT_WRITE_STR_ABC,
	WRITE_RUNTIME_VALUE_ASRT_WRITE_QUOTED_ADD_1_PLUS_2
} interpreter_write_runtime_value_assert_kind_t;

typedef struct {
    const struct interpreter_ctx *write_runtime_value_fn_arg_ctx;
    const runtime_env_value *write_runtime_value_fn_arg_value;
	test_interpreter_runtime_value_snapshot write_runtime_value_fn_arg_value_snapshot;
	bool write_runtime_value_ret;
} interpreter_write_runtime_value_spy_t;

static interpreter_write_runtime_value_spy_t *g_write_runtime_value_spy = NULL;

static bool spy_write_runtime_value_fn (
        const struct interpreter_ctx *ctx,
        const struct runtime_env_value *value ) {

    assert_non_null(g_write_runtime_value_spy);
    g_write_runtime_value_spy->write_runtime_value_fn_arg_ctx = ctx;
    g_write_runtime_value_spy->write_runtime_value_fn_arg_value = value;
	make_runtime_env_value_snapshot(value, &g_write_runtime_value_spy->write_runtime_value_fn_arg_value_snapshot);
	// no retain!
    return g_write_runtime_value_spy->write_runtime_value_ret;
}

static const interpreter_ops_t SPY_INTERPRETER_OPS = {
    .read_ast_fn = spy_read_ast_fn,
    .write_runtime_value_fn = spy_write_runtime_value_fn
};

static struct interpreter_ctx spy_interpreter_ctx = {
    .ops = &SPY_INTERPRETER_OPS,
    .host_ctx = dummy_runtime_session
};


// stubs


// fakes

#define FAKABLE_MALLOC(n) (get_current_malloc()(n))
#define FAKABLE_FREE(p) (get_current_free()(p))
#define FAKABLE_STRDUP(s) (get_current_string_duplicate()(s))



//-----------------------------------------------------------------------------
// TESTS
// interpreter_status interpreter_eval(
//         struct runtime_env *env,
//         const struct ast *root,
//         struct runtime_env_value **out );
//-----------------------------------------------------------------------------


// At every test:
// Expected:
//  - no invalid free
//  - no double free
//  - no memory leak



//-----------------------------------------------------------------------------
// NON PARAMETRIC TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// INVALID ARGUMENTS HANDLING
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
    - functions of standard library which are used:
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
    const runtime_env_value **arg_out_before = out;

    interpreter_status status = interpreter_eval(&spy_interpreter_ctx, NULL, DUMMY_AST_P, out);

    assert_int_equal(status, INTERPRETER_STATUS_ERROR);
    assert_ptr_equal(out, arg_out_before);
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
    const runtime_env_value **arg_out_before = out;

    interpreter_status status = interpreter_eval(&spy_interpreter_ctx, DUMMY_RUNTIME_ENV_P, NULL, out);

    assert_int_equal(status, INTERPRETER_STATUS_ERROR);
    assert_ptr_equal(out, arg_out_before);
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

    interpreter_status status = interpreter_eval(&spy_interpreter_ctx, DUMMY_RUNTIME_ENV_P, DUMMY_AST_P, NULL);

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
    const runtime_env_value **arg_out_before = out;

    interpreter_status status = interpreter_eval(&spy_interpreter_ctx, DUMMY_RUNTIME_ENV_P, unsupported_ast, out);

    assert_int_equal(status, INTERPRETER_STATUS_ERROR);
    assert_ptr_equal(out, arg_out_before);
    assert_ptr_equal(*out, sentinel);

    *out = NULL;
    fake_free(out);
    fake_free(unsupported_ast);
}



//-----------------------------------------------------------------------------
// PARAMETRIC TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// PARAMETRIC CASE STRUCTURE
//-----------------------------------------------------------------------------


typedef struct {
	// args
    struct interpreter_ctx *arg_ctx;
    runtime_env *arg_env;
    const ast *arg_root;

	// spy env interaction
    const runtime_env_value **arg_out;
    const runtime_env_value **arg_out_before;
    const runtime_env_value *arg_out_value_before;
	interpreter_runtime_env_arrange_kind_t runtime_env_arrange_kind;
	interpreter_runtime_env_assert_kind_t runtime_env_assert_kind;
    interpreter_runtime_spy_t runtime_spy;

	// spy read_ast callback
	interpreter_read_ast_arrange_kind_t read_ast_arrange_kind;
	interpreter_read_ast_assert_kind_t read_ast_assert_kind;
    interpreter_read_ast_spy_t read_ast_spy;

	// spy write_runtime_value callback
	interpreter_write_runtime_value_arrange_kind_t write_runtime_value_arrange_kind;
	interpreter_write_runtime_value_assert_kind_t write_runtime_value_assert_kind;
    interpreter_write_runtime_value_spy_t spy_write_runtime_value;
} test_interpreter_ctx;

typedef void (*test_interpreter_root_constructor_fn_t)(test_interpreter_ctx *ctx);
typedef void (*test_interpreter_expected_out_fn_t)(test_interpreter_ctx *ctx);
typedef void (*test_interpreter_clean_up_fn_t)(test_interpreter_ctx *ctx);

typedef struct {

    // test name
    const char *name;

    // arrange utilities
    test_interpreter_root_constructor_fn_t root_constructor_fn;
    bool env_is_dummy;
    bool oom;
	interpreter_runtime_env_arrange_kind_t runtime_env_arrange_kind;
	interpreter_read_ast_arrange_kind_t read_ast_arrange_kind;
	interpreter_write_runtime_value_arrange_kind_t write_runtime_value_arrange_kind;

    // assert utilities
    test_interpreter_expected_out_fn_t expected_out_fn;
    interpreter_status expected_status;
	interpreter_runtime_env_assert_kind_t runtime_env_assert_kind;
	interpreter_read_ast_assert_kind_t read_ast_assert_kind;
	interpreter_write_runtime_value_assert_kind_t write_runtime_value_assert_kind;

    // test infrastructure cleanup utilities
    test_interpreter_clean_up_fn_t clean_up_fn;

    // information sharing utilities
    test_interpreter_ctx *ctx;

} test_interpreter_case;



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int parametric_setup(void **state) {
    test_interpreter_case *p = (test_interpreter_case *) *state;

    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    p->ctx->arg_ctx = &spy_interpreter_ctx;

    if (!p->env_is_dummy) {
        p->ctx->arg_env = runtime_env_wind(NULL);
        assert_non_null(p->ctx->arg_env);
    }
    else p->ctx->arg_env = DUMMY_RUNTIME_ENV_P;

    // globals initialization so that spies could act on spy variables of test context
    p->ctx->runtime_spy = (interpreter_runtime_spy_t){0};
    p->ctx->read_ast_spy = (interpreter_read_ast_spy_t){0};
    p->ctx->spy_write_runtime_value = (interpreter_write_runtime_value_spy_t){0};
    g_runtime_spy = &p->ctx->runtime_spy;
    g_read_ast_spy = &p->ctx->read_ast_spy;
	g_write_runtime_value_spy = &p->ctx->spy_write_runtime_value;

	// spies initialization
    runtime_env_set_set_local(spy_runtime_env_set_local);
    runtime_env_set_get_local(spy_runtime_env_get_local);
    runtime_env_set_get(spy_runtime_env_get);

    p->ctx->arg_out = fake_malloc(sizeof *p->ctx->arg_out);
    *p->ctx->arg_out = NULL;

    return 0;
}

static int parametric_teardown(void **state) {
    test_interpreter_case *p = (test_interpreter_case *) *state;

    if (p->clean_up_fn) p->clean_up_fn(p->ctx);

    if (p->ctx->arg_out) {
        fake_free(p->ctx->arg_out);
        p->ctx->arg_out = NULL;
    }

    if (p->ctx->arg_env && !p->env_is_dummy) {
        runtime_env_unwind(p->ctx->arg_env);
        p->ctx->arg_env = NULL;
    }

    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());

    g_runtime_spy = NULL;
	g_read_ast_spy = NULL;
	g_write_runtime_value_spy = NULL;

    runtime_env_set_set_local(NULL);
    runtime_env_set_get_local(NULL);
    runtime_env_set_get(NULL);

    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
    fake_memory_reset();

    return 0;
}



//-----------------------------------------------------------------------------
// PARAMETRIC TEST RUNNER
//-----------------------------------------------------------------------------


// forward declarations
static void expected_out_unchanged(test_interpreter_ctx *ctx);
static void spy_runtime_env_arrange(const test_interpreter_case *);
static void spy_read_ast_arrange(const test_interpreter_case *);
static void spy_write_runtime_value_arrange(const test_interpreter_case *);
static void runtime_env_usage_expectation(test_interpreter_ctx *ctx);
static void read_ast_usage_expectation(test_interpreter_ctx *ctx);
static void write_runtime_value_usage_expectation(test_interpreter_ctx *ctx);

static void eval_test(void **state) {
    test_interpreter_case *p = (test_interpreter_case *) *state;

    // ARRANGE
    p->root_constructor_fn(p->ctx);

    // out snapshot to check its invariance
    if (p->expected_out_fn == &expected_out_unchanged) {
        p->ctx->arg_out_before = p->ctx->arg_out;
        p->ctx->arg_out_value_before = *p->ctx->arg_out;
    }

	// spies arrange
    spy_runtime_env_arrange(p);
    spy_read_ast_arrange(p);
    spy_write_runtime_value_arrange(p);


    // ACT

    // Simulate a total memory allocation failure
    if (p->oom)
        fake_memory_fail_on_all_call();

    interpreter_status status =
        interpreter_eval(
            p->ctx->arg_ctx,
            p->ctx->arg_env,
            p->ctx->arg_root,
            p->ctx->arg_out );

    // Restore normal allocation behavior
    if (p->oom)
        fake_memory_fail_on_calls(0, NULL);


    // ASSERT

    assert_int_equal(status, p->expected_status);
	runtime_env_usage_expectation(p->ctx);
	read_ast_usage_expectation(p->ctx);
    write_runtime_value_usage_expectation(p->ctx);
    if (p->expected_out_fn) p->expected_out_fn(p->ctx);
}



//-----------------------------------------------------------------------------
// ROOT CONSTRUCTORS
//-----------------------------------------------------------------------------


static ast *an_empty_function_node_with_dummy_name() {
    // function name
    ast *dummy_function_name = ast_create_symbol_node(DUMMY_SYMBOL_P);

    // list of params
    ast *empty_list_of_params =
        ast_create_children_node_var(
            AST_TYPE_LIST_OF_PARAMETERS,
            1,
            ast_create_children_node_var(
                AST_TYPE_PARAMETERS,
                0 ) );

    // body
    ast *empty_body =
        ast_create_children_node_var(
            AST_TYPE_BLOCK,
            1,
            ast_create_children_node_var(
                AST_TYPE_BLOCK_ITEMS,
                0 ) );

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
                ast_create_int_node(A_INT_NOT_ZERO) );

        case ILL_TWO_CHILDREN:
            return ast_create_children_node_var(
                type,
                2,
                ast_create_int_node(A_INT_NOT_ZERO),
                ast_create_int_node(A_INT_NOT_ZERO) );

        case ILL_THREE_CHILDREN:
            return ast_create_children_node_var(
                type,
                3,
                ast_create_int_node(A_INT_NOT_ZERO),
                ast_create_int_node(A_INT_NOT_ZERO),
                ast_create_int_node(A_INT_NOT_ZERO) );
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
static ast *make_unary_with_symbol(ast_type type, symbol *sym) {
    return ast_create_children_node_var(type, 1, ast_create_symbol_node(sym));
}

static ast *a_ill_formed_function_node(illness_type how) {
    return make_ill_formed_node(AST_TYPE_FUNCTION, how);
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
static ast *a_negation_node_with_a_symbol(symbol *sym) {
    return make_unary_with_symbol(AST_TYPE_NEGATION, sym);
}

static ast *a_ill_formed_addition_node(illness_type how) {
    return make_ill_formed_node(AST_TYPE_ADDITION, how);
}
static ast *a_well_formed_addition_of_number_and_string() {
    return ast_create_children_node_var(
        AST_TYPE_ADDITION,
        2,
        ast_create_int_node(A_INT_NOT_ZERO),
        ast_create_string_node(A_CONSTANT_STRING) );
}
static ast *a_well_formed_addition_of_two_numbers() {
    return ast_create_children_node_var(
        AST_TYPE_ADDITION,
        2,
        ast_create_int_node(A_INT_NOT_ZERO),
        ast_create_int_node(A_INT_NOT_ZERO) );
}
static void make_root_a_well_formed_addition_with_lhs_number_rhs_symbol(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_ADDITION,
            2,
            ast_create_int_node(A_INT_NOT_ZERO),
            ast_create_symbol_node(DUMMY_SYMBOL_P) );
}

static ast *a_function_node_with_duplicate_param(void) {
    return
        ast_create_children_node_var(
            AST_TYPE_FUNCTION,
            3,
            ast_create_symbol_node(DUMMY_SYMBOL_P),
            ast_create_children_node_var(
                AST_TYPE_LIST_OF_PARAMETERS,
                1,
                ast_create_children_node_var(
                    AST_TYPE_PARAMETERS,
                    3,
                    ast_create_symbol_node(DUMMY_SYMBOL_P),
                    ast_create_symbol_node(DUMMY_OTHER_SYMBOL_P),
                    ast_create_symbol_node(DUMMY_SYMBOL_P) ) ),
            ast_create_children_node_var(
                AST_TYPE_BLOCK,
                1,
                ast_create_children_node_var(
                    AST_TYPE_BLOCK_ITEMS,
                    0 ) ) );
}

static ast *a_ill_formed_subtraction_node(illness_type how) {
    return make_ill_formed_node(AST_TYPE_SUBTRACTION, how);
}
static ast *a_well_formed_subtraction_of_two_numbers() {
    return ast_create_children_node_var(
        AST_TYPE_SUBTRACTION,
        2,
        ast_create_int_node(A_INT_NOT_ZERO),
        ast_create_int_node(A_INT_NOT_ZERO) );
}

static void make_root_a_well_formed_subtraction_with_lhs_number_rhs_symbol(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_SUBTRACTION,
            2,
            ast_create_int_node(A_INT_NOT_ZERO),
            ast_create_symbol_node(DUMMY_SYMBOL_P) );
}

static ast *a_ill_formed_multiplication_node(illness_type how) {
    return make_ill_formed_node(AST_TYPE_MULTIPLICATION, how);
}
static ast *a_well_formed_multiplication_of_two_numbers() {
    return ast_create_children_node_var(
        AST_TYPE_MULTIPLICATION,
        2,
        ast_create_int_node(A_INT_NOT_ZERO),
        ast_create_int_node(A_INT_NOT_ZERO) );
}
static void make_root_a_well_formed_multiplication_with_lhs_number_rhs_symbol(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_MULTIPLICATION,
            2,
            ast_create_int_node(A_INT_NOT_ZERO),
            ast_create_symbol_node(DUMMY_SYMBOL_P) );
}

static ast *a_ill_formed_division_node(illness_type how) {
    return make_ill_formed_node(AST_TYPE_DIVISION, how);
}
static void make_root_a_well_formed_division_by_zero_of_two_numbers(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_DIVISION,
            2,
            ast_create_int_node(A_INT_NOT_ZERO),
            ast_create_int_node(0) );
}
static void make_root_a_well_formed_division_not_by_zero_of_two_numbers(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_DIVISION,
            2,
            ast_create_int_node(A_INT_NOT_ZERO),
            ast_create_int_node(A_INT_NOT_ZERO) );
}
static void make_root_a_well_formed_division_with_lhs_number_rhs_symbol(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_DIVISION,
            2,
            ast_create_int_node(A_INT_NOT_ZERO),
            ast_create_symbol_node(DUMMY_SYMBOL_P) );
}

static void make_root_a_int_node(test_interpreter_ctx *ctx) {
    ctx->arg_root = ast_create_int_node(42);
}

static void make_root_a_string_node_abc(test_interpreter_ctx *ctx) {
    ctx->arg_root = ast_create_string_node("abc");
}

static void make_root_a_symbol_node(test_interpreter_ctx *ctx) {
    ctx->arg_root = ast_create_symbol_node(DUMMY_SYMBOL_P);
}

static void make_root_an_error_not_sentinel_node(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_error_node(
            AST_ERROR_CODE_BINDING_NODE_CREATION_FAILED,
            A_CONSTANT_ERROR_MESSAGE );
}

static void make_root_the_error_sentinel_node(test_interpreter_ctx *ctx) {
    ctx->arg_root = ast_error_sentinel();
}

static void make_root_a_ill_formed_function_node_because_children_null(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_function_node(ILL_CHILDREN_NULL);
}

static void make_root_a_ill_formed_function_node_because_no_child(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_function_node(ILL_NO_CHILD);
}

static void make_root_a_ill_formed_function_node_because_two_children(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_function_node(ILL_TWO_CHILDREN);
}

static void make_root_a_function_node_with_duplicate_param(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_function_node_with_duplicate_param();
}

static void make_root_an_empty_function_node_with_dummy_name(test_interpreter_ctx *ctx) {
    ctx->arg_root = an_empty_function_node_with_dummy_name();
}

static void make_root_a_ill_formed_function_definition_node_because_children_null(test_interpreter_ctx *ctx) {
    ctx->arg_root = make_ill_formed_node(AST_TYPE_FUNCTION_DEFINITION, ILL_CHILDREN_NULL);
}

static void make_root_a_ill_formed_function_definition_node_because_no_child(test_interpreter_ctx *ctx) {
    ctx->arg_root = make_ill_formed_node(AST_TYPE_FUNCTION_DEFINITION, ILL_NO_CHILD);
}

static void make_root_a_ill_formed_function_definition_node_because_two_children(test_interpreter_ctx *ctx) {
    ctx->arg_root = make_ill_formed_node(AST_TYPE_FUNCTION_DEFINITION, ILL_TWO_CHILDREN);
}

static void make_root_a_function_definition_node_with_empty_function(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_function_definition_node_with_empty_function();
}

static void make_root_a_ill_formed_negation_node_because_children_null(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_negation_node(ILL_CHILDREN_NULL);
}

static void make_root_a_ill_formed_negation_node_because_no_child(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_negation_node(ILL_NO_CHILD);
}

static void make_root_a_ill_formed_negation_node_because_two_children(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_negation_node(ILL_TWO_CHILDREN);
}

static void make_root_a_negation_node_with_a_number(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_negation_node_with_a_number(A_INT_NOT_ZERO);
}

static void make_root_a_negation_node_with_a_symbol(test_interpreter_ctx *ctx) {
    ctx->arg_root =  a_negation_node_with_a_symbol(DUMMY_SYMBOL_P);
}

static void make_root_a_ill_formed_addition_node_because_children_null(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_addition_node(ILL_CHILDREN_NULL);
}

static void make_root_a_ill_formed_addition_node_because_no_child(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_addition_node(ILL_NO_CHILD);
}

static void make_root_a_ill_formed_addition_node_because_one_child(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_addition_node(ILL_ONE_CHILD);
}

static void make_root_a_ill_formed_addition_node_because_three_children(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_addition_node(ILL_THREE_CHILDREN);
}

static void make_root_a_well_formed_addition_of_two_numbers(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_well_formed_addition_of_two_numbers();
}

static void make_root_a_ill_formed_subtraction_node_because_children_null(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_subtraction_node(ILL_CHILDREN_NULL);
}

static void make_root_a_ill_formed_subtraction_node_because_no_child(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_subtraction_node(ILL_NO_CHILD);
}

static void make_root_a_ill_formed_subtraction_node_because_one_child(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_subtraction_node(ILL_ONE_CHILD);
}

static void make_root_a_ill_formed_subtraction_node_because_three_children(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_subtraction_node(ILL_THREE_CHILDREN);
}

static void make_root_a_well_formed_subtraction_of_two_numbers(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_well_formed_subtraction_of_two_numbers();
}

static void make_root_a_ill_formed_multiplication_node_because_children_null(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_multiplication_node(ILL_CHILDREN_NULL);
}

static void make_root_a_ill_formed_multiplication_node_because_no_child(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_multiplication_node(ILL_NO_CHILD);
}

static void make_root_a_ill_formed_multiplication_node_because_one_child(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_multiplication_node(ILL_ONE_CHILD);
}

static void make_root_a_ill_formed_multiplication_node_because_three_children(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_multiplication_node(ILL_THREE_CHILDREN);
}

static void make_root_a_well_formed_multiplication_of_two_numbers(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_well_formed_multiplication_of_two_numbers();
}

static void make_root_a_ill_formed_division_node_because_children_null(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_division_node(ILL_CHILDREN_NULL);
}

static void make_root_a_ill_formed_division_node_because_no_child(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_division_node(ILL_NO_CHILD);
}

static void make_root_a_ill_formed_division_node_because_one_child(test_interpreter_ctx *ctx) {
    ctx->arg_root = a_ill_formed_division_node(ILL_ONE_CHILD);
}

static void make_root_a_ill_formed_division_node_because_three_children(test_interpreter_ctx *ctx) {
    ctx->arg_root = make_ill_formed_node(AST_TYPE_DIVISION, ILL_THREE_CHILDREN);
}

static void make_root_a_ill_formed_numbers_node_because_children_null(test_interpreter_ctx *ctx) {
    ctx->arg_root = make_ill_formed_node(AST_TYPE_NUMBERS, ILL_CHILDREN_NULL);
}

static void make_root_a_numbers_node_with_no_child(test_interpreter_ctx *ctx) {
    ctx->arg_root = ast_create_children_node_var(AST_TYPE_NUMBERS, 0);
}

static void make_root_a_ill_formed_quote_node_because_children_null(test_interpreter_ctx *ctx) {
    ctx->arg_root = ast_create_children_node_var(AST_TYPE_QUOTE, 0);
}

static ast *a_function_node_with_two_params(void) {
    return
        ast_create_children_node_var(
            AST_TYPE_FUNCTION,
            3,
            ast_create_symbol_node(DUMMY_SYMBOL_FOR_FUNCTION_NAME_P),
            ast_create_children_node_var(
                AST_TYPE_LIST_OF_PARAMETERS,
                1,
                ast_create_children_node_var(
                    AST_TYPE_PARAMETERS,
                    2,
                    ast_create_symbol_node(DUMMY_SYMBOL_P),
                    ast_create_symbol_node(DUMMY_OTHER_SYMBOL_P) ) ),
            ast_create_children_node_var(
                AST_TYPE_BLOCK,
                0
            ) );
}

static const ast *a_function_call_child_with_params_7_8(void) {
    return
        ast_create_children_node_var(
            AST_TYPE_FUNCTION_CALL,
            2,
            ast_create_symbol_node(DUMMY_SYMBOL_FOR_FUNCTION_NAME_P),
            ast_create_children_node_var(
                AST_TYPE_LIST_OF_NUMBERS,
                1,
                ast_create_children_node_var(
                    AST_TYPE_NUMBERS,
                    2,
                    ast_create_int_node(7),
                    ast_create_int_node(8)
                )
            )
        );
}

static void make_root_a_quote_node_with_a_function_call_child_with_params_7_8(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_QUOTE,
            1,
            a_function_call_child_with_params_7_8() );
}

static void make_root_a_ill_formed_reading_node_because_no_child(test_interpreter_ctx *ctx) {
    ctx->arg_root = ast_create_children_node_var(AST_TYPE_READING, 0);
}

static void make_root_a_ill_formed_reading_node_because_three_children(test_interpreter_ctx *ctx) {
    ctx->arg_root = make_ill_formed_node(AST_TYPE_READING, ILL_THREE_CHILDREN);
}

static void make_root_a_reading_node(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_READING,
            1,
            ast_create_symbol_node(DUMMY_SYMBOL_P) );
}

static void make_root_a_ill_formed_binding_node_because_no_child(test_interpreter_ctx *ctx) {
    ctx->arg_root = ast_create_children_node_var(AST_TYPE_BINDING, 0);
}

static void make_root_a_binding_node_with_rhs_number_42(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_BINDING,
            2,
            ast_create_symbol_node(DUMMY_SYMBOL_P),
            ast_create_int_node(42) );
}

static void make_root_a_ill_formed_eval_node_because_child_is_not_symbol(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_EVAL,
            1,
            ast_create_int_node(42) );
}

static void make_root_an_eval_node(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        make_unary_with_symbol(AST_TYPE_EVAL, DUMMY_SYMBOL_P);
}

static void make_root_a_ill_formed_writing_node_because_two_children(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        make_ill_formed_node(AST_TYPE_WRITING, ILL_TWO_CHILDREN);
}

static void make_root_a_writing_node_with_child_number_42(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_WRITING,
            1,
            ast_create_int_node(42) );
}

static void make_root_a_writing_node_with_child_string_abc(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_WRITING,
            1,
            ast_create_string_node("abc") );
}

static void make_root_a_writing_node_with_a_symbol(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_WRITING,
            1,
            ast_create_symbol_node(DUMMY_SYMBOL_P) );
}

static void make_root_a_writing_node_with_an_addition_1_plus_2(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_WRITING,
            1,
			ast_create_children_node_var(
				AST_TYPE_ADDITION,
				2,
				ast_create_int_node(1),
				ast_create_int_node(2) ) );
}

static void make_root_a_writing_node_with_quoted_add_1_2(test_interpreter_ctx *ctx) {
    ctx->arg_root =
        ast_create_children_node_var(
            AST_TYPE_WRITING,
            1,
			ast_create_children_node_var(
				AST_TYPE_QUOTE,
				1,
				ast_create_children_node_var(
					AST_TYPE_ADDITION,
					2,
					ast_create_int_node(1),
					ast_create_int_node(2) ) ) );
}



//-----------------------------------------------------------------------------
// RUNTIME ENVIRONMENT USAGE EXPECTATIONS
//-----------------------------------------------------------------------------


static void runtime_env_usage_expectation(test_interpreter_ctx *ctx) {
	if (!ctx->runtime_env_assert_kind) return;
	switch (ctx->runtime_env_assert_kind) {
	case ENV_ASRT_EXPECT_GET_CALLED_ON_ROOT_SYMBOL:
	    assert_ptr_equal(ctx->runtime_spy.get_arg_e, ctx->arg_env);
    	assert_ptr_equal(ctx->runtime_spy.get_arg_key, ctx->arg_root->data->data.symbol_value);
		break;
	case ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL:
	    assert_ptr_equal(ctx->runtime_spy.get_arg_e, ctx->arg_env);
    	assert_ptr_equal(ctx->runtime_spy.get_arg_key, ctx->arg_root->children->children[0]->data->data.symbol_value);
		break;
	case ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL:
	    assert_ptr_equal(ctx->runtime_spy.get_arg_e, ctx->arg_env);
    	assert_ptr_equal(ctx->runtime_spy.get_arg_key, ctx->arg_root->children->children[1]->data->data.symbol_value);
		break;
	case ENV_ASRT_EXPECT_SET_LOCAL_DEF_FCT:
    	assert_ptr_equal(
        	ctx->runtime_spy.set_local_arg_e,
        	ctx->arg_env );
    	assert_ptr_equal(
        	ctx->runtime_spy.set_local_arg_key,
        	ctx->arg_root->children->children[0]->children->children[0]->data->data.symbol_value );
    	assert_int_equal(
        	ctx->runtime_spy.set_local_arg_value->type,
        	RUNTIME_VALUE_FUNCTION );
    	if (g_runtime_spy->set_local_ret) {
        	// refcount is 1 after function node evaluation and then it is incremented by runtime_env_set_local
        	assert_int_equal(ctx->runtime_spy.set_local_arg_value_snapshot.value.refcount, 1);
        	assert_int_equal(ctx->runtime_spy.set_local_arg_value_snapshot.address->refcount, 2);
        	assert_ptr_equal(
            	ctx->runtime_spy.set_local_arg_value_snapshot.value.as.fn.function_node,
            	ctx->arg_root->children->children[0] );
        	assert_ptr_equal(
            	ctx->runtime_spy.set_local_arg_value_snapshot.value.as.fn.closure,
            	ctx->arg_env );
    	}
    	assert_int_equal(ctx->arg_env->refcount, 2);
		break;
	case ENV_ASRT_EXPECT_SET_LOCAL_BND_CHILD_TO_READ:
	    assert_ptr_equal(
    	    ctx->runtime_spy.set_local_arg_e,
        	ctx->arg_env );
    	assert_ptr_equal(
        	ctx->runtime_spy.set_local_arg_key,
        	ctx->arg_root->children->children[0]->data->data.symbol_value );
    	if (g_runtime_spy->set_local_ret) {
        	switch (ctx->read_ast_arrange_kind) {
        	case READ_AST_ARR_READ_NUM_42:
            	assert_int_equal(
                	ctx->runtime_spy.set_local_arg_value_snapshot.value.type,
                	RUNTIME_VALUE_NUMBER );
	            assert_int_equal(
    	            ctx->runtime_spy.set_local_arg_value_snapshot.value.refcount,
        	        1 );
            	assert_int_equal(
                	ctx->runtime_spy.set_local_arg_value_snapshot.value.as.i,
                	42 );
			break;
        	default:
            	assert_true(false);
        	}
    	} else {
        	assert_int_equal(
            	ctx->runtime_spy.set_local_arg_value_snapshot.value.refcount,
            	1 );
		}
        break;
	case ENV_ASRT_EXPECT_SET_LOCAL_BND_LHS_TO_NUM_42:
    	assert_ptr_equal(
        	ctx->runtime_spy.set_local_arg_e,
        	ctx->arg_env );
    	assert_ptr_equal(
        	ctx->runtime_spy.set_local_arg_key,
        	ctx->arg_root->children->children[0]->data->data.symbol_value );
    	if (g_runtime_spy->set_local_ret) {
        	assert_int_equal(
            	ctx->runtime_spy.set_local_arg_value_snapshot.value.refcount,
            	1 );
        	assert_int_equal(
            	ctx->runtime_spy.set_local_arg_value_snapshot.address->refcount,
            	2 );
        	assert_int_equal(
            	ctx->runtime_spy.set_local_arg_value_snapshot.value.as.i,
            	42 );
    	}
	    break;
    default:
        assert_true(false);
    }
}



//-----------------------------------------------------------------------------
// READ_AST CALLBACK USAGE EXPECTATIONS
//-----------------------------------------------------------------------------


static void read_ast_usage_expectation(test_interpreter_ctx *ctx) {
	if (!ctx->read_ast_assert_kind) return;
	switch (ctx->read_ast_assert_kind) {
	case READ_AST_ASRT_NONE:
		// do nothing
		break;
	case READ_AST_ASRT_CALL:
	    assert_ptr_equal(
    	    ctx->read_ast_spy.read_ast_fn_arg_ctx,
        	ctx->arg_ctx );
		break;
	default:
		assert_true(false);
	}
}



//-----------------------------------------------------------------------------
// WRITE_RUNTIME_VALUE CALLBACK USAGE EXPECTATIONS
//-----------------------------------------------------------------------------


static void write_runtime_value_usage_expectation(test_interpreter_ctx *ctx) {
	if (!ctx->write_runtime_value_assert_kind) return;
	assert_ptr_equal(
		ctx->spy_write_runtime_value.write_runtime_value_fn_arg_ctx,
		ctx->arg_ctx
	);
	test_interpreter_runtime_value_snapshot snapshot =
		ctx->spy_write_runtime_value.write_runtime_value_fn_arg_value_snapshot;
	switch (ctx->write_runtime_value_assert_kind) {
	case WRITE_RUNTIME_VALUE_ASRT_NONE:
		// do nothing
		break;
	case WRITE_RUNTIME_VALUE_ASRT_WRITE_NUM_42:
		assert_int_equal(
			snapshot.value.type,
			RUNTIME_VALUE_NUMBER
		);
		assert_int_equal(
			snapshot.value.refcount,
			1
		);
		assert_int_equal(
			snapshot.value.as.i,
			42
		);
		break;
	case WRITE_RUNTIME_VALUE_ASRT_WRITE_NUM_3:
		assert_int_equal(
			snapshot.value.type,
			RUNTIME_VALUE_NUMBER
		);
		assert_int_equal(
			snapshot.value.refcount,
			1
		);
		assert_int_equal(
			snapshot.value.as.i,
			3
		);
		break;
	case WRITE_RUNTIME_VALUE_ASRT_WRITE_NUM_42_RCNT_2:
		assert_int_equal(
			snapshot.value.type,
			RUNTIME_VALUE_NUMBER
		);
		assert_int_equal(
			snapshot.value.refcount,
			2
		);
		assert_int_equal(
			snapshot.address->refcount,
			1 );
		assert_int_equal(
			snapshot.value.as.i,
			42
		);
		break;
	case WRITE_RUNTIME_VALUE_ASRT_WRITE_STR_ABC:
		assert_int_equal(
			snapshot.value.type,
			RUNTIME_VALUE_STRING
		);
		assert_int_equal(
			snapshot.value.refcount,
			1
		);
		assert_string_equal(
			snapshot.value.as.s,
			"abc"
		);
		break;
	case WRITE_RUNTIME_VALUE_ASRT_WRITE_QUOTED_ADD_1_PLUS_2:
		assert_int_equal(
			snapshot.value.type,
			RUNTIME_VALUE_QUOTED
		);
		assert_int_equal(
			snapshot.value.refcount,
			1
		);
		assert_ptr_equal(
			snapshot.value.as.quoted,
			ctx->arg_root->children->children[0]->children->children[0]
		);
		break;
	default:
		assert_true(false);
	}
}



//-----------------------------------------------------------------------------
// SPY INITIALIZATION FOR runtime_env INTERACTION TESTING
//-----------------------------------------------------------------------------


static void spy_runtime_env_arrange(const test_interpreter_case *param_case) {
	param_case->ctx->runtime_env_arrange_kind = param_case->runtime_env_arrange_kind;
	param_case->ctx->runtime_env_assert_kind = param_case->runtime_env_assert_kind;
	switch (param_case->ctx->runtime_env_arrange_kind) {
	case ENV_ARR_NONE:
		// do nothing
		break;
	case ENV_ARR_GET_RET_NULL:
		param_case->ctx->runtime_spy.get_ret = NULL;
		break;
	case ENV_ARR_GET_RET_NUM_42:
		param_case->ctx->runtime_spy.get_ret = runtime_env_make_number(42);
		break;
	case ENV_ARR_GET_RET_NUM_42_RCNT_2:
		runtime_env_value *value = runtime_env_make_number(42);
		runtime_env_value_retain(value);
		assert_int_equal(value->refcount, 2);
		param_case->ctx->runtime_spy.get_ret = value;
		break;
	case ENV_ARR_GET_RET_NUM_0:
		param_case->ctx->runtime_spy.get_ret = runtime_env_make_number(0);
		break;
	case ENV_ARR_GET_RET_QUOTED_ADD_1_PLUS_2:
		param_case->ctx->runtime_spy.get_ret =
			runtime_env_make_quoted(
				ast_create_children_node_var(
					AST_TYPE_ADDITION,
					2,
					ast_create_int_node(1),
					ast_create_int_node(2) ) );
		break;
	case ENV_ARR_GET_RET_STR_ABC:
		param_case->ctx->runtime_spy.get_ret = runtime_env_make_string("abc");
		break;
	case ENV_ARR_SET_LOCAL_RET_FALSE:
		param_case->ctx->runtime_spy.set_local_ret = false;
		break;
	case ENV_ARR_SET_LOCAL_RET_TRUE:
		param_case->ctx->runtime_spy.set_local_ret = true;
		break;
	default:
		assert_true(false);
	}
}



//-----------------------------------------------------------------------------
// SPY INITIALIZATION FOR read_ast INTERACTION TESTING
//-----------------------------------------------------------------------------


static void spy_read_ast_arrange(const test_interpreter_case *param_case) {
	param_case->ctx->read_ast_arrange_kind = param_case->read_ast_arrange_kind;
	param_case->ctx->read_ast_assert_kind = param_case->read_ast_assert_kind;
	switch (param_case->ctx->read_ast_arrange_kind) {
		case READ_AST_ARR_NONE:
		// do nothing
		break;
	case READ_AST_ARR_READ_FAIL:
		param_case->ctx->read_ast_spy.read_ast_ret = NULL;
		break;
	case READ_AST_ARR_READ_NUM_42:
		param_case->ctx->read_ast_spy.read_ast_ret = ast_create_int_node(42);
		break;
	default:
		assert_true(false);
	}
}



//-----------------------------------------------------------------------------
// SPY INITIALIZATION FOR write_runtime_value INTERACTION TESTING
//-----------------------------------------------------------------------------


static void spy_write_runtime_value_arrange(const test_interpreter_case *param_case) {
	param_case->ctx->write_runtime_value_arrange_kind = param_case->write_runtime_value_arrange_kind;
	param_case->ctx->write_runtime_value_assert_kind = param_case->write_runtime_value_assert_kind;
	switch (param_case->ctx->write_runtime_value_arrange_kind) {
	case WRITE_RUNTIME_VALUE_ARR_NONE:
		// do nothing
		break;
	case WRITE_RUNTIME_VALUE_ARR_WRITE_FAIL:
		param_case->ctx->spy_write_runtime_value.write_runtime_value_ret = false;
		break;
	case WRITE_RUNTIME_VALUE_ARR_WRITE_SUCCESS:
		param_case->ctx->spy_write_runtime_value.write_runtime_value_ret = true;
		break;
	default:
		assert_true(false);
	}
}



//-----------------------------------------------------------------------------
// OUT EXPECTATIONS
//-----------------------------------------------------------------------------


static void expected_out_unchanged(test_interpreter_ctx *ctx) {
    assert_ptr_equal(ctx->arg_out, ctx->arg_out_before);
    assert_ptr_equal(*ctx->arg_out, ctx->arg_out_value_before);
}

static void expected_out_points_on_fresh_string_abc(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_STRING);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_string_equal((*ctx->arg_out)->as.s, ctx->arg_root->data->data.string_value);
}

static void expected_out_points_on_get_ret_and_its_refcount_has_been_incremented_to_3(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_ptr_equal(*ctx->arg_out, ctx->runtime_spy.get_ret_snapshot.address);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal((*ctx->arg_out)->refcount, 3);
	assert_int_equal((*ctx->arg_out)->as.i, 42);
}

static void expected_out_points_on_fresh_error_not_sentinel_value(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_ERROR);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_int_equal((*ctx->arg_out)->as.err.code, ctx->arg_root->error->code);
    assert_string_equal((*ctx->arg_out)->as.err.msg, ctx->arg_root->error->message);
}

static void expected_out_points_on_fresh_error_sentinel_value(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_ERROR);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_int_equal((*ctx->arg_out)->as.err.code, ast_error_sentinel()->error->code);
    assert_string_equal((*ctx->arg_out)->as.err.msg, ast_error_sentinel()->error->message);
}

static void expected_out_points_on_fresh_function_value(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_FUNCTION);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_ptr_equal((*ctx->arg_out)->as.fn.function_node, ctx->arg_root);
    assert_ptr_equal((*ctx->arg_out)->as.fn.closure, ctx->arg_env);
}

static void expected_out_points_on_child_function_value_with_refcount_2(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_FUNCTION);
    assert_int_equal((*ctx->arg_out)->refcount, 2);
    assert_ptr_equal((*ctx->arg_out)->as.fn.function_node, ctx->arg_root->children->children[0]);
    assert_ptr_equal((*ctx->arg_out)->as.fn.closure, ctx->arg_env);
}

static void expected_out_points_on_negation_result_when_number(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_int_equal((*ctx->arg_out)->as.i, - ctx->arg_root->children->children[0]->data->data.int_value);
}

static void expected_out_points_on_fresh_number_minus_42(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
	assert_int_equal((*ctx->arg_out)->as.i, -42);
}

static void expected_out_points_on_addition_result_when_two_numbers(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_int_equal(
        (*ctx->arg_out)->as.i,
        ctx->arg_root->children->children[0]->data->data.int_value
        +
        ctx->arg_root->children->children[1]->data->data.int_value );
}

static void expected_out_points_on_addition_result_when_lhs_number_rhs_symbol(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_int_equal(
        (*ctx->arg_out)->as.i,
        ctx->arg_root->children->children[0]->data->data.int_value
        +
        ctx->runtime_spy.get_ret_snapshot.value.as.i );
}

static void expected_out_points_on_subtraction_result_when_two_numbers(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_int_equal(
        (*ctx->arg_out)->as.i,
        ctx->arg_root->children->children[0]->data->data.int_value
        -
        ctx->arg_root->children->children[1]->data->data.int_value );
}

static void expected_out_points_on_subtraction_result_when_lhs_number_rhs_symbol(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_int_equal(
        (*ctx->arg_out)->as.i,
        ctx->arg_root->children->children[0]->data->data.int_value
        -
        ctx->runtime_spy.get_ret_snapshot.value.as.i );
}

static void expected_out_points_on_subtraction_result(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal(
        (*ctx->arg_out)->as.i,
        ctx->arg_root->children->children[0]->data->data.int_value
        -
        ctx->arg_root->children->children[1]->data->data.int_value );
}

static void expected_out_points_on_multiplication_result(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal(
        (*ctx->arg_out)->as.i,
        ctx->arg_root->children->children[0]->data->data.int_value
        *
        ctx->arg_root->children->children[1]->data->data.int_value );
}

static void expected_out_points_on_multiplication_result_when_two_numbers(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_int_equal(
        (*ctx->arg_out)->as.i,
        ctx->arg_root->children->children[0]->data->data.int_value
        *
        ctx->arg_root->children->children[1]->data->data.int_value );
}

static void expected_out_points_on_multiplication_result_when_lhs_number_rhs_symbol(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_int_equal(
        (*ctx->arg_out)->as.i,
        ctx->arg_root->children->children[0]->data->data.int_value
        *
        ctx->runtime_spy.get_ret_snapshot.value.as.i );
}

static void expected_out_points_on_division_result_when_two_numbers(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal(
        (*ctx->arg_out)->as.i,
        ctx->arg_root->children->children[0]->data->data.int_value
        /
        ctx->arg_root->children->children[1]->data->data.int_value );
}

static void expected_out_points_on_division_result_when_lhs_number_rhs_symbol(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_int_equal(
        (*ctx->arg_out)->as.i,
        ctx->arg_root->children->children[0]->data->data.int_value
        /
        ctx->runtime_spy.get_ret_snapshot.value.as.i );
}

static void expected_out_points_on_quoted_value(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_QUOTED);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_ptr_equal((*ctx->arg_out)->as.quoted, ctx->arg_root->children->children[0]);
}

static void expected_out_points_on_bound_number_42(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal((*ctx->arg_out)->refcount, 2);
    assert_int_equal((*ctx->arg_out)->as.i, 42);
}

static void expected_out_points_on_bound_string_abc(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_STRING);
    assert_int_equal((*ctx->arg_out)->refcount, 2);
    assert_string_equal((*ctx->arg_out)->as.s, "abc");
}

static void expected_out_points_on_fresh_number_3(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_int_equal((*ctx->arg_out)->as.i, 3);
}

static void expected_out_points_on_fresh_number_42(test_interpreter_ctx *ctx) {
    assert_non_null(*ctx->arg_out);
    assert_int_equal((*ctx->arg_out)->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal((*ctx->arg_out)->refcount, 1);
    assert_int_equal((*ctx->arg_out)->as.i, 42);
}



//-----------------------------------------------------------------------------
// TEST INFRASTUCTURE CLEANUP
//-----------------------------------------------------------------------------


static void destroy_root(test_interpreter_ctx *ctx) {
    ast_destroy((ast *) ctx->arg_root);
    ctx->arg_root = NULL;
}

static void destroy_runtime_value(const runtime_env_value *value) {
    size_t refcount = value->refcount;
    for (size_t i = 0; i < refcount; i++) runtime_env_value_release(value);
}

static void destroy_root_and_runtime_value_at_out(test_interpreter_ctx *ctx) {
    destroy_root(ctx);
    destroy_runtime_value(*ctx->arg_out);
}

static void destroy_root_and_read_ast(test_interpreter_ctx *ctx) {
    destroy_root(ctx);
    ast_destroy((ast *) ctx->read_ast_spy.read_ast_ret);
}

static void destroy_root_and_read_ast_and_runtime_value_at_out(test_interpreter_ctx *ctx) {
    destroy_root(ctx);
    ast_destroy((ast *) ctx->read_ast_spy.read_ast_ret);
    destroy_runtime_value(*ctx->arg_out);
}

static void destroy_root_and_quoted_ast_in_get_ret_and_get_ret_and_runtime_value_at_out(test_interpreter_ctx *ctx) {
    destroy_root(ctx);
    ast_destroy((ast *) ctx->runtime_spy.get_ret->as.quoted);
    destroy_runtime_value(ctx->runtime_spy.get_ret);
    destroy_runtime_value(*ctx->arg_out);
}

static void destroy_root_and_quoted_ast_in_get_ret_and_get_ret(test_interpreter_ctx *ctx) {
    destroy_root(ctx);
    ast_destroy((ast *) ctx->runtime_spy.get_ret->as.quoted);
    destroy_runtime_value(ctx->runtime_spy.get_ret);
}

static void destroy_root_and_written_runtime_value(test_interpreter_ctx *ctx) {
    destroy_root(ctx);
	destroy_runtime_value(ctx->spy_write_runtime_value.write_runtime_value_fn_arg_value);
}

static void destroy_root_and_get_ret(test_interpreter_ctx *ctx) {
    destroy_root(ctx);
	destroy_runtime_value(ctx->runtime_spy.get_ret);
}

static void destroy_root_and_get_ret_and_runtime_value_at_out(test_interpreter_ctx *ctx) {
    destroy_root(ctx);
	destroy_runtime_value(ctx->runtime_spy.get_ret);
	destroy_runtime_value(*ctx->arg_out);
}



//-----------------------------------------------------------------------------
// EVALUATION OF AST OF TYPES AST_TYPE_DATA_WRAPPER AND AST_TYPE_ERROR
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
  - interpreter_status interpreter_eval(
        struct interpreter_ctx *ctx,
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
  - spy:
    - runtime_env_set_local
    - runtime_env_get_local
    - runtime_env_get
  - fake:
    - functions of standard library which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// PARAMETRIC CASES
//-----------------------------------------------------------------------------


// Given:
//  - args are valid
//  - root is a number node
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_INT_OOM = {0};
static const test_interpreter_case INT_NODE_OOM = {
    .name = "eval_error_oom_when_int_node_and_malloc_fails",

    .root_constructor_fn = &make_root_a_int_node,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_INT_OOM,
};

// Given:
//  - args are valid
//  - root is a number node registering integer value 42
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_NUMBER
//    && (*out)->refcount == 1
//    && (*out)->as.i == 42
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_INT_NODE_SUCCESS = {0};
static const test_interpreter_case INT_NODE_SUCCESS = {
    .name = "eval_success_when_int_node_and_malloc_succeeds",

    .root_constructor_fn = &make_root_a_int_node,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_points_on_fresh_number_42,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_INT_NODE_SUCCESS,
};

// Given:
//  - args are valid
//  - root is a string node registering the value of the constant string A_CONSTANT_STRING
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_STRING_OOM = {0};
static const test_interpreter_case STRING_NODE_OOM = {
    .name = "eval_error_oom_when_string_node_and_malloc_fails",

    .root_constructor_fn = &make_root_a_string_node_abc,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_STRING_OOM,
};

// Given:
//  - args are valid
//  - root is a string node registering the value "abc"
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_STRING
//    && (*out)->refcount == 1
//    && string_equal((*out)->as.s, "abc")
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_STRING_NODE_SUCCESS = {0};
static const test_interpreter_case STRING_NODE_SUCCESS = {
    .name = "eval_success_when_string_node_and_malloc_succeeds",

    .root_constructor_fn = &make_root_a_string_node_abc,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_points_on_fresh_string_abc,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_STRING_NODE_SUCCESS,
};

// Given:
//  - args are valid
//  - root is a symbol node registering DUMMY_SYMBOL_P
//  - DUMMY_SYMBOL_P is unbound in environment
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_LOOKUP_FAILED
static test_interpreter_ctx CTX_SYMBOL_UNBOUND = {0};
static const test_interpreter_case SYMBOL_NODE_UNBOUND = {
    .name = "eval_error_when_symbol_node_unbound",

    .root_constructor_fn = &make_root_a_symbol_node,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NULL,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_ROOT_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_LOOKUP_FAILED,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_SYMBOL_UNBOUND,
};

// Given:
//  - args are valid
//  - root is a symbol node registering DUMMY_SYMBOL_P
//  - DUMMY_SYMBOL_P is bound to a RUNTIME_VALUE_NUMBER 42 with
//    refcount 2 in environment
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - calls runtime_env_get with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//  - environment is not modified
//  - *out != NULL
//    && *out == param_case->ctx->runtime_spy.get_ret_snapshot.address
//    && *out->refcount == 3
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_SYMBOL_BOUND = {0};
static const test_interpreter_case SYMBOL_NODE_BOUND = {
    .name = "eval_success_when_symbol_node_bound",

    .root_constructor_fn = &make_root_a_symbol_node,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NUM_42_RCNT_2,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_ROOT_SYMBOL,
    .expected_out_fn = &expected_out_points_on_get_ret_and_its_refcount_has_been_incremented_to_3,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_SYMBOL_BOUND,
};

// Given:
//  - args are valid
//  - root is an error node distinct from the sentinel
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_ERROR_NOT_SENTINEL_OOM = {0};
static const test_interpreter_case ERROR_NOT_SENTINEL_NODE_OOM = {
    .name = "eval_error_oom_when_error_node_not_sentinel_and_malloc_fails",

    .root_constructor_fn = &make_root_an_error_not_sentinel_node,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_ERROR_NOT_SENTINEL_OOM,
};

// Given:
//  - args are valid
//  - root is an error node distinct from the sentinel
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  -     *out != NULL
//    && (*out)->type == RUNTIME_VALUE_ERROR
//    && (*out)->refcount == 1
//    && (*out)->as.err.code == root->error->code
//    && string_equal((*out)->as.err.msg, root->error->message)
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_ERROR_NOT_SENTINEL_NODE_SUCCESS = {0};
static const test_interpreter_case ERROR_NOT_SENTINEL_NODE_SUCCESS = {
    .name = "eval_success_when_error_node_not_sentinel_and_malloc_succeeds",

    .root_constructor_fn = &make_root_an_error_not_sentinel_node,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_points_on_fresh_error_not_sentinel_value,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_ERROR_NOT_SENTINEL_NODE_SUCCESS,
};

// Given:
//  - args are valid
//  - root is the error node sentinel
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_ERROR_SENTINEL_OOM = {0};
static const test_interpreter_case ERROR_SENTINEL_NODE_OOM = {
    .name = "eval_error_oom_when_error_node_sentinel_and_malloc_fails",

    .root_constructor_fn = &make_root_the_error_sentinel_node,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_ERROR_SENTINEL_OOM,
};

// Given:
//  - args are valid
//  - root is the error node sentinel
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_ERROR
//    && (*out)->refcount == 1
//    && (*out)->as.err.code == root->error->code
//    && string_equal((*out)->as.err.msg, root->error->message)
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_ERROR_SENTINEL_NODE_SUCCESS = {0};
static const test_interpreter_case ERROR_SENTINEL_NODE_SUCCESS = {
    .name = "eval_success_when_error_node_sentinel_and_malloc_succeeds",

    .root_constructor_fn = &make_root_the_error_sentinel_node,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_points_on_fresh_error_sentinel_value,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_ERROR_SENTINEL_NODE_SUCCESS,
};



//-----------------------------------------------------------------------------
// EVALUATION OF AST OF TYPE AST_TYPE_FUNCTION
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
  - interpreter_status interpreter_eval(
        struct interpreter_ctx *ctx,
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
    - ast_create_int_node
    - ast_create_children_node_var
    - ast_destroy

doubles:
  - dummy:
    - argument "symbol *sym" of ast_create_symbol_node
  - spy:
    - runtime_env_set_local
    - runtime_env_get_local
    - runtime_env_get
  - fake:
    - functions of standard library which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// PARAMETRIC CASES
//-----------------------------------------------------------------------------


// Given:
//  - env and out are valid
//  - root is a ill-formed function node because:
//    - root->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_FUNCTION_NODE_ILL_FORMED_CHILDREN_NULL = {0};
static const test_interpreter_case FUNCTION_NODE_ILL_FORMED_CHILDREN_NULL = {
    .name = "eval_error_invalid_ast_when_function_node_ill_formed_cause_children_null",

    .root_constructor_fn = &make_root_a_ill_formed_function_node_because_children_null,
    .env_is_dummy = false,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_FUNCTION_NODE_ILL_FORMED_CHILDREN_NULL,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed function node because:
//  - root->children != NULL && root->children->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_FUNCTION_NODE_ILL_FORMED_NO_CHILD = {0};
static const test_interpreter_case FUNCTION_NODE_ILL_FORMED_NO_CHILD = {
    .name = "eval_error_invalid_ast_when_function_node_ill_formed_cause_no_child",

    .root_constructor_fn = &make_root_a_ill_formed_function_node_because_no_child,
    .env_is_dummy = false,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_FUNCTION_NODE_ILL_FORMED_NO_CHILD,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed function node because:
//    - root->children != NULL && root->children->children_nb == 2
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_FUNCTION_NODE_ILL_FORMED_TWO_CHILDREN = {0};
static const test_interpreter_case FUNCTION_NODE_ILL_FORMED_TWO_CHILDREN = {
    .name = "eval_error_invalid_ast_when_function_node_ill_formed_cause_two_children",

    .root_constructor_fn = &make_root_a_ill_formed_function_node_because_two_children,
    .env_is_dummy = false,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_FUNCTION_NODE_ILL_FORMED_TWO_CHILDREN,
};

// Given:
//  - env and out are valid
//  - root is a well-formed function node
//  - but two parameters point on the same symbol
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_DUPLICATE_PARAMETER
static test_interpreter_ctx CTX_FUNCTION_NODE_DUPLICATE_PARAM = {0};
static const test_interpreter_case FUNCTION_NODE_DUPLICATE_PARAM = {
    .name = "eval_error_duplicate_param_when_function_node_well_formed_but_duplicate_param",

    .root_constructor_fn = &make_root_a_function_node_with_duplicate_param,
    .env_is_dummy = false,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_DUPLICATE_PARAMETER,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_FUNCTION_NODE_DUPLICATE_PARAM,
};

// Given:
//  - args are valid
//  - root is an empty function node (so there is no duplicate parameter)
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_FUNCTION_NODE_OOM = {0};
static const test_interpreter_case FUNCTION_NODE_OOM = {
    .name = "eval_error_oom_when_function_node_and_malloc_fails",

    .root_constructor_fn = &make_root_an_empty_function_node_with_dummy_name,
    .env_is_dummy = false,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_FUNCTION_NODE_OOM,
};

// Given:
//  - args are valid
//  - root is an empty function node (so there is no duplicate parameter)
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  - environment is not modified
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_FUNCTION
//    && (*out)->refcount == 1
//    && (*out)->as.fn.function_node == root
//    && (*out)->as.fn.closure == env
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_FUNCTION_NODE_SUCCESS = {0};
static const test_interpreter_case FUNCTION_NODE_SUCCESS = {
    .name = "eval_success_when_function_node_and_malloc_succeeds",

    .root_constructor_fn = &make_root_an_empty_function_node_with_dummy_name,
    .env_is_dummy = false,
    .oom = false,

    .expected_out_fn = &expected_out_points_on_fresh_function_value,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_FUNCTION_NODE_SUCCESS,
};



//-----------------------------------------------------------------------------
// EVALUATION OF AST OF TYPE AST_TYPE_FUNCTION_DEFINITION
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
    interpreter_status interpreter_eval(
        struct interpreter_ctx *ctx,
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
    - ast_create_int_node
    - ast_create_children_node_var
    - ast_destroy

doubles:
  - dummy:
    - argument "symbol *sym" of ast_create_symbol_node
  - spy:
    - runtime_env_set_local
    - runtime_env_get_local
    - runtime_env_get
  - fake:
    - functions of standard library which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// PARAMETRIC CASES
//-----------------------------------------------------------------------------


// Given:
//  - env and out are valid
//  - root is a ill-formed function definition node because:
//  - root->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_FUNCTION_DEFINITION_NODE_ILL_FORMED_CHILDREN_NULL = {0};
static const test_interpreter_case FUNCTION_DEFINITION_NODE_ILL_FORMED_CHILDREN_NULL = {
    .name = "eval_error_invalid_ast_when_function_definition_node_ill_formed_cause_children_null",

    .root_constructor_fn = &make_root_a_ill_formed_function_definition_node_because_children_null,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_FUNCTION_DEFINITION_NODE_ILL_FORMED_CHILDREN_NULL,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed function definition node because:
//  - root->children != NULL && root->children->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_FUNCTION_DEFINITION_NODE_ILL_FORMED_NO_CHILD = {0};
static const test_interpreter_case FUNCTION_DEFINITION_NODE_ILL_FORMED_NO_CHILD = {
    .name = "eval_error_invalid_ast_when_function_definition_node_ill_formed_cause_no_child",

    .root_constructor_fn = &make_root_a_ill_formed_function_definition_node_because_no_child,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_FUNCTION_DEFINITION_NODE_ILL_FORMED_NO_CHILD,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed function definition node because:
//    - root->children != NULL && root->children->children_nb == 2
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_FUNCTION_DEFINITION_NODE_ILL_FORMED_TWO_CHILDREN = {0};
static const test_interpreter_case FUNCTION_DEFINITION_NODE_ILL_FORMED_TWO_CHILDREN = {
    .name = "eval_error_invalid_ast_when_function_definition_node_ill_formed_cause_two_children",

    .root_constructor_fn = &make_root_a_ill_formed_function_definition_node_because_two_children,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_FUNCTION_DEFINITION_NODE_ILL_FORMED_TWO_CHILDREN,
};

// Given:
//  - args are valid
//  - root is a function definition node and its child is an empty function node
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_FUNCTION_DEFINITION_NODE_OOM = {0};
static const test_interpreter_case FUNCTION_DEFINITION_NODE_OOM = {
    .name = "eval_error_oom_when_function_definition_node_and_malloc_fails",

    .root_constructor_fn = &make_root_a_function_definition_node_with_empty_function,
    .env_is_dummy = false,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_FUNCTION_DEFINITION_NODE_OOM,
};

// Given:
//  - args are valid
//  - root is a function definition node and its child is an empty function node
//  - all allocations will succeed during interpreter_eval call
//  - symbol binding will fail
// Expected:
//  - build v, a runtime_env_value * such as
//    - v != NULL
//    - v->type == RUNTIME_VALUE_FUNCTION
//    - v->refcount == 1
//    - v->as.fn.function_node == root->children->children[0]
//    - v->as.fn.closure == env
//  - calls runtime_env_set_local with:
//    - e: env
//    - key: root->children->children[0]->children->children[0]->data->data.symbol_value
//    - value: v
//  - destroys v
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_BINDING_ERROR
static test_interpreter_ctx CTX_FUNCTION_DEFINITION_NODE_BINDING_FAILURE = {0};
static const test_interpreter_case FUNCTION_DEFINITION_NODE_BINDING_FAILURE = {
    .name = "eval_binding_error_when_function_definition_node_and_binding_fails",

    .root_constructor_fn = &make_root_a_function_definition_node_with_empty_function,
    .env_is_dummy = false,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_SET_LOCAL_RET_FALSE,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_SET_LOCAL_DEF_FCT,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_BINDING_ERROR,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_FUNCTION_DEFINITION_NODE_BINDING_FAILURE,
};

// Given:
//  - args are valid
//  - root is a function definition node and its child is an empty function node
//  - all allocations will succeed during interpreter_eval call
//  - symbol binding will succeed
// Expected:
//  - build v, a runtime_env_value * such as
//    - v != NULL
//    - v->type == RUNTIME_VALUE_FUNCTION
//    - v->refcount == 1 (not checked by this test but it is in function node evaluation tests)
//    - v->as.fn.function_node == root->children->children[0]
//    - v->as.fn.closure == env
//  - calls runtime_env_set_local with:
//    - e: env
//    - key: root->children->children[0]->children->children[0]->data->data.symbol_value
//    - value: v
//  - the symbol interned by resolver while function symbol name promotion is bound to v
//  - *out == v
//    - (*out)->type == RUNTIME_VALUE_FUNCTION
//    - (*out)->refcount == 2 (THE OUT RUNTIME VALUE IS OWNED!)
//    - (*out)->as.fn.function_node == root->children->children[0]
//    - (*out)->as.fn.closure == env
//    - (*out)->as.fn.closure->refcount == 2 (this is env->refcount)
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_FUNCTION_DEFINITION_NODE_SUCCESS = {0};
static const test_interpreter_case FUNCTION_DEFINITION_NODE_SUCCESS = {
    .name = "eval_success_when_function_definition_node_and_binding_succeeds",

    .root_constructor_fn = &make_root_a_function_definition_node_with_empty_function,
    .env_is_dummy = false,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_SET_LOCAL_RET_TRUE,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_SET_LOCAL_DEF_FCT,
    .expected_out_fn = &expected_out_points_on_child_function_value_with_refcount_2,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_FUNCTION_DEFINITION_NODE_SUCCESS,
};



//-----------------------------------------------------------------------------
// EVALUATION OF AST OF TYPE TYPE AST_TYPE_NEGATION
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
    interpreter_status interpreter_eval(
        struct interpreter_ctx *ctx,
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
    - runtime_env_set_local
    - runtime_env_get_local
    - runtime_env_get
  - fake:
    - functions of standard library which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// PARAMETRIC CASES
//-----------------------------------------------------------------------------


// Given:
//  - env and out are valid
//  - root is a ill-formed negation node because:
//    - root->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_NEGATION_NODE_ILL_FORMED_CHILDREN_NULL = {0};
static const test_interpreter_case NEGATION_NODE_ILL_FORMED_CHILDREN_NULL = {
    .name = "eval_error_invalid_ast_when_negation_node_ill_formed_cause_children_null",

    .root_constructor_fn = &make_root_a_ill_formed_negation_node_because_children_null,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_NEGATION_NODE_ILL_FORMED_CHILDREN_NULL,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed negation node because:
//  - root->children != NULL && root->children->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_NEGATION_NODE_ILL_FORMED_NO_CHILD = {0};
static const test_interpreter_case NEGATION_NODE_ILL_FORMED_NO_CHILD = {
    .name = "eval_error_invalid_ast_when_negation_node_ill_formed_cause_no_child",

    .root_constructor_fn = &make_root_a_ill_formed_negation_node_because_no_child,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_NEGATION_NODE_ILL_FORMED_NO_CHILD,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed negation node because:
//    - root->children != NULL && root->children->children_nb == 2
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_NEGATION_NODE_ILL_FORMED_TWO_CHILDREN = {0};
static const test_interpreter_case NEGATION_NODE_ILL_FORMED_TWO_CHILDREN = {
    .name = "eval_error_invalid_ast_when_negation_node_ill_formed_cause_two_children",

    .root_constructor_fn = &make_root_a_ill_formed_negation_node_because_two_children,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_NEGATION_NODE_ILL_FORMED_TWO_CHILDREN,
};

// Given:
//  - args are well-formed
//  - root is a negation node with one child
//  - root->children->children[0]->data->type == TYPE_SYMBOL
//  - in runtime environment:
//    root->children->children[0]->data->data.symbol_value
//    is not bound to any runtime_env_value*
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_LOOKUP_FAILED
static test_interpreter_ctx CTX_NEGATION_NODE_WITH_SYMBOL_AND_UNBOUND = {0};
static const test_interpreter_case NEGATION_NODE_WITH_SYMBOL_AND_UNBOUND = {
    .name = "eval_error_unbound_when_negation_node_with_symbol_and_unbound",

    .root_constructor_fn = &make_root_a_negation_node_with_a_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NULL,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_LOOKUP_FAILED,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_NEGATION_NODE_WITH_SYMBOL_AND_UNBOUND,
};

// Given:
//  - args are well-formed
//  - root is a negation node with one child
//  - root->children->children[0]->data->type == TYPE_SYMBOL
//  - in runtime environment:
//    root->children->children[0]->data->data.symbol_value
//    is not bound to any runtime_env_value*
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_LOOKUP_FAILED
static test_interpreter_ctx CTX_NEGATION_NODE_WITH_SYMBOL_AND_UNBOUND_AND_OOM = {0};
static const test_interpreter_case NEGATION_NODE_WITH_SYMBOL_AND_UNBOUND_AND_OOM = {
    .name = "eval_error_unbound_when_negation_node_with_symbol_and_unbound_and_oom",

    .root_constructor_fn = &make_root_a_negation_node_with_a_symbol,
    .env_is_dummy = true,
    .oom = true,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NULL,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_LOOKUP_FAILED,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_NEGATION_NODE_WITH_SYMBOL_AND_UNBOUND_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a negation node with one child
//  - root->children->children[0]->data->type == TYPE_SYMBOL
//  - in runtime environment:
//    root->children->children[0]->data->data.symbol_value
//    is bound to
//    a runtime value which is not of type RUNTIME_VALUE_NUMBER
//    (it is bound to a RUNTIME_VALUE_STRING "abc"
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_TYPE_ERROR
static test_interpreter_ctx CTX_NEGATION_NODE_WITH_SYMBOL_AND_BOUND_NOT_TO_A_NUMBER = {0};
static const test_interpreter_case NEGATION_NODE_WITH_SYMBOL_AND_BOUND_NOT_TO_A_NUMBER = {
    .name = "eval_type_error_when_negation_node_with_symbol_and_bound_not_to_a_number",

    .root_constructor_fn = &make_root_a_negation_node_with_a_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_STR_ABC,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_TYPE_ERROR,

    .clean_up_fn = &destroy_root_and_get_ret,

    .ctx =&CTX_NEGATION_NODE_WITH_SYMBOL_AND_BOUND_NOT_TO_A_NUMBER,
};

// Given:
//  - args are well-formed
//  - root is a negation node with one child
//  - root->children->children[0]->data->type == TYPE_SYMBOL
//  - in runtime environment:
//    root->children->children[0]->data->data.symbol_value
//    is bound to
//    a runtime value which is not of type RUNTIME_VALUE_NUMBER
//    (it is bound to a RUNTIME_VALUE_STRING "abc"
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_TYPE_ERROR
static test_interpreter_ctx CTX_NEGATION_NODE_WITH_SYMBOL_AND_BOUND_NOT_TO_A_NUMBER_AND_OOM = {0};
static const test_interpreter_case NEGATION_NODE_WITH_SYMBOL_AND_BOUND_NOT_TO_A_NUMBER_AND_OOM = {
    .name = "eval_type_error_when_negation_node_with_symbol_and_bound_not_to_a_number_and_oom",

    .root_constructor_fn = &make_root_a_negation_node_with_a_symbol,
    .env_is_dummy = true,
    .oom = true,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_STR_ABC,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_TYPE_ERROR,

    .clean_up_fn = &destroy_root_and_get_ret,

    .ctx =&CTX_NEGATION_NODE_WITH_SYMBOL_AND_BOUND_NOT_TO_A_NUMBER_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a negation node with one child
//  - root->children->children[0]->data->type == TYPE_SYMBOL
//  - in runtime environment:
//    root->children->children[0]->data->data.symbol_value
//    is bound to value such as:
//      - value->type == RUNTIME_VALUE_NUMBER
//      - value->refcount == 1
//      - value->as.i == 42
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_NEGATION_NODE_WITH_SYMBOL_AND_BOUND_TO_A_NUMBER_AND_OOM = {0};
static const test_interpreter_case NEGATION_NODE_WITH_SYMBOL_AND_BOUND_TO_A_NUMBER_AND_OOM = {
    .name = "eval_error_oom_when_negation_node_with_symbol_and_bound_to_a_number_and_oom",

    .root_constructor_fn = &make_root_a_negation_node_with_a_symbol,
    .env_is_dummy = true,
    .oom = true,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NUM_42,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root_and_get_ret,

    .ctx =&CTX_NEGATION_NODE_WITH_SYMBOL_AND_BOUND_TO_A_NUMBER_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a negation node with one child
//  - root->children->children[0]->data->type == TYPE_SYMBOL
//  - in runtime environment:
//    root->children->children[0]->data->data.symbol_value
//    is bound to value such as:
//    - value->type == RUNTIME_VALUE_NUMBER
//    - value->refcount == 1
//    - value->as.i == 42
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_NUMBER
//    && (*out)->refcount == 1
//    && (*out)->as.i == - 42
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_NEGATION_NODE_WITH_SYMBOL_AND_BOUND_TO_A_NUMBER = {0};
static const test_interpreter_case NEGATION_NODE_WITH_SYMBOL_AND_BOUND_TO_A_NUMBER = {
    .name = "eval_success_when_negation_node_with_symbol_and_bound_to_a_number",

    .root_constructor_fn = &make_root_a_negation_node_with_a_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NUM_42,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
    .expected_out_fn = &expected_out_points_on_fresh_number_minus_42,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_get_ret_and_runtime_value_at_out,

    .ctx =&CTX_NEGATION_NODE_WITH_SYMBOL_AND_BOUND_TO_A_NUMBER,
};

// Given:
//  - args are well-formed
//  - root is a negation node
//  - root->children->children[0]->data->type == TYPE_INT
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_NEGATION_NODE_WITH_NUMBER_AND_OOM = {0};
static const test_interpreter_case NEGATION_NODE_WITH_NUMBER_AND_OOM = {
    .name = "eval_error_oom_when_negation_node_with_number_and_oom",

    .root_constructor_fn = &make_root_a_negation_node_with_a_number,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_NEGATION_NODE_WITH_NUMBER_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a negation node
//  - root->children->children[0]->data->type == TYPE_INT
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_NUMBER
//    && (*out)->refcount == 1
//    && (*out)->as.i == - root->children->children[0]->data->data->int_value
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_NEGATION_NODE_WITH_NUMBER_SUCCESS = {0};
static const test_interpreter_case NEGATION_NODE_WITH_NUMBER_SUCCESS = {
    .name = "eval_success_when_negation_node_with_number",

    .root_constructor_fn = &make_root_a_negation_node_with_a_number,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_points_on_negation_result_when_number,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_NEGATION_NODE_WITH_NUMBER_SUCCESS,
};



//-----------------------------------------------------------------------------
// EVALUATION OF AST OF TYPE AST_TYPE_ADDITION
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
    interpreter_status interpreter_eval(
        struct interpreter_ctx *ctx,
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
    - runtime_env_set_local
    - runtime_env_get_local
    - runtime_env_get
  - fake:
    - functions of standard library which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// PARAMETRIC CASES
//-----------------------------------------------------------------------------


// Given:
//  - env and out are valid
//  - root is a ill-formed addition node because:
//    - root->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_ADDITION_NODE_ILL_FORMED_CHILDREN_NULL = {0};
static const test_interpreter_case ADDITION_NODE_ILL_FORMED_CHILDREN_NULL = {
    .name = "eval_error_invalid_ast_when_addition_node_ill_formed_cause_children_null",

    .root_constructor_fn = &make_root_a_ill_formed_addition_node_because_children_null,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_ADDITION_NODE_ILL_FORMED_CHILDREN_NULL,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed addition node because:
//    - root->children->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_ADDITION_NODE_ILL_FORMED_NO_CHILD = {0};
static const test_interpreter_case ADDITION_NODE_ILL_FORMED_NO_CHILD = {
    .name = "eval_error_invalid_ast_when_addition_node_ill_formed_cause_no_child",

    .root_constructor_fn = &make_root_a_ill_formed_addition_node_because_no_child,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_ADDITION_NODE_ILL_FORMED_NO_CHILD,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed addition node because:
//    - root->children->children_nb == 1
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_ADDITION_NODE_ILL_FORMED_ONE_CHILD = {0};
static const test_interpreter_case ADDITION_NODE_ILL_FORMED_ONE_CHILD = {
    .name = "eval_error_invalid_ast_when_addition_node_ill_formed_cause_one_child",

    .root_constructor_fn = &make_root_a_ill_formed_addition_node_because_one_child,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_ADDITION_NODE_ILL_FORMED_ONE_CHILD,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed addition node because:
//    - root->children->children_nb == 3
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_ADDITION_NODE_ILL_FORMED_THREE_CHILDREN = {0};
static const test_interpreter_case ADDITION_NODE_ILL_FORMED_THREE_CHILDREN = {
    .name = "eval_error_invalid_ast_when_addition_node_ill_formed_cause_three_children",

    .root_constructor_fn = &make_root_a_ill_formed_addition_node_because_three_children,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_ADDITION_NODE_ILL_FORMED_THREE_CHILDREN,
};

// Given:
//  - args are well-formed
//  - root is a well-formed addition node
//  - the first child evaluates to number but the second is an unbound symbol
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_LOOKUP_FAILED
static test_interpreter_ctx CTX_ADDITION_NODE_RHS_SYMBOL_UNBOUND = {0};
static const test_interpreter_case ADDITION_NODE_RHS_SYMBOL_UNBOUND = {
    .name = "eval_error_unbound_when_addition_node_and_rhs_symbol_unbound",

    .root_constructor_fn = &make_root_a_well_formed_addition_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NULL,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_LOOKUP_FAILED,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_ADDITION_NODE_RHS_SYMBOL_UNBOUND,
};

// Given:
//  - args are well-formed
//  - root is a well-formed addition node
//  - the first child evaluates to number but the second is an unbound symbol
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_ADDITION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM = {0};
static const test_interpreter_case ADDITION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM = {
    .name = "eval_error_oom_when_addition_node_and_rhs_symbol_unbound_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_addition_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_ADDITION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed addition node
//  - the first child evaluates to number but the second is
//    a symbol bound to a value such as:
//      - value->type == RUNTIME_VALUE_STRING
//      - value->refcount == 1
//      - value->as.s == "abc"
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - calls runtime_env_get with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_TYPE_ERROR
static test_interpreter_ctx CTX_ADDITION_NODE_RHS_NOT_NUMBER = {0};
static const test_interpreter_case ADDITION_NODE_RHS_NOT_NUMBER = {
    .name = "eval_type_error_when_addition_node_and_rhs_not_bound_to_a_number",

    .root_constructor_fn = &make_root_a_well_formed_addition_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_STR_ABC,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_TYPE_ERROR,

    .clean_up_fn = &destroy_root_and_get_ret,

    .ctx =&CTX_ADDITION_NODE_RHS_NOT_NUMBER,
};

// Given:
//  - args are well-formed
//  - root is a well-formed addition node
//  - the first child evaluates to number but the second is a symbol bound to a string
//  - RHS would be a symbol bound to a STRING runtime value if no OOM
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_ADDITION_NODE_RHS_NOT_NUMBER_AND_OOM = {0};
static const test_interpreter_case ADDITION_NODE_RHS_NOT_NUMBER_AND_OOM = {
    .name = "eval_error_oom_when_addition_node_and_rhs_not_bound_to_a_number_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_addition_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_ADDITION_NODE_RHS_NOT_NUMBER_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed addition node
//  - the two children evaluate to numbers
//  - the second child is a symbol whose name is "symbol_name"
//    bound to value such as:
//    - value->type == RUNTIME_VALUE_NUMBER
//    - value->refcount == 1
//    - value->as.i == 42
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - calls get with:
//    - st: ctx.st
//    - name: "symbol_name"
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_NUMBER
//    && (*out)->refcount == 1
//    && (*out)->as.i ==
//        root->children->children[0]->data->data->int_value
//        +
//        <the RUNTIME_VALUE_NUMBER bound to the second child>->as.i
//  - environment is not modified
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_ADDITION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER = {0};
static const test_interpreter_case ADDITION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER = {
    .name = "eval_success_when_addition_node_with_lhs_number_rhs_symbol_to_bound_number",

    .root_constructor_fn = &make_root_a_well_formed_addition_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NUM_42,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL,
    .expected_out_fn = &expected_out_points_on_addition_result_when_lhs_number_rhs_symbol,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_get_ret_and_runtime_value_at_out,

    .ctx =&CTX_ADDITION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER,
};

// Given:
//  - args are well-formed
//  - root is a well-formed addition node
//  - the two children evaluate to numbers
//  - the second child is a symbol bound to a number runtime value
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_ADDITION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_AND_OOM = {0};
static const test_interpreter_case ADDITION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_AND_OOM = {
    .name = "eval_error_oom_when_addition_node_with_lhs_number_rhs_symbol_bound_number_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_addition_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_ADDITION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed addition node
//  - the two children are of type AST_TYPE_DATA_WRAPPER with data of type TYPE_INT
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_ADDITION_NODE_LHS_NUMBER_RHS_NUMBER_AND_OOM = {0};
static const test_interpreter_case ADDITION_NODE_LHS_NUMBER_RHS_NUMBER_AND_OOM = {
    .name = "eval_error_oom_when_addition_node_with_two_numbers_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_addition_of_two_numbers,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_ADDITION_NODE_LHS_NUMBER_RHS_NUMBER_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed addition node
//  - the two children are of type AST_TYPE_DATA_WRAPPER with data of type TYPE_INT
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_NUMBER
//    && (*out)->as.i ==
//           root->children->children[0]->data->data->int_value
//           +
//           root->children->children[1]->data->data->int_value
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_ADDITION_NODE_LHS_NUMBER_RHS_NUMBER = {0};
static const test_interpreter_case ADDITION_NODE_LHS_NUMBER_RHS_NUMBER = {
    .name = "eval_success_when_addition_node_with_two_numbers",

    .root_constructor_fn = &make_root_a_well_formed_addition_of_two_numbers,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_points_on_addition_result_when_two_numbers,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_ADDITION_NODE_LHS_NUMBER_RHS_NUMBER,
};



//-----------------------------------------------------------------------------
// EVALUATION OF AST OF TYPE AST_TYPE_SUBTRACTION
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
    interpreter_status interpreter_eval(
        struct interpreter_ctx *ctx,
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
    - runtime_env_set_local
    - runtime_env_get_local
    - runtime_env_get
  - fake:
    - functions of standard library which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// PARAMETRIC CASES
//-----------------------------------------------------------------------------


// Given:
//  - env and out are valid
//  - root is a ill-formed subtraction node because:
//    - root->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_SUBTRACTION_NODE_ILL_FORMED_CHILDREN_NULL = {0};
static const test_interpreter_case SUBTRACTION_NODE_ILL_FORMED_CHILDREN_NULL = {
    .name = "eval_error_invalid_ast_when_subtraction_node_ill_formed_cause_children_null",

    .root_constructor_fn = &make_root_a_ill_formed_subtraction_node_because_children_null,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_SUBTRACTION_NODE_ILL_FORMED_CHILDREN_NULL,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed subtraction node because:
//    - root->children->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_SUBTRACTION_NODE_ILL_FORMED_NO_CHILD = {0};
static const test_interpreter_case SUBTRACTION_NODE_ILL_FORMED_NO_CHILD = {
    .name = "eval_error_invalid_ast_when_subtraction_node_ill_formed_cause_no_child",

    .root_constructor_fn = &make_root_a_ill_formed_subtraction_node_because_no_child,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_SUBTRACTION_NODE_ILL_FORMED_NO_CHILD,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed subtraction node because:
//    - root->children->children_nb == 1
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_SUBTRACTION_NODE_ILL_FORMED_ONE_CHILD = {0};
static const test_interpreter_case SUBTRACTION_NODE_ILL_FORMED_ONE_CHILD = {
    .name = "eval_error_invalid_ast_when_subtraction_node_ill_formed_cause_one_child",

    .root_constructor_fn = &make_root_a_ill_formed_subtraction_node_because_one_child,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_SUBTRACTION_NODE_ILL_FORMED_ONE_CHILD,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed subtraction node because:
//    - root->children->children_nb == 3
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_SUBTRACTION_NODE_ILL_FORMED_THREE_CHILDREN = {0};
static const test_interpreter_case SUBTRACTION_NODE_ILL_FORMED_THREE_CHILDREN = {
    .name = "eval_error_invalid_ast_when_subtraction_node_ill_formed_cause_three_children",

    .root_constructor_fn = &make_root_a_ill_formed_subtraction_node_because_three_children,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_SUBTRACTION_NODE_ILL_FORMED_THREE_CHILDREN,
};

// Given:
//  - args are well-formed
//  - root is a well-formed subtraction node
//  - the first child evaluates to number but the second is an unbound symbol
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_LOOKUP_FAILED
static test_interpreter_ctx CTX_SUBTRACTION_NODE_RHS_SYMBOL_UNBOUND = {0};
static const test_interpreter_case SUBTRACTION_NODE_RHS_SYMBOL_UNBOUND = {
    .name = "eval_error_unbound_when_subtraction_node_and_rhs_symbol_unbound",

    .root_constructor_fn = &make_root_a_well_formed_subtraction_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NULL,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_LOOKUP_FAILED,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_SUBTRACTION_NODE_RHS_SYMBOL_UNBOUND,
};

// Given:
//  - args are well-formed
//  - root is a well-formed subtraction node
//  - the first child evaluates to number but the second is an unbound symbol
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_SUBTRACTION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM = {0};
static const test_interpreter_case SUBTRACTION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM = {
    .name = "eval_error_oom_when_subtraction_node_and_rhs_symbol_unbound_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_subtraction_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_SUBTRACTION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed subtraction node
//  - the first child evaluates to number but the second
//    is bound to
//    a runtime value which is not of type RUNTIME_VALUE_NUMBER
//    (it is bound to a RUNTIME_VALUE_STRING "abc")
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - calls get with:
//    - st: ctx.st
//    - name: "symbol_name"
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_TYPE_ERROR
static test_interpreter_ctx CTX_SUBTRACTION_RHS_NOT_NUMBER = {0};
static const test_interpreter_case SUBTRACTION_RHS_NOT_NUMBER = {
    .name = "eval_type_error_when_subtraction_node_and_rhs_not_bound_to_a_number",

    .root_constructor_fn = &make_root_a_well_formed_subtraction_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_STR_ABC,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_TYPE_ERROR,

    .clean_up_fn = &destroy_root_and_get_ret,

    .ctx =&CTX_SUBTRACTION_RHS_NOT_NUMBER,
};

// Given:
//  - args are well-formed
//  - root is a well-formed subtraction node
//  - the first child evaluates to number but the second is a symbol bound to a string
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_SUBTRACTION_NODE_RHS_NOT_NUMBER_AND_OOM = {0};
static const test_interpreter_case SUBTRACTION_NODE_RHS_NOT_NUMBER_AND_OOM = {
    .name = "eval_error_oom_when_subtraction_node_and_rhs_not_bound_to_a_number_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_subtraction_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_SUBTRACTION_NODE_RHS_NOT_NUMBER_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed subtraction node
//  - the two children evaluate to numbers
//  - the second child is a symbol whose name is "symbol_name"
//    bound to value such as:
//    - value->type == RUNTIME_VALUE_NUMBER
//    - value->refcount == 1
//    - value->as.i == 42
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - calls get with:
//    - st: ctx.st
//    - name: "symbol_name"
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_NUMBER
//    && (*out)->refcount == 1
//    && (*out)->as.i ==
//        root->children->children[0]->data->data->int_value
//        -
//        <number runtime value bound to the second child>->as.i
//  - environment is not modified
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_SUBTRACTION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER = {0};
static const test_interpreter_case SUBTRACTION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER = {
    .name = "eval_success_when_subtraction_node_with_lhs_number_rhs_symbol_bound_to_number",

    .root_constructor_fn = &make_root_a_well_formed_subtraction_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NUM_42,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL,
    .expected_out_fn = &expected_out_points_on_subtraction_result_when_lhs_number_rhs_symbol,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_get_ret_and_runtime_value_at_out,

    .ctx =&CTX_SUBTRACTION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER,
};

// Given:
//  - args are well-formed
//  - root is a well-formed subtraction node
//  - the two children evaluate to numbers
//  - the second child is a symbol bound to a number runtime value
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_SUBTRACTION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_AND_OOM = {0};
static const test_interpreter_case SUBTRACTION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_AND_OOM = {
    .name = "eval_error_oom_when_subtraction_node_with_lhs_number_rhs_symbol_bound_number_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_subtraction_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_SUBTRACTION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed subtraction node
//  - the two children are of type AST_TYPE_DATA_WRAPPER with data of type TYPE_INT
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_SUBTRACTION_NODE_LHS_NUMBER_RHS_NUMBER_AND_OOM = {0};
static const test_interpreter_case SUBTRACTION_NODE_LHS_NUMBER_RHS_NUMBER_AND_OOM = {
    .name = "eval_error_oom_when_subtraction_node_with_two_numbers_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_subtraction_of_two_numbers,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_SUBTRACTION_NODE_LHS_NUMBER_RHS_NUMBER_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed subtraction node
//  - the two children are of type AST_TYPE_DATA_WRAPPER with data of type TYPE_INT
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_NUMBER
//    && (*out)->as.i ==
//           root->children->children[0]->data->data->int_value
//           -
//           root->children->children[1]->data->data->int_value
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_SUBTRACTION_NODE_LHS_NUMBER_RHS_NUMBER = {0};
static const test_interpreter_case SUBTRACTION_NODE_LHS_NUMBER_RHS_NUMBER = {
    .name = "eval_success_when_subtraction_node_with_two_numbers",

    .root_constructor_fn = &make_root_a_well_formed_subtraction_of_two_numbers,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_points_on_subtraction_result_when_two_numbers,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_SUBTRACTION_NODE_LHS_NUMBER_RHS_NUMBER,
};



//-----------------------------------------------------------------------------
// EVALUATION OF AST OF TYPE AST_TYPE_MULTIPLICATION
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
    interpreter_status interpreter_eval(
        struct interpreter_ctx *ctx,
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
    - runtime_env_set_local
    - runtime_env_get_local
    - runtime_env_get
  - fake:
    - functions of standard library which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// PARAMETRIC CASES
//-----------------------------------------------------------------------------


// Given:
//  - env and out are valid
//  - root is a ill-formed multiplication node because:
//    - root->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_MULTIPLICATION_NODE_ILL_FORMED_CHILDREN_NULL = {0};
static const test_interpreter_case MULTIPLICATION_NODE_ILL_FORMED_CHILDREN_NULL = {
    .name = "eval_error_invalid_ast_when_multiplication_node_ill_formed_cause_children_null",

    .root_constructor_fn = &make_root_a_ill_formed_multiplication_node_because_children_null,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_MULTIPLICATION_NODE_ILL_FORMED_CHILDREN_NULL,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed multiplication node because:
//    - root->children->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_MULTIPLICATION_NODE_ILL_FORMED_NO_CHILD = {0};
static const test_interpreter_case MULTIPLICATION_NODE_ILL_FORMED_NO_CHILD = {
    .name = "eval_error_invalid_ast_when_multiplication_node_ill_formed_cause_no_child",

    .root_constructor_fn = &make_root_a_ill_formed_multiplication_node_because_no_child,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_MULTIPLICATION_NODE_ILL_FORMED_NO_CHILD,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed multiplication node because:
//    - root->children->children_nb == 1
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_MULTIPLICATION_NODE_ILL_FORMED_ONE_CHILD = {0};
static const test_interpreter_case MULTIPLICATION_NODE_ILL_FORMED_ONE_CHILD = {
    .name = "eval_error_invalid_ast_when_multiplication_node_ill_formed_cause_one_child",

    .root_constructor_fn = &make_root_a_ill_formed_multiplication_node_because_one_child,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_MULTIPLICATION_NODE_ILL_FORMED_ONE_CHILD,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed multiplication node because:
//    - root->children->children_nb == 3
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_MULTIPLICATION_NODE_ILL_FORMED_THREE_CHILDREN = {0};
static const test_interpreter_case MULTIPLICATION_NODE_ILL_FORMED_THREE_CHILDREN = {
    .name = "eval_error_invalid_ast_when_multiplication_node_ill_formed_cause_three_children",

    .root_constructor_fn = &make_root_a_ill_formed_multiplication_node_because_three_children,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_MULTIPLICATION_NODE_ILL_FORMED_THREE_CHILDREN,
};

// Given:
//  - args are well-formed
//  - root is a well-formed multiplication node
//  - the first child evaluates to number but the second is an unbound symbol whose name is "symbol_name"
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - calls get with:
//    - st: ctx.st
//    - name: "symbol_name"
//  -    *out != NULL
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_LOOKUP_FAILED
static test_interpreter_ctx CTX_MULTIPLICATION_NODE_RHS_SYMBOL_UNBOUND = {0};
static const test_interpreter_case MULTIPLICATION_NODE_RHS_SYMBOL_UNBOUND = {
    .name = "eval_error_unbound_when_multiplication_node_and_rhs_symbol_unbound",

    .root_constructor_fn = &make_root_a_well_formed_multiplication_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NULL,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_LOOKUP_FAILED,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_MULTIPLICATION_NODE_RHS_SYMBOL_UNBOUND,
};

// Given:
//  - args are well-formed
//  - root is a well-formed multiplication node
//  - the first child evaluates to number but the second is an unbound symbol
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_MULTIPLICATION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM = {0};
static const test_interpreter_case MULTIPLICATION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM = {
        .name = "eval_error_oom_when_multiplication_node_and_rhs_symbol_unbound_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_multiplication_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_MULTIPLICATION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed multiplication node
//  - the first child evaluates to number but the second is
//    a symbol whose name is "symbol_name" bound to a value such as:
//      - value->type == RUNTIME_VALUE_STRING
//      - value->refcount == 1
//      - value->as.s == "abc"
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - calls get with:
//    - st: ctx.st
//    - name: "symbol_name"
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_TYPE_ERROR
static test_interpreter_ctx CTX_MULTIPLICATION_NODE_RHS_NOT_NUMBER = {0};
static const test_interpreter_case MULTIPLICATION_NODE_RHS_NOT_NUMBER = {
    .name = "eval_type_error_when_multiplication_node_and_rhs_not_bound_to_a_number",

    .root_constructor_fn = &make_root_a_well_formed_multiplication_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_STR_ABC,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_TYPE_ERROR,

    .clean_up_fn = &destroy_root_and_get_ret,

    .ctx =&CTX_MULTIPLICATION_NODE_RHS_NOT_NUMBER,
};

// Given:
//  - args are well-formed
//  - root is a well-formed multiplication node
//  - the first child evaluates to number but the second is a symbol bound to a string
//  - RHS would be a symbol bound to a STRING runtime value if no OOM
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_MULTIPLICATION_NODE_RHS_NOT_NUMBER_AND_OOM = {0};
static const test_interpreter_case MULTIPLICATION_NODE_RHS_NOT_NUMBER_AND_OOM = {
    .name = "eval_error_oom_when_multiplication_node_and_rhs_not_bound_to_a_number_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_multiplication_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_MULTIPLICATION_NODE_RHS_NOT_NUMBER_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed multiplication node
//  - the two children evaluate to numbers
//  - the second child is a symbol whose name is "symbol_name"
//    bound to value such as:
//    - value->type == RUNTIME_VALUE_NUMBER
//    - value->refcount == 1
//    - value->as.i == 42
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - calls get with:
//    - st: ctx.st
//    - name: "symbol_name"
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_NUMBER
//    && (*out)->refcount == 1
//    && (*out)->as.i ==
//        root->children->children[0]->data->data->int_value
//        *
//        <number runtime value bound to the second child>->as.i
//  - environment is not modified
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_MULTIPLICATION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER = {0};
static const test_interpreter_case MULTIPLICATION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER = {
    .name = "eval_success_when_multiplication_node_with_lhs_number_rhs_symbol_to_bound_number",

    .root_constructor_fn = &make_root_a_well_formed_multiplication_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NUM_42,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL,
    .expected_out_fn = &expected_out_points_on_multiplication_result_when_lhs_number_rhs_symbol,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_get_ret_and_runtime_value_at_out,

    .ctx =&CTX_MULTIPLICATION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER,
};

// Given:
//  - args are well-formed
//  - root is a well-formed multiplication node
//  - the two children evaluate to numbers
//  - the second child is a symbol bound to a number runtime value
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_MULTIPLICATION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_AND_OOM = {0};
static const test_interpreter_case MULTIPLICATION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_AND_OOM = {
    .name = "eval_error_oom_when_multiplication_node_with_lhs_number_rhs_symbol_bound_number_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_multiplication_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_MULTIPLICATION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed multiplication node
//  - the two children are of type AST_TYPE_DATA_WRAPPER with data of type TYPE_INT
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_MULTIPLICATION_NODE_LHS_NUMBER_RHS_NUMBER_AND_OOM = {0};
static const test_interpreter_case MULTIPLICATION_NODE_LHS_NUMBER_RHS_NUMBER_AND_OOM = {
    .name = "eval_error_oom_when_multiplication_node_with_two_numbers_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_multiplication_of_two_numbers,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_MULTIPLICATION_NODE_LHS_NUMBER_RHS_NUMBER_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed multiplication node
//  - the two children are of type AST_TYPE_DATA_WRAPPER with data of type TYPE_INT
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_NUMBER
//    && (*out)->as.i ==
//           root->children->children[0]->data->data->int_value
//           *
//           root->children->children[1]->data->data->int_value
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_MULTIPLICATION_NODE_LHS_NUMBER_RHS_NUMBER = {0};
static const test_interpreter_case MULTIPLICATION_NODE_LHS_NUMBER_RHS_NUMBER = {
    .name = "eval_success_when_multiplication_node_with_two_numbers",

    .root_constructor_fn = &make_root_a_well_formed_multiplication_of_two_numbers,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_points_on_multiplication_result_when_two_numbers,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_MULTIPLICATION_NODE_LHS_NUMBER_RHS_NUMBER,
};



//-----------------------------------------------------------------------------
// EVALUATION OF AST OF TYPE AST_TYPE_DIVISION
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
    interpreter_status interpreter_eval(
        struct interpreter_ctx *ctx,
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
    - runtime_env_set_local
    - runtime_env_get_local
    - runtime_env_get
  - fake:
    - functions of standard library which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// PARAMETRIC CASES
//-----------------------------------------------------------------------------


// Given:
//  - env and out are valid
//  - root is a ill-formed division node because:
//    - root->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_DIVISION_NODE_ILL_FORMED_CHILDREN_NULL = {0};
static const test_interpreter_case DIVISION_NODE_ILL_FORMED_CHILDREN_NULL = {
    .name = "eval_error_invalid_ast_when_division_node_ill_formed_cause_children_null",

    .root_constructor_fn = &make_root_a_ill_formed_division_node_because_children_null,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_DIVISION_NODE_ILL_FORMED_CHILDREN_NULL,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed division node because:
//    - root->children->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_DIVISION_NODE_ILL_FORMED_NO_CHILD = {0};
static const test_interpreter_case DIVISION_NODE_ILL_FORMED_NO_CHILD = {
    .name = "eval_error_invalid_ast_when_division_node_ill_formed_cause_no_child",

    .root_constructor_fn = &make_root_a_ill_formed_division_node_because_no_child,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_DIVISION_NODE_ILL_FORMED_NO_CHILD,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed division node because:
//    - root->children->children_nb == 1
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_DIVISION_NODE_ILL_FORMED_ONE_CHILD = {0};
static const test_interpreter_case DIVISION_NODE_ILL_FORMED_ONE_CHILD = {
    .name = "eval_error_invalid_ast_when_division_node_ill_formed_cause_one_child",

    .root_constructor_fn = &make_root_a_ill_formed_division_node_because_one_child,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_DIVISION_NODE_ILL_FORMED_ONE_CHILD,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed division node because:
//    - root->children->children_nb == 3
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_DIVISION_NODE_ILL_FORMED_THREE_CHILDREN = {0};
static const test_interpreter_case DIVISION_NODE_ILL_FORMED_THREE_CHILDREN = {
    .name = "eval_error_invalid_ast_when_division_node_ill_formed_cause_three_children",

    .root_constructor_fn = &make_root_a_ill_formed_division_node_because_three_children,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_DIVISION_NODE_ILL_FORMED_THREE_CHILDREN,
};

// Given:
//  - args are well-formed
//  - root is a well-formed division node
//  - the first child evaluates to number but the second is an unbound symbol
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_LOOKUP_FAILED
static test_interpreter_ctx CTX_DIVISION_NODE_RHS_SYMBOL_UNBOUND = {0};
static const test_interpreter_case DIVISION_NODE_RHS_SYMBOL_UNBOUND = {
    .name = "eval_error_unbound_when_division_node_and_rhs_symbol_unbound",

    .root_constructor_fn = &make_root_a_well_formed_division_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NULL,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_LOOKUP_FAILED,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_DIVISION_NODE_RHS_SYMBOL_UNBOUND,
};

// Given:
//  - args are well-formed
//  - root is a well-formed division node
//  - the first child evaluates to number but the second is an unbound symbol
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_DIVISION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM = {0};
static const test_interpreter_case DIVISION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM = {
    .name = "eval_error_oom_when_division_node_and_rhs_symbol_unbound_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_division_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_DIVISION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed division node
//  - the first child evaluates to number but the second is
//    a symbol bound to a value such as:
//      - value->type == RUNTIME_VALUE_STRING
//      - value->refcount == 1
//      - value->as.s == "abc"
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - calls get with:
//    - st: ctx.st
//    - name: "symbol_name"
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_TYPE_ERROR
static test_interpreter_ctx CTX_DIVISION_NODE_RHS_NOT_NUMBER = {0};
static const test_interpreter_case DIVISION_NODE_RHS_NOT_NUMBER = {
    .name = "eval_type_error_when_division_node_and_rhs_not_bound_to_a_number",

    .root_constructor_fn = &make_root_a_well_formed_division_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_STR_ABC,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_TYPE_ERROR,

    .clean_up_fn = &destroy_root_and_get_ret,

    .ctx =&CTX_DIVISION_NODE_RHS_NOT_NUMBER,
};

// Given:
//  - args are well-formed
//  - root is a well-formed division node
//  - the first child evaluates to number but the second is a symbol bound to a string
//  - RHS would be a symbol bound to a STRING runtime value if no OOM
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_DIVISION_NODE_RHS_NOT_NUMBER_AND_OOM = {0};
static const test_interpreter_case DIVISION_NODE_RHS_NOT_NUMBER_AND_OOM = {
    .name = "eval_error_oom_when_division_node_and_rhs_not_bound_to_a_number_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_division_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_DIVISION_NODE_RHS_NOT_NUMBER_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed division node
//  - the two children evaluate to numbers
//  - the second child is a symbol whose name is "symbol_name"
//    bound to value such as:
//    - value->type == RUNTIME_VALUE_NUMBER
//    - value->refcount == 1
//    - value->as.i == 0
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - calls get with:
//    - st: ctx.st
//    - name: "symbol_name"
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_DIVISION_BY_ZERO
static test_interpreter_ctx CTX_DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_ZERO = {0};
static const test_interpreter_case DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_ZERO = {
    .name = "eval_error_division_by_zero_when_division_node_with_lhs_number_rhs_symbol_to_bound_number_zero",

    .root_constructor_fn = &make_root_a_well_formed_division_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NUM_0,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_DIVISION_BY_ZERO,

    .clean_up_fn = &destroy_root_and_get_ret,

    .ctx =&CTX_DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_ZERO,
};

// Given:
//  - args are well-formed
//  - root is a well-formed division node
//  - the two children evaluate to numbers
//  - the second child is a symbol bound to a number runtime value with value zero
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_ZERO_AND_OOM = {0};
static const test_interpreter_case DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_ZERO_AND_OOM = {
    .name = "eval_error_oom_when_division_node_with_lhs_number_rhs_symbol_to_bound_number_zero_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_division_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_ZERO_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed division node
//  - the two children evaluate to numbers
//  - the second child is a symbol whose name is "symbol_name"
//    bound to value such as:
//    - value->type == RUNTIME_VALUE_NUMBER
//    - value->refcount == 1
//    - value->as.i == 42
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_NUMBER
//    && (*out)->refcount == 1
//    && (*out)->as.i ==
//        root->children->children[0]->data->data->int_value
//        /
//        <number runtime value bound to the second child>->as.i
//  - environment is not modified
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_NOT_ZERO = {0};
static const test_interpreter_case DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_NOT_ZERO = {
    .name = "eval_success_when_division_node_with_lhs_number_rhs_symbol_to_bound_number_not_zero",

    .root_constructor_fn = &make_root_a_well_formed_division_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NUM_42,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_RHS_SYMBOL,
    .expected_out_fn = &expected_out_points_on_division_result_when_lhs_number_rhs_symbol,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_get_ret_and_runtime_value_at_out,

    .ctx =&CTX_DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_NOT_ZERO,
};

// Given:
//  - args are well-formed
//  - root is a well-formed division node
//  - the two children evaluate to numbers
//  - the second child is a symbol bound to a number runtime value with value not zero
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_NOT_ZERO_AND_OOM = {0};
static const test_interpreter_case DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_NOT_ZERO_AND_OOM = {
    .name = "eval_error_oom_when_division_node_with_lhs_number_rhs_symbol_to_bound_number_not_zero_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_division_with_lhs_number_rhs_symbol,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_NOT_ZERO_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed division node
//  - the two children are of type AST_TYPE_DATA_WRAPPER with data of type TYPE_INT
//  - the second child has value 0
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_ZERO_AND_OOM = {0};
static const test_interpreter_case DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_ZERO_AND_OOM = {
    .name = "eval_error_oom_when_division_by_zero_node_with_two_numbers_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_division_by_zero_of_two_numbers,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_ZERO_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed division node
//  - the two children are of type AST_TYPE_DATA_WRAPPER with data of type TYPE_INT
//  - the second child has value 0
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_DIVISION_BY_ZERO
static test_interpreter_ctx CTX_DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_ZERO = {0};
static const test_interpreter_case DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_ZERO = {
    .name = "eval_error_division_by_zero_when_division_by_zero_node_with_two_numbers",

    .root_constructor_fn = &make_root_a_well_formed_division_by_zero_of_two_numbers,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_DIVISION_BY_ZERO,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_ZERO,
};

// Given:
//  - args are well-formed
//  - root is a well-formed division node
//  - the two children are of type AST_TYPE_DATA_WRAPPER with data of type TYPE_INT
//  - the second child value is not 0
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_NOT_ZERO_AND_OOM = {0};
static const test_interpreter_case DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_NOT_ZERO_AND_OOM = {
    .name = "eval_error_oom_when_division_not_by_zero_node_with_two_numbers_and_oom",

    .root_constructor_fn = &make_root_a_well_formed_division_not_by_zero_of_two_numbers,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_NOT_ZERO_AND_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed division node
//  - the two children are of type AST_TYPE_DATA_WRAPPER with data of type TYPE_INT
//  - the second child value is not 0
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_NUMBER
//    && (*out)->as.i ==
//           root->children->children[0]->data->data->int_value
//           /
//           root->children->children[1]->data->data->int_value
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_NOT_ZERO = {0};
static const test_interpreter_case DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_NOT_ZERO = {
    .name = "eval_success_when_division_not_by_zero_node_with_two_numbers",

    .root_constructor_fn = &make_root_a_well_formed_division_not_by_zero_of_two_numbers,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_points_on_division_result_when_two_numbers,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_NOT_ZERO,
};



//-----------------------------------------------------------------------------
// EVALUATION OF AST OF TYPE TYPE AST_TYPE_QUOTE
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
    interpreter_status interpreter_eval(
        struct interpreter_ctx *ctx,
        struct runtime_env *env,
        const struct ast *root,
        struct runtime_env_value **out );
other elements of the isolated unit:
  - root
  - out
  - from the runtime_env module:
    - runtime_env_make_quoted
    - runtime_env_value_destroy
    - runtime_env_make_number
  - from the ast module:
    - ast_create_int_node
    - ast_create_symbol_node
    - ast_create_children_node_var
    - ast_destroy

doubles:
  - dummy:
    - env except for success path
  - spy:
    - runtime_env_set_local
    - runtime_env_get_local
    - runtime_env_get
  - fake:
    - functions of standard library which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// PARAMETRIC CASES
//-----------------------------------------------------------------------------


// Given:
//  - env and out are valid
//  - root is a ill-formed quote node because:
//    - root->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_QUOTE_NODE_ILL_FORMED_CHILDREN_NULL = {0};
static const test_interpreter_case QUOTE_NODE_ILL_FORMED_CHILDREN_NULL = {
    .name = "eval_error_invalid_ast_when_quote_node_ill_formed_cause_children_null",

    .root_constructor_fn = &make_root_a_ill_formed_quote_node_because_children_null,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_QUOTE_NODE_ILL_FORMED_CHILDREN_NULL,
};

// Given:
//  - args are well-formed
//  - root is a quote node
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_QUOTE_NODE_OOM = {0};
static const test_interpreter_case QUOTE_NODE_OOM = {
    .name = "eval_error_oom_when_quote_node_and_malloc_fails",

    .root_constructor_fn = &make_root_a_quote_node_with_a_function_call_child_with_params_7_8,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_QUOTE_NODE_OOM,
};

// Given:
//  - args are well-formed
//  - root is a quote node
//  - all allocations will succeed during interpreter_eval call
// Expected:
//  - environment is not modified
//  -    *out != NULL
//    && (*out)->type == RUNTIME_VALUE_QUOTED
//    && (*out)->refcount == 1
//    && (*out)->as.quoted == root->children->children[0]
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_QUOTE_NODE = {0};
static const test_interpreter_case QUOTE_NODE = {
    .name = "eval_success_when_quote_node",

    .root_constructor_fn = &make_root_a_quote_node_with_a_function_call_child_with_params_7_8,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_points_on_quoted_value,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_QUOTE_NODE,
};



//-----------------------------------------------------------------------------
// EVALUATION OF AST OF TYPE AST_TYPE_READING
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
    interpreter_status interpreter_eval(
        struct interpreter_ctx *ctx,
        struct runtime_env *env,
        const struct ast *root,
        struct runtime_env_value **out );
other elements of the isolated unit:
  - root
  - out
  - env for success path
- from the runtime_env module:
    - runtime_env_value_destroy
    - runtime_env_make_number
  - from the ast module:
    - ast_create_int_node
    - ast_create_symbol_node
    - ast_create_children_node_var
    - ast_destroy

doubles:
  - dummy:
    - env except for success path
  - spy:
    - read_ast
    - runtime_env_set_local
  - fake:
    - functions of standard library which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// PARAMETRIC CASES
//-----------------------------------------------------------------------------


// Given:
//  - env and out are valid
//  - root is a ill-formed reading node because:
//    - root->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_READING_NODE_ILL_FORMED_NO_CHILD = {0};
static const test_interpreter_case READING_NODE_ILL_FORMED_NO_CHILD = {
    .name = "eval_error_invalid_ast_when_reading_node_ill_formed_cause_no_child",

    .root_constructor_fn = &make_root_a_ill_formed_reading_node_because_no_child,
    .env_is_dummy = true,
    .oom = false,

	.read_ast_assert_kind = READ_AST_ASRT_NONE,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_READING_NODE_ILL_FORMED_NO_CHILD,
};

// Given:
//  - env and out are valid
//  - root is a ill-formed reading node because:
//    - root->children->children_nb == 3
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_READING_NODE_ILL_FORMED_THREE_CHILDREN = {0};
static const test_interpreter_case READING_NODE_ILL_FORMED_THREE_CHILDREN = {
    .name = "eval_error_invalid_ast_when_reading_node_ill_formed_cause_three_children",

    .root_constructor_fn = &make_root_a_ill_formed_reading_node_because_three_children,
    .env_is_dummy = true,
    .oom = false,

	.read_ast_assert_kind = READ_AST_ASRT_NONE,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_READING_NODE_ILL_FORMED_THREE_CHILDREN,
};

// Given:
//  - args are well-formed
//  - root is a well-formed reading node
//  - all allocations will fail during interpreter_eval call
// Expected:
//  - calls read_ast_fn with:
//    - ctx: arg_ctx
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_READ_AST_ERROR
static test_interpreter_ctx CTX_READING_NODE_OOM = {0};
static const test_interpreter_case READING_NODE_OOM = {
    .name = "eval_error_when_reading_node_and_oom",

    .root_constructor_fn = &make_root_a_reading_node,
    .env_is_dummy = true,
    .oom = true,
	.read_ast_arrange_kind = READ_AST_ARR_READ_FAIL,

	.read_ast_assert_kind = READ_AST_ASRT_CALL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_READ_AST_ERROR,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_READING_NODE_OOM,
};

// Given:
//  - args are well-formed
//  - root is a well-formed reading node
//  - all allocations will succeed during interpreter_eval call
//  - host_read_ast_fn call returns NULL (i.e. it fails)
// Expected:
//  - calls read_ast_fn with:
//    - ctx: arg_ctx
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_READ_AST_ERROR
static test_interpreter_ctx CTX_READING_NODE_CALLBACK_READ_AST_FAILS = {0};
static const test_interpreter_case READING_NODE_CALLBACK_READ_AST_FAILS = {
    .name = "eval_error_when_reading_node_and_read_ast_fails",

    .root_constructor_fn = &make_root_a_reading_node,
    .env_is_dummy = true,
    .oom = false,
	.read_ast_arrange_kind = READ_AST_ARR_READ_FAIL,

	.read_ast_assert_kind = READ_AST_ASRT_CALL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_READ_AST_ERROR,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_READING_NODE_CALLBACK_READ_AST_FAILS,
};

// Given:
//  - args are well-formed
//  - root is a well-formed reading node
//  - all allocations will succeed during interpreter_eval call
//  - host_read_ast_fn call returns an ast data wrapper for a number 42
//  - symbol binding will fail
// Expected:
//  - calls read_ast_fn with:
//    - ctx: arg_ctx
//  - calls runtime_env_set_local with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//    - value: <evaluated_rhs owned by implementation> such as:
//      - evaluated_rhs != NULL
//      - evaluated_rhs->type == RUNTIME_VALUE_NUMBER
//      - evaluated_rhs->refcount == 1
//      - evaluated_rhs->as.i == 42
//  - destroys evaluated_rhs
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_BINDING_ERROR
static test_interpreter_ctx CTX_READING_NODE_BINDING_FAILS = {0};
static const test_interpreter_case READING_NODE_BINDING_FAILS = {
    .name = "eval_error_when_reading_node_and_binding_fails",

    .root_constructor_fn = &make_root_a_reading_node,
    .env_is_dummy = true,
    .oom = false,
	.read_ast_arrange_kind = READ_AST_ARR_READ_NUM_42,
	.runtime_env_arrange_kind = ENV_ARR_SET_LOCAL_RET_FALSE,

	.read_ast_assert_kind = READ_AST_ASRT_CALL,
	.runtime_env_assert_kind = ENV_ASRT_EXPECT_SET_LOCAL_BND_CHILD_TO_READ,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_BINDING_ERROR,

    .clean_up_fn = &destroy_root_and_read_ast,

    .ctx =&CTX_READING_NODE_BINDING_FAILS,
};

// Given:
//  - args are well-formed
//  - root is a well-formed reading node
//  - all allocations will succeed during interpreter_eval call
//  - host_read_ast_fn call returns an ast data wrapper for a number 42
//  - symbol binding will succeed
// Expected:
//  - calls read_ast_fn with:
//    - ctx: arg_ctx
//  - calls runtime_env_set_local with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//    - value: <evaluated_rhs owned by implementation> such as:
//      - evaluated_rhs != NULL
//      - evaluated_rhs->type == RUNTIME_VALUE_NUMBER
//      - evaluated_rhs->refcount == 2 (after ACT test stage)
//      - evaluated_rhs->as.i == 42
//  - root->children->children[0]->data->data.symbol_value is bound to evaluated_rhs
//  - *out == evaluated_rhs
//      && (*out)->type == RUNTIME_VALUE_NUMBER
//      && (*out)->refcount == 2
//      && (*out)->as.i == 42
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_READING_NODE_SUCCESS = {0};
static const test_interpreter_case READING_NODE_SUCCESS = {
    .name = "eval_success_when_reading_node_and_eval_read_make_42_and_binding_succeeds",

    .root_constructor_fn = &make_root_a_reading_node,
    .env_is_dummy = false,
    .oom = false,
	.read_ast_arrange_kind = READ_AST_ARR_READ_NUM_42,
	.runtime_env_arrange_kind = ENV_ARR_SET_LOCAL_RET_TRUE,

	.read_ast_assert_kind = READ_AST_ASRT_CALL,
	.runtime_env_assert_kind = ENV_ASRT_EXPECT_SET_LOCAL_BND_CHILD_TO_READ,
    .expected_out_fn = &expected_out_points_on_bound_number_42,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_read_ast_and_runtime_value_at_out,

    .ctx =&CTX_READING_NODE_SUCCESS,
};



//-----------------------------------------------------------------------------
// EVALUATION OF AST OF TYPE AST_TYPE_BINDING
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
    interpreter_status interpreter_eval(
        struct interpreter_ctx *ctx,
        struct runtime_env *env,
        const struct ast *root,
        struct runtime_env_value **out );
other elements of the isolated unit:
  - root
  - out
  - ast_is_well_formed_binding
- from the runtime_env module:
  - runtime_env_make_number
  - runtime_env_value_retain
  - runtime_env_value_release
  - runtime_env_value_destroy
- from the ast module:
  - ast_create_symbol_node
  - ast_create_int_node
  - ast_create_children_node_var
  - ast_destroy

doubles:
  - dummy:
    - env
- spy:
    - runtime_env_set_local
- fake:
  - functions of standard library which are used:
    - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// PARAMETRIC CASES
//-----------------------------------------------------------------------------


// Given:
//  - env and out are valid
//  - root is a ill-formed binding node because:
//    - root->children == NULL
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_BINDING_NODE_ILL_FORMED_NO_CHILD = {0};
static const test_interpreter_case BINDING_NODE_ILL_FORMED_NO_CHILD = {
    .name = "eval_error_invalid_ast_when_binding_node_ill_formed_cause_no_child",

    .root_constructor_fn = &make_root_a_ill_formed_binding_node_because_no_child,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_BINDING_NODE_ILL_FORMED_NO_CHILD,
};

// Given:
//  - env and out are valid
//  - root is a well-formed binding node with rhs 42
//  - oom
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_BINDING_NODE_OOM = {0};
static const test_interpreter_case BINDING_NODE_OOM = {
    .name = "eval_error_when_binding_node_and_oom",

    .root_constructor_fn = &make_root_a_binding_node_with_rhs_number_42,
    .env_is_dummy = true,
    .oom = true,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_BINDING_NODE_OOM,
};

// Given:
//  - env and out are valid
//  - root is a well-formed binding node with rhs 42
//  - symbol binding will fail
// Expected:
//  - rhs is correctly evaluated to a RUNTIME_VALUE_NUMBER 42 evaluated_rhs
//  - calls runtime_env_set_local with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//    - value: evaluated_rhs
//  - destroys evaluated_rhs
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_BINDING_ERROR
static test_interpreter_ctx CTX_BINDING_NODE_BINDING_FAILS = {0};
static const test_interpreter_case BINDING_NODE_BINDING_FAILS = {
    .name = "eval_error_when_binding_node_and_binding_fails",

    .root_constructor_fn = &make_root_a_binding_node_with_rhs_number_42,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_SET_LOCAL_RET_FALSE,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_SET_LOCAL_BND_LHS_TO_NUM_42,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_BINDING_ERROR,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_BINDING_NODE_BINDING_FAILS,
};

// Given:
//  - env and out are valid
//  - root is a well-formed binding node with rhs 42
//  - symbol binding will succeed
// Expected:
//  - rhs is correctly evaluated to a RUNTIME_VALUE_NUMBER 42 evaluated_rhs
//  - calls runtime_env_set_local with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//    - value: evaluated_rhs
//  - root->children->children[0]->data->data.symbol_value is bound to evaluated_rhs
//  - *out == evaluated_rhs
//      && (*out)->type == RUNTIME_VALUE_NUMBER
//      && (*out)->refcount == 2
//      && (*out)->as.i == 42
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_BINDING_NODE_BINDING_SUCCEEDS = {0};
static const test_interpreter_case BINDING_NODE_BINDING_SUCCEEDS = {
    .name = "eval_success_when_binding_node_and_binding_succeeds",

    .root_constructor_fn = &make_root_a_binding_node_with_rhs_number_42,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_SET_LOCAL_RET_TRUE,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_SET_LOCAL_BND_LHS_TO_NUM_42,
    .expected_out_fn = &expected_out_points_on_bound_number_42,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_BINDING_NODE_BINDING_SUCCEEDS,
};



//-----------------------------------------------------------------------------
// EVALUATION OF AST OF TYPE AST_TYPE_EVAL
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
    interpreter_status interpreter_eval(
        struct interpreter_ctx *ctx,
        struct runtime_env *env,
        const struct ast *root,
        struct runtime_env_value **out );
other elements of the isolated unit:
  - root
  - out
  - ast
- from the runtime_env module:
  - runtime_env_make_string
  - runtime_env_make_number
  - runtime_env_make_string
  - runtime_env_value_retain
  - runtime_env_value_release
  - runtime_env_value_destroy
- from the ast module:
  - ast_create_string_node
  - ast_create_symbol_node
  - ast_create_int_node
  - ast_create_children_node_var
  - ast_destroy

doubles:
  - dummy:
    - env
  - spy:
    - runtime_env_get
  - fake:
    - functions of standard library which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// PARAMETRIC CASES
//-----------------------------------------------------------------------------


// Given:
//  - env and out are valid
//  - root is a ill-formed eval node because:
//    - its child is not an ast data wrapper for a symbol
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_EVAL_NODE_ILL_FORMED_CAUSE_CHILD_NOT_SYMBOL = {0};
static const test_interpreter_case EVAL_NODE_ILL_FORMED_CAUSE_CHILD_NOT_SYMBOL = {
    .name = "eval_error_invalid_ast_when_eval_node_ill_formed_cause_child_is_not_symbol",

    .root_constructor_fn = &make_root_a_ill_formed_eval_node_because_child_is_not_symbol,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_EVAL_NODE_ILL_FORMED_CAUSE_CHILD_NOT_SYMBOL,
};

// Given:
//  - env and out are valid
//  - root is a well-formed eval node
//  - child symbol of root is not a bound symbol
// Expected:
//  - calls runtime_env_get with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//  - runtime_env_get returns NULL
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_LOOKUP_FAILED
static test_interpreter_ctx CTX_EVAL_NODE_UNBOUND_SYMBOL = {0};
static const test_interpreter_case EVAL_NODE_UNBOUND_SYMBOL = {
    .name = "eval_error_when_eval_node_and_child_symbol_is_unbound",

    .root_constructor_fn = &make_root_an_eval_node,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NULL,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_LOOKUP_FAILED,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_EVAL_NODE_UNBOUND_SYMBOL,
};

// Given:
//  - env and out are valid
//  - root is a well-formed eval node
//  - child symbol of root is bound to a value such as:
//      - value->type == RUNTIME_VALUE_STRING
//      - value->refcount == 1
//      - value->as.s == "abc"
// Expected:
//  - calls runtime_env_get with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//  - (*out)->type == RUNTIME_VALUE_STRING
//    - (*out)->refcount == 2
//    - (*out)->as.s == "abc"
//  - environment is not modified
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_EVAL_NODE_SYMBOL_BOUND_TO_STRING = {0};
static const test_interpreter_case EVAL_NODE_SYMBOL_BOUND_TO_STRING = {
    .name = "eval_success_when_eval_node_and_child_symbol_is_bound_to_a_string",

    .root_constructor_fn = &make_root_an_eval_node,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_STR_ABC,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
    .expected_out_fn = &expected_out_points_on_bound_string_abc,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_EVAL_NODE_SYMBOL_BOUND_TO_STRING,
};

// Given:
//  - env and out are valid
//  - root is a well-formed eval node
//  - child symbol of root is bound to a RUNTIME_VALUE_STRING with A_CONSTANT_STRING
//  - oom
// Expected:
//  - calls runtime_env_get with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//  - environment is not modified
//    - (*out)->type == RUNTIME_VALUE_STRING
//    - (*out)->refcount == 2
//    - (*out)->as.s == A_CONSTANT_STRING
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_EVAL_NODE_SYMBOL_BOUND_TO_STRING_OOM = {0};
static const test_interpreter_case EVAL_NODE_SYMBOL_BOUND_TO_STRING_OOM = {
    .name = "eval_success_when_eval_node_and_child_symbol_is_bound_to_a_string_and_oom",

    .root_constructor_fn = &make_root_an_eval_node,
    .env_is_dummy = true,
    .oom = true,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_STR_ABC,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
    .expected_out_fn = &expected_out_points_on_bound_string_abc,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_runtime_value_at_out,

    .ctx =&CTX_EVAL_NODE_SYMBOL_BOUND_TO_STRING_OOM,
};

// Given:
//  - env and out are valid
//  - root is a well-formed eval node
//  - child symbol of root is bound to a RUNTIME_VALUE_QUOTED with an AST_TYPE_ADDITION 1+2
// Expected:
//  - calls runtime_env_get with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//  - environment is not modified
//    - (*out)->type == RUNTIME_VALUE_NUMBER
//    - (*out)->refcount == 1
//    - (*out)->as.i == 3
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_EVAL_NODE_SYMBOL_BOUND_TO_QUOTED_ADDITION = {0};
static const test_interpreter_case EVAL_NODE_SYMBOL_BOUND_TO_QUOTED_ADDITION = {
    .name = "eval_success_when_eval_node_and_child_symbol_is_bound_to_a_quoted_addition",

    .root_constructor_fn = &make_root_an_eval_node,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_QUOTED_ADD_1_PLUS_2,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
    .expected_out_fn = &expected_out_points_on_fresh_number_3,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_quoted_ast_in_get_ret_and_get_ret_and_runtime_value_at_out,
    .ctx =&CTX_EVAL_NODE_SYMBOL_BOUND_TO_QUOTED_ADDITION,
};

// Given:
//  - env and out are valid
//  - root is a well-formed eval node
//  - child symbol of root is bound to a RUNTIME_VALUE_QUOTED with an AST_TYPE_ADDITION 1+2
//  - oom
// Expected:
//  - calls runtime_env_get with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_EVAL_NODE_SYMBOL_BOUND_TO_QUOTED_ADDITION_OOM = {0};
static const test_interpreter_case EVAL_NODE_SYMBOL_BOUND_TO_QUOTED_ADDITION_OOM = {
    .name = "eval_error_when_eval_node_and_child_symbol_is_bound_to_a_quoted_addition_and_oom",

    .root_constructor_fn = &make_root_an_eval_node,
    .env_is_dummy = true,
    .oom = true,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_QUOTED_ADD_1_PLUS_2,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root_and_quoted_ast_in_get_ret_and_get_ret,

    .ctx =&CTX_EVAL_NODE_SYMBOL_BOUND_TO_QUOTED_ADDITION_OOM,
};



//-----------------------------------------------------------------------------
// EVALUATION OF AST OF TYPE AST_TYPE_WRITING
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
core of the isolated unit:
    interpreter_status interpreter_eval(
        struct interpreter_ctx *ctx,
        struct runtime_env *env,
        const struct ast *root,
        struct runtime_env_value **out );
other elements of the isolated unit:
  - root
  - out
  - ast
- from the runtime_env module:
  - runtime_env_make_string
  - runtime_env_make_number
  - runtime_env_make_string
  - runtime_env_value_retain
  - runtime_env_value_release
  - runtime_env_value_destroy
- from the ast module:
  - ast_create_string_node
  - ast_create_symbol_node
  - ast_create_int_node
  - ast_create_children_node_var
  - ast_destroy

doubles:
  - dummy:
    - env
  - spy:
    - runtime_env_get
  - fake:
    - functions of standard library which are used:
      - malloc, free, strdup
*/



//-----------------------------------------------------------------------------
// PARAMETRIC CASES
//-----------------------------------------------------------------------------


// Given:
//  - env and out are valid
//  - root is a ill-formed writing node because it has two children
// Expected:
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_INVALID_AST
static test_interpreter_ctx CTX_WRITING_NODE_ILL_FORMED_CAUSE_TWO_CHILDREN = {0};
static const test_interpreter_case WRITING_NODE_ILL_FORMED_CAUSE_TWO_CHILDREN = {
    .name = "eval_error_invalid_ast_when_writing_node_ill_formed_cause_two_children",

    .root_constructor_fn = &make_root_a_ill_formed_writing_node_because_two_children,
    .env_is_dummy = true,
    .oom = false,

    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_INVALID_AST,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_WRITING_NODE_ILL_FORMED_CAUSE_TWO_CHILDREN,
};

// Given:
//  - env and out are valid
//  - root is a well-formed writing node
//  - child of root is an ast data wrapper for a number 42
//  - call write_runtime_value_fn will fail
// Expected:
//  - calls write_runtime_value_fn with:
//    - ctx: arg_ctx
//    - value: <evaluated_child owned by implementation> such as:
//      - evaluated_child != NULL
//      - evaluated_child->type == RUNTIME_VALUE_NUMBER
//      - evaluated_child->refcount == 1
//      - evaluated_child->as.i == 42
//  - destroys evaluated_child
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_WRITE_RUNTIME_VALUE_ERROR
static test_interpreter_ctx CTX_WRITING_NODE_NUMBER_CALLBACK_WRITE_FAILS = {0};
static const test_interpreter_case WRITING_NODE_NUMBER_CALLBACK_WRITE_FAILS = {
    .name = "eval_error_when_writing_node_and_number_and_callback_write_fails",

    .root_constructor_fn = &make_root_a_writing_node_with_child_number_42,
    .env_is_dummy = true,
    .oom = false,
	.write_runtime_value_arrange_kind = WRITE_RUNTIME_VALUE_ARR_WRITE_FAIL,

	.write_runtime_value_assert_kind = WRITE_RUNTIME_VALUE_ASRT_WRITE_NUM_42,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_WRITE_RUNTIME_VALUE_ERROR,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_WRITING_NODE_NUMBER_CALLBACK_WRITE_FAILS,
};

// Given:
//  - env and out are valid
//  - root is a well-formed writing node
//  - child of root is an ast data wrapper for a number 42
//  - call write_runtime_value_fn will succeed
// Expected:
//  - calls write_runtime_value_fn with:
//    - ctx: arg_ctx
//    - value: <evaluated_child owned by implementation> such as:
//      - evaluated_child != NULL
//      - evaluated_child->type == RUNTIME_VALUE_NUMBER
//      - evaluated_child->refcount == 1
//      - evaluated_child->as.i == 42
//  - destroys evaluated_child
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_WRITING_NODE_NUMBER = {0};
static const test_interpreter_case WRITING_NODE_NUMBER = {
    .name = "eval_success_when_writing_node_and_number",

    .root_constructor_fn = &make_root_a_writing_node_with_child_number_42,
    .env_is_dummy = true,
    .oom = false,
	.write_runtime_value_arrange_kind = WRITE_RUNTIME_VALUE_ARR_WRITE_SUCCESS,

	.write_runtime_value_assert_kind = WRITE_RUNTIME_VALUE_ASRT_WRITE_NUM_42,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_WRITING_NODE_NUMBER,
};

// Given:
//  - env and out are valid
//  - root is a well-formed writing node
//  - child of root is an ast data wrapper for a number 42
//  - oom
// Expected:
//  - no write_runtime_value_fn call
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_WRITING_NODE_NUMBER_OOM = {0};
static const test_interpreter_case WRITING_NODE_NUMBER_OOM = {
    .name = "eval_error_when_writing_node_and_number_and_oom",

    .root_constructor_fn = &make_root_a_writing_node_with_child_number_42,
    .env_is_dummy = true,
    .oom = true,

	.write_runtime_value_assert_kind = WRITE_RUNTIME_VALUE_ASRT_NONE,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_WRITING_NODE_NUMBER_OOM,
};

// Given:
//  - env and out are valid
//  - root is a well-formed writing node
//  - child of root is an ast data wrapper for a string "abc"
//  - call write_runtime_value_fn will fail
// Expected:
//  - calls write_runtime_value_fn with:
//    - ctx: arg_ctx
//    - value: <evaluated_child owned by implementation> such as:
//      - evaluated_child != NULL
//      - evaluated_child->type == RUNTIME_VALUE_STRING
//      - evaluated_child->refcount == 1
//      - evaluated_child->as.s == "abc"
//  - destroys evaluated_child
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_WRITE_RUNTIME_VALUE_ERROR
static test_interpreter_ctx CTX_WRITING_NODE_STRING_CALLBACK_WRITE_FAILS = {0};
static const test_interpreter_case WRITING_NODE_STRING_CALLBACK_WRITE_FAILS = {
    .name = "eval_error_when_writing_node_and_string_abc_and_callback_write_fails",

    .root_constructor_fn = &make_root_a_writing_node_with_child_string_abc,
    .env_is_dummy = true,
    .oom = false,
	.write_runtime_value_arrange_kind = WRITE_RUNTIME_VALUE_ARR_WRITE_FAIL,

	.write_runtime_value_assert_kind = WRITE_RUNTIME_VALUE_ASRT_WRITE_STR_ABC,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_WRITE_RUNTIME_VALUE_ERROR,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_WRITING_NODE_STRING_CALLBACK_WRITE_FAILS,
};

// Given:
//  - env and out are valid
//  - root is a well-formed writing node
//  - child of root is an ast data wrapper for a string "abc"
//  - call write_runtime_value_fn will succeed
// Expected:
//  - calls write_runtime_value_fn with:
//    - ctx: arg_ctx
//    - value: <evaluated_child owned by implementation> such as:
//      - evaluated_child != NULL
//      - evaluated_child->type == RUNTIME_VALUE_STRING
//      - evaluated_child->refcount == 1
//      - evaluated_child->as.s == "abc"
//  - destroys evaluated_child
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_WRITING_NODE_STRING = {0};
static const test_interpreter_case WRITING_NODE_STRING = {
    .name = "eval_success_when_writing_node_and_string",

    .root_constructor_fn = &make_root_a_writing_node_with_child_string_abc,
    .env_is_dummy = true,
    .oom = false,
	.write_runtime_value_arrange_kind = WRITE_RUNTIME_VALUE_ARR_WRITE_SUCCESS,

	.write_runtime_value_assert_kind = WRITE_RUNTIME_VALUE_ASRT_WRITE_STR_ABC,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_WRITING_NODE_STRING,
};

// Given:
//  - env and out are valid
//  - root is a well-formed writing node
//  - child of root is an ast data wrapper for a string "abc"
//  - oom
// Expected:
//  - no write_runtime_value_fn call
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_WRITING_NODE_STRING_OOM = {0};
static const test_interpreter_case WRITING_NODE_STRING_OOM = {
    .name = "eval_error_when_writing_node_and_string_and_oom",

    .root_constructor_fn = &make_root_a_writing_node_with_child_string_abc,
    .env_is_dummy = true,
    .oom = true,

	.write_runtime_value_assert_kind = WRITE_RUNTIME_VALUE_ASRT_NONE,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_WRITING_NODE_STRING_OOM,
};

// Given:
//  - env and out are valid
//  - root is a well-formed writing node
//  - child of root is an ast data wrapper for an unbound symbol or
//    a one whose value can't be retrieved in environment (runtime_env_get fails)
// Expected:
//  - calls runtime_env_get with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//  - no write_runtime_value_fn call
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_LOOKUP_FAILED
static test_interpreter_ctx CTX_WRITING_NODE_SYMBOL_UNBOUND = {0};
static const test_interpreter_case WRITING_NODE_SYMBOL_UNBOUND = {
    .name = "eval_error_when_writing_node_and_symbol_unbound",

    .root_constructor_fn = &make_root_a_writing_node_with_a_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NULL,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
	.write_runtime_value_assert_kind = WRITE_RUNTIME_VALUE_ASRT_NONE,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_LOOKUP_FAILED,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_WRITING_NODE_SYMBOL_UNBOUND,
};

// Given:
//  - env and out are valid
//  - root is a well-formed writing node
//  - child of root is an ast data wrapper for a bound symbol
//    whose value can be retrieved in environment (runtime_env_get succeeds)
//  - the value bound to this symbol is a RUNTIME_VALUE_NUMBER 42
//  - call write_runtime_value_fn will fail
// Expected:
//  - calls runtime_env_get with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//  - calls write_runtime_value_fn with:
//    - ctx: arg_ctx
//    - value: <ret of runtime_env_get>
//      - value != NULL
//      - value->type == RUNTIME_VALUE_NUMBER
//      - value->refcount == 2
//      - value->as.i == 42
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_WRITE_RUNTIME_VALUE_ERROR
static test_interpreter_ctx CTX_WRITING_NODE_SYMBOL_CALLBACK_WRITE_FAILS = {0};
static const test_interpreter_case WRITING_NODE_SYMBOL_CALLBACK_WRITE_FAILS = {
    .name = "eval_error_when_writing_node_and_symbol_and_callback_write_fails",

    .root_constructor_fn = &make_root_a_writing_node_with_a_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NUM_42,
	.write_runtime_value_arrange_kind = WRITE_RUNTIME_VALUE_ARR_WRITE_FAIL,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
	.write_runtime_value_assert_kind = WRITE_RUNTIME_VALUE_ASRT_WRITE_NUM_42_RCNT_2,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_WRITE_RUNTIME_VALUE_ERROR,

    .clean_up_fn = &destroy_root_and_get_ret,

    .ctx =&CTX_WRITING_NODE_SYMBOL_CALLBACK_WRITE_FAILS,
};

// Given:
//  - env and out are valid
//  - root is a well-formed writing node
//  - child of root is an ast data wrapper for a bound symbol
//    whose value can be retrieved in environment (runtime_env_get succeeds)
//  - the value bound to this symbol is a RUNTIME_VALUE_NUMBER 42
//  - call write_runtime_value_fn will succeed
// Expected:
//  - calls runtime_env_get with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//  - calls write_runtime_value_fn with:
//    - ctx: arg_ctx
//    - value: <ret of runtime_env_get>
//      - value != NULL
//      - value->type == RUNTIME_VALUE_NUMBER
//      - value->refcount == 2
//      - value->as.i == 42
//  - <ret of runtime_env_get> is released after the callback call (its refcount is decremented) (not checked)
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_WRITING_NODE_SYMBOL = {0};
static const test_interpreter_case WRITING_NODE_SYMBOL = {
    .name = "eval_success_when_writing_node_and_symbol",

    .root_constructor_fn = &make_root_a_writing_node_with_a_symbol,
    .env_is_dummy = true,
    .oom = false,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NUM_42,
	.write_runtime_value_arrange_kind = WRITE_RUNTIME_VALUE_ARR_WRITE_SUCCESS,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
	.write_runtime_value_assert_kind = WRITE_RUNTIME_VALUE_ASRT_WRITE_NUM_42_RCNT_2,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_get_ret,

    .ctx =&CTX_WRITING_NODE_SYMBOL,
};

// Given:
//  - env and out are valid
//  - root is a well-formed writing node
//  - child of root is an ast data wrapper for a bound symbol
//    whose value can be retrieved in environment (runtime_env_get succeeds)
//  - the value bound to this symbol is a RUNTIME_VALUE_NUMBER 42
//  - call write_runtime_value_fn will succeed
//  - global OOM is enabled (all allocations fail), but this execution path does not allocate
// Expected:
//  - calls runtime_env_get with:
//    - e: env
//    - key: root->children->children[0]->data->data.symbol_value
//  - calls write_runtime_value_fn with:
//    - ctx: arg_ctx
//    - value: <ret of runtime_env_get>
//      - value != NULL
//      - value->type == RUNTIME_VALUE_NUMBER
//      - value->refcount == 2
//      - value->as.i == 42
//  - <ret of runtime_env_get> is released after the callback call (its refcount is decremented) (not checked)
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_WRITING_NODE_SYMBOL_OOM = {0};
static const test_interpreter_case WRITING_NODE_SYMBOL_OOM = {
    .name = "eval_success_when_writing_node_and_symbol_and_oom",

    .root_constructor_fn = &make_root_a_writing_node_with_a_symbol,
    .env_is_dummy = true,
    .oom = true,
	.runtime_env_arrange_kind = ENV_ARR_GET_RET_NUM_42,
	.write_runtime_value_arrange_kind = WRITE_RUNTIME_VALUE_ARR_WRITE_SUCCESS,

	.runtime_env_assert_kind = ENV_ASRT_EXPECT_GET_CALLED_ON_CHILD_SYMBOL,
	.write_runtime_value_assert_kind = WRITE_RUNTIME_VALUE_ASRT_WRITE_NUM_42_RCNT_2,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root_and_get_ret,

    .ctx =&CTX_WRITING_NODE_SYMBOL_OOM,
};

// Given:
//  - env and out are valid
//  - root is a well-formed writing node
//  - child of root is an ast of type AST_TYPE_ADDITION for "1+2"
//  - call write_runtime_value_fn will fail
// Expected:
//  - calls write_runtime_value_fn with:
//    - ctx: arg_ctx
//    - value: <evaluated_child owned by implementation> such as:
//      - evaluated_child != NULL
//      - evaluated_child->type == RUNTIME_VALUE_NUMBER
//      - evaluated_child->refcount == 1
//      - evaluated_child->as.i == 3
//  - destroys evaluated_child
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_WRITE_RUNTIME_VALUE_ERROR
static test_interpreter_ctx CTX_WRITING_NODE_ADDITION_CALLBACK_WRITE_FAILS = {0};
static const test_interpreter_case WRITING_NODE_ADDITION_CALLBACK_WRITE_FAILS = {
    .name = "eval_error_when_writing_node_and_addition_and_callback_write_fails",

    .root_constructor_fn = &make_root_a_writing_node_with_an_addition_1_plus_2,
    .env_is_dummy = true,
    .oom = false,
	.write_runtime_value_arrange_kind = WRITE_RUNTIME_VALUE_ARR_WRITE_FAIL,

	.write_runtime_value_assert_kind = WRITE_RUNTIME_VALUE_ASRT_WRITE_NUM_3,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_WRITE_RUNTIME_VALUE_ERROR,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_WRITING_NODE_ADDITION_CALLBACK_WRITE_FAILS,
};

// Given:
//  - env and out are valid
//  - root is a well-formed writing node
//  - child of root is an ast of type AST_TYPE_QUOTE whose child is an
//    AST_TYPE_ADDITION "1+2"
//  - call write_runtime_value_fn will fail
// Expected:
//  - calls write_runtime_value_fn with:
//    - ctx: arg_ctx
//    - value: <evaluated_child owned by implementation> such as:
//      - evaluated_child != NULL
//      - evaluated_child->type == RUNTIME_VALUE_QUOTED
//      - evaluated_child->refcount == 1
//      - evaluated_child->as.quoted == root->children->children[0]->children->children[0]
//  - destroys evaluated_child
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_WRITE_RUNTIME_VALUE_ERROR
static test_interpreter_ctx CTX_WRITING_NODE_QUOTED_ADDITION_CALLBACK_WRITE_FAILS = {0};
static const test_interpreter_case WRITING_NODE_QUOTED_ADDITION_CALLBACK_WRITE_FAILS = {
    .name = "eval_error_when_writing_node_and_quoted_addition_and_callback_write_fails",

    .root_constructor_fn = &make_root_a_writing_node_with_quoted_add_1_2,
    .env_is_dummy = true,
    .oom = false,
	.write_runtime_value_arrange_kind = WRITE_RUNTIME_VALUE_ARR_WRITE_FAIL,

	.write_runtime_value_assert_kind = WRITE_RUNTIME_VALUE_ASRT_WRITE_QUOTED_ADD_1_PLUS_2,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_WRITE_RUNTIME_VALUE_ERROR,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_WRITING_NODE_QUOTED_ADDITION_CALLBACK_WRITE_FAILS,
};

// Given:
//  - env and out are valid
//  - root is a well-formed writing node
//  - child of root is an ast of type AST_TYPE_QUOTE whose child is an
//    AST_TYPE_ADDITION "1+2"
//  - call write_runtime_value_fn will succeed
// Expected:
//  - calls write_runtime_value_fn with:
//    - ctx: arg_ctx
//    - value: <evaluated_child owned by implementation> such as:
//      - evaluated_child != NULL
//      - evaluated_child->type == RUNTIME_VALUE_QUOTED
//      - evaluated_child->refcount == 1
//      - evaluated_child->as.quoted == root->children->children[0]->children->children[0]
//  - destroys evaluated_child
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OK
static test_interpreter_ctx CTX_WRITING_NODE_QUOTED_ADDITION_CALLBACK_WRITE = {0};
static const test_interpreter_case WRITING_NODE_QUOTED_ADDITION_CALLBACK_WRITE = {
    .name = "eval_error_when_writing_node_and_quoted_addition_and_callback_write_succeeds",

    .root_constructor_fn = &make_root_a_writing_node_with_quoted_add_1_2,
    .env_is_dummy = true,
    .oom = false,
	.write_runtime_value_arrange_kind = WRITE_RUNTIME_VALUE_ARR_WRITE_SUCCESS,

	.write_runtime_value_assert_kind = WRITE_RUNTIME_VALUE_ASRT_WRITE_QUOTED_ADD_1_PLUS_2,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OK,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_WRITING_NODE_QUOTED_ADDITION_CALLBACK_WRITE,
};

// Given:
//  - env and out are valid
//  - root is a well-formed writing node
//  - child of root is an ast of type AST_TYPE_QUOTE whose child is an
//    AST_TYPE_ADDITION "1+2"
//  - oom
// Expected:
//  - no write_runtime_value_fn call
//  - environment is not modified
//  - *out remains unchanged
//  - returns INTERPRETER_STATUS_OOM
static test_interpreter_ctx CTX_WRITING_NODE_QUOTED_ADDITION_OOM = {0};
static const test_interpreter_case WRITING_NODE_QUOTED_ADDITION_OOM = {
    .name = "eval_error_when_writing_node_and_quoted_addition_and_oom",

    .root_constructor_fn = &make_root_a_writing_node_with_quoted_add_1_2,
    .env_is_dummy = true,
    .oom = true,

	.write_runtime_value_assert_kind = WRITE_RUNTIME_VALUE_ASRT_NONE,
    .expected_out_fn = &expected_out_unchanged,
    .expected_status = INTERPRETER_STATUS_OOM,

    .clean_up_fn = &destroy_root,

    .ctx =&CTX_WRITING_NODE_QUOTED_ADDITION_OOM,
};



//-----------------------------------------------------------------------------
// PARAMETRIC CASES REGISTRY
//-----------------------------------------------------------------------------
//
// Centralized registry of all parametric test cases for interpreter_eval.
// Each case is defined once here, then automatically expanded into a
// CMocka CMUnitTest array below.
//
// To add a new test case:
//     1. Define its `test_interpreter_case` struct (e.g. ADDITION_NODE_OOM)
//     2. Add one line in INTERPRETER_PARAM_CASES() below
//

#define INTERPRETER_PARAM_CASES(X) \
    X(INT_NODE_OOM) \
    X(INT_NODE_SUCCESS) \
    X(STRING_NODE_OOM) \
    X(STRING_NODE_SUCCESS) \
    X(ERROR_NOT_SENTINEL_NODE_OOM) \
    X(ERROR_NOT_SENTINEL_NODE_SUCCESS) \
    X(ERROR_SENTINEL_NODE_OOM) \
    X(ERROR_SENTINEL_NODE_SUCCESS) \
    X(SYMBOL_NODE_UNBOUND) \
    X(SYMBOL_NODE_BOUND) \
    X(FUNCTION_NODE_ILL_FORMED_CHILDREN_NULL) \
    X(FUNCTION_NODE_ILL_FORMED_NO_CHILD) \
    X(FUNCTION_NODE_ILL_FORMED_TWO_CHILDREN) \
    X(FUNCTION_NODE_DUPLICATE_PARAM) \
    X(FUNCTION_NODE_OOM) \
    X(FUNCTION_NODE_SUCCESS) \
    X(FUNCTION_DEFINITION_NODE_ILL_FORMED_CHILDREN_NULL) \
    X(FUNCTION_DEFINITION_NODE_ILL_FORMED_NO_CHILD) \
    X(FUNCTION_DEFINITION_NODE_ILL_FORMED_TWO_CHILDREN) \
    X(FUNCTION_DEFINITION_NODE_OOM) \
    X(FUNCTION_DEFINITION_NODE_BINDING_FAILURE) \
    X(FUNCTION_DEFINITION_NODE_SUCCESS) \
    X(NEGATION_NODE_ILL_FORMED_CHILDREN_NULL) \
    X(NEGATION_NODE_ILL_FORMED_NO_CHILD) \
    X(NEGATION_NODE_ILL_FORMED_TWO_CHILDREN) \
    X(NEGATION_NODE_WITH_SYMBOL_AND_UNBOUND) \
    X(NEGATION_NODE_WITH_SYMBOL_AND_UNBOUND_AND_OOM) \
    X(NEGATION_NODE_WITH_SYMBOL_AND_BOUND_NOT_TO_A_NUMBER) \
    X(NEGATION_NODE_WITH_SYMBOL_AND_BOUND_NOT_TO_A_NUMBER_AND_OOM) \
    X(NEGATION_NODE_WITH_SYMBOL_AND_BOUND_TO_A_NUMBER_AND_OOM) \
    X(NEGATION_NODE_WITH_SYMBOL_AND_BOUND_TO_A_NUMBER) \
    X(NEGATION_NODE_WITH_NUMBER_AND_OOM) \
    X(NEGATION_NODE_WITH_NUMBER_SUCCESS) \
    X(ADDITION_NODE_ILL_FORMED_CHILDREN_NULL) \
    X(ADDITION_NODE_ILL_FORMED_NO_CHILD) \
    X(ADDITION_NODE_ILL_FORMED_ONE_CHILD) \
    X(ADDITION_NODE_ILL_FORMED_THREE_CHILDREN) \
    X(ADDITION_NODE_RHS_SYMBOL_UNBOUND) \
    X(ADDITION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM) \
    X(ADDITION_NODE_RHS_NOT_NUMBER) \
    X(ADDITION_NODE_RHS_NOT_NUMBER_AND_OOM) \
    X(ADDITION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER) \
    X(ADDITION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_AND_OOM) \
    X(ADDITION_NODE_LHS_NUMBER_RHS_NUMBER_AND_OOM) \
    X(ADDITION_NODE_LHS_NUMBER_RHS_NUMBER) \
    X(SUBTRACTION_NODE_ILL_FORMED_CHILDREN_NULL) \
    X(SUBTRACTION_NODE_ILL_FORMED_NO_CHILD) \
    X(SUBTRACTION_NODE_ILL_FORMED_ONE_CHILD) \
    X(SUBTRACTION_NODE_ILL_FORMED_THREE_CHILDREN) \
    X(SUBTRACTION_NODE_RHS_SYMBOL_UNBOUND) \
    X(SUBTRACTION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM) \
    X(SUBTRACTION_RHS_NOT_NUMBER) \
    X(SUBTRACTION_NODE_RHS_NOT_NUMBER_AND_OOM) \
    X(SUBTRACTION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER) \
    X(SUBTRACTION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_AND_OOM) \
    X(SUBTRACTION_NODE_LHS_NUMBER_RHS_NUMBER_AND_OOM) \
    X(SUBTRACTION_NODE_LHS_NUMBER_RHS_NUMBER) \
    X(MULTIPLICATION_NODE_ILL_FORMED_CHILDREN_NULL) \
    X(MULTIPLICATION_NODE_ILL_FORMED_NO_CHILD) \
    X(MULTIPLICATION_NODE_ILL_FORMED_ONE_CHILD) \
    X(MULTIPLICATION_NODE_ILL_FORMED_THREE_CHILDREN) \
    X(MULTIPLICATION_NODE_RHS_SYMBOL_UNBOUND) \
    X(MULTIPLICATION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM) \
    X(MULTIPLICATION_NODE_RHS_NOT_NUMBER) \
    X(MULTIPLICATION_NODE_RHS_NOT_NUMBER_AND_OOM) \
    X(MULTIPLICATION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER) \
    X(MULTIPLICATION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_AND_OOM) \
    X(MULTIPLICATION_NODE_LHS_NUMBER_RHS_NUMBER_AND_OOM) \
    X(MULTIPLICATION_NODE_LHS_NUMBER_RHS_NUMBER) \
    X(DIVISION_NODE_ILL_FORMED_CHILDREN_NULL) \
    X(DIVISION_NODE_ILL_FORMED_NO_CHILD) \
    X(DIVISION_NODE_ILL_FORMED_ONE_CHILD) \
    X(DIVISION_NODE_ILL_FORMED_THREE_CHILDREN) \
    X(DIVISION_NODE_RHS_SYMBOL_UNBOUND) \
    X(DIVISION_NODE_RHS_SYMBOL_UNBOUND_AND_OOM) \
    X(DIVISION_NODE_RHS_NOT_NUMBER) \
    X(DIVISION_NODE_RHS_NOT_NUMBER_AND_OOM) \
    X(DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_ZERO) \
    X(DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_ZERO_AND_OOM) \
    X(DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_NOT_ZERO) \
    X(DIVISION_NODE_LHS_NUMBER_RHS_SYMBOL_BOUND_TO_NUMBER_NOT_ZERO_AND_OOM) \
    X(DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_ZERO_AND_OOM) \
    X(DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_ZERO) \
    X(DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_NOT_ZERO_AND_OOM) \
    X(DIVISION_NODE_LHS_NUMBER_RHS_NUMBER_NOT_ZERO) \
    X(QUOTE_NODE_ILL_FORMED_CHILDREN_NULL) \
    X(QUOTE_NODE_OOM) \
    X(QUOTE_NODE) \
    X(READING_NODE_ILL_FORMED_NO_CHILD) \
    X(READING_NODE_ILL_FORMED_THREE_CHILDREN) \
    X(READING_NODE_OOM) \
    X(READING_NODE_CALLBACK_READ_AST_FAILS) \
    X(READING_NODE_BINDING_FAILS) \
    X(READING_NODE_SUCCESS) \
    X(BINDING_NODE_ILL_FORMED_NO_CHILD) \
    X(BINDING_NODE_OOM) \
    X(BINDING_NODE_BINDING_FAILS) \
    X(BINDING_NODE_BINDING_SUCCEEDS) \
    X(EVAL_NODE_ILL_FORMED_CAUSE_CHILD_NOT_SYMBOL) \
    X(EVAL_NODE_UNBOUND_SYMBOL) \
    X(EVAL_NODE_SYMBOL_BOUND_TO_STRING) \
    X(EVAL_NODE_SYMBOL_BOUND_TO_STRING_OOM) \
    X(EVAL_NODE_SYMBOL_BOUND_TO_QUOTED_ADDITION) \
    X(EVAL_NODE_SYMBOL_BOUND_TO_QUOTED_ADDITION_OOM) \
    X(WRITING_NODE_ILL_FORMED_CAUSE_TWO_CHILDREN) \
	X(WRITING_NODE_NUMBER_CALLBACK_WRITE_FAILS) \
	X(WRITING_NODE_NUMBER) \
	X(WRITING_NODE_NUMBER_OOM) \
	X(WRITING_NODE_STRING_CALLBACK_WRITE_FAILS) \
	X(WRITING_NODE_STRING) \
	X(WRITING_NODE_STRING_OOM) \
	X(WRITING_NODE_SYMBOL_UNBOUND) \
	X(WRITING_NODE_SYMBOL_CALLBACK_WRITE_FAILS) \
	X(WRITING_NODE_SYMBOL) \
	X(WRITING_NODE_SYMBOL_OOM) \
	X(WRITING_NODE_ADDITION_CALLBACK_WRITE_FAILS) \
	X(WRITING_NODE_QUOTED_ADDITION_CALLBACK_WRITE_FAILS) \
	X(WRITING_NODE_QUOTED_ADDITION_CALLBACK_WRITE) \
	X(WRITING_NODE_QUOTED_ADDITION_OOM) \

#define MAKE_TEST(CASE_SYM) \
    { .name = CASE_SYM.name, \
    .test_func = eval_test, \
    .setup_func = parametric_setup, \
    .teardown_func = parametric_teardown, \
    .initial_state = (void*)&CASE_SYM },

static const struct CMUnitTest parametric_tests[] = {
    INTERPRETER_PARAM_CASES(MAKE_TEST)
};

#undef MAKE_TEST



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
    const struct CMUnitTest invalid_args_tests[] = {
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
    };

    int failed = 0;
    failed += cmocka_run_group_tests(invalid_args_tests, NULL, NULL);
    failed += cmocka_run_group_tests(parametric_tests, NULL, NULL);

    return failed;
}
