// src/runtime_env/tests/test_runtime_env.c

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "internal/runtime_env_internal.h"
#include "internal/runtime_env_ctx.h"
#include "fake_memory.h"
#include "memory_allocator.h"
#include "string_utils.h"
#include "internal/hashtable_test_utils.h"



//-----------------------------------------------------------------------------
// GLOBALS NOT DOUBLES, MAGIC NUMBER KILLERS
//-----------------------------------------------------------------------------


static const int A_INT = 7;
static const char *A_NON_NULL_STRING = "a non null string";
static const int AN_ERROR_CODE = 2;
static const char *AN_ERROR_MESSAGE = "an error message";
static bool A_BOOLEAN_VALUE = true;



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


// mocks
void mock_hashtable_destroy(hashtable *ht) {
    check_expected(ht);
}
hashtable *mock_hashtable_create(
        size_t size,
        hashtable_key_type key_type,
        hashtable_destroy_value_fn_t destroy_value_fn ) {
    check_expected(size);
    check_expected(key_type);
    check_expected(destroy_value_fn);
    return mock_type(hashtable *);
}

// stubs


// dummies
static const max_align_t DUMMY_SYMBOL;
static const struct symbol *const DUMMY_SYMBOL_P = (const struct symbol *)&DUMMY_SYMBOL;
static const max_align_t DUMMY_FUNCTION_NODE;
static const struct ast *const DUMMY_FUNCTION_NODE_P = (const struct ast *)&DUMMY_FUNCTION_NODE;
static runtime_env DUMMY_CLOSURE = {.bindings = NULL, .refcount = 2, .is_root = false, .parent = NULL};
static runtime_env *const DUMMY_CLOSURE_P = &DUMMY_CLOSURE;
static runtime_env *const DUMMY_RUNTIME_ENV_P = &DUMMY_CLOSURE;
static max_align_t DUMMY_HASHTABLE;
static struct hashtable *DUMMY_HASHTABLE_P = (struct hashtable *)&DUMMY_HASHTABLE;

// fakes
#define FAKABLE_MALLOC(n) (get_current_malloc()(n))
#define FAKABLE_FREE(p) (get_current_free()(p))
#define FAKABLE_STRDUP(s) (get_current_string_duplicate()(s))



//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// TESTS runtime_env_value *runtime_env_make_number(int i);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// runtime_env_make_number

// fake:
//  - functions of standard libray which are used:
//    - malloc, free



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int make_number_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    fake_memory_reset();

    return 0;
}

static int make_number_teardown(void **state) {
    (void)state;
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
//  - i == A_INT
// Expected:
//  - no invalid free
//  - no double free
//  - no memory leak


// Given:
//  - first allocation will fail
// Expected:
//  - ret == NULL
static void make_number_returns_null_when_first_allocation_fails(void **state) {
    (void)state;
    fake_memory_fail_only_on_call(1);

    assert_null(runtime_env_make_number(A_INT));
}

// Given:
//  - first allocation will succeed
// Expected:
//  - ret != NULL
//  - ret->type == RUNTIME_VALUE_NUMBER
//  - ret->as.i == A_INT
static void make_number_success_when_first_allocation_succeeds(void **state) {
    (void)state;

    runtime_env_value *ret = runtime_env_make_number(A_INT);

    assert_non_null(ret);
    assert_int_equal(ret->type, RUNTIME_VALUE_NUMBER);
    assert_int_equal(ret->as.i, A_INT);

    fake_free(ret);
}



//-----------------------------------------------------------------------------
// TESTS runtime_env_value *runtime_env_make_string(const char *s);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// runtime_env_make_string

// fake:
//  - functions of standard libray which are used:
//    - malloc, free, strdup



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int make_string_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    return 0;
}

static int make_string_teardown(void **state) {
    (void)state;
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


// At every test:
// Expected:
//  - no invalid free
//  - no double free
//  - no memory leak


// Given:
//  - s == NULL
// Expected:
//  - ret == NULL
static void make_string_returns_null_when_s_null(void **state) {
    (void)state;
    assert_null(runtime_env_make_string(NULL));

    fake_memory_fail_only_on_call(1);
    assert_null(runtime_env_make_string(NULL));

    fake_memory_fail_only_on_call(2);
    assert_null(runtime_env_make_string(NULL));
}

// Given:
//  - s != NULL
//  - first allocation will fail
// Expected:
//  - ret == NULL
static void make_string_returns_null_when_first_allocation_fails(void **state) {
    (void)state;
    fake_memory_fail_only_on_call(1);

    assert_null(runtime_env_make_string(A_NON_NULL_STRING));
}

// Given:
//  - s != NULL
//  - the failing allocation indexes: {2}
//    i.e.:
//      - allocation for runtime_env_value will succeed
//      - allocation for string duplication will fail
// Expected:
//  - ret == NULL
static void make_string_returns_null_when_second_allocation_fails(void **state) {
    (void)state;
    fake_memory_fail_only_on_call(2);

    assert_null(runtime_env_make_string(A_NON_NULL_STRING));
}

// Given:
//  - s != NULL
//  - all allocations will succeed
//    i.e.:
//      - allocation for runtime_env_value will succeed
//      - allocation for string duplication will succeed
// Expected:
//  - ret != NULL
//  - ret->type == RUNTIME_VALUE_STRING
//  - ret->as.s is a copy of s
static void make_string_success_when_two_allocations_succeed_and_s_non_null(void **state) {
    (void)state;

    runtime_env_value *ret = runtime_env_make_string(A_NON_NULL_STRING);

    assert_non_null(ret);
    assert_int_equal(ret->type, RUNTIME_VALUE_STRING);
    assert_string_equal(ret->as.s, A_NON_NULL_STRING);

    fake_free(ret->as.s);
    fake_free(ret);
}



//-----------------------------------------------------------------------------
// TESTS runtime_env_value *runtime_env_make_symbol(const struct symbol *sym);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// runtime_env_make_symbol

// dummy:
//  - sym
// fake:
//  - functions of standard libray which are used:
//    - malloc, free



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int make_symbol_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    fake_memory_reset();

    return 0;
}

static int make_symbol_teardown(void **state) {
    (void)state;
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
//  - sym == DUMMY_SYMBOL_P
// Expected:
//  - no invalid free
//  - no double free
//  - no memory leak


// Given:
//  - first allocation will fail
// Expected:
//  - ret == NULL
static void make_symbol_returns_null_when_first_allocation_fails(void **state) {
    (void)state;
    fake_memory_fail_only_on_call(1);

    assert_null(runtime_env_make_symbol(DUMMY_SYMBOL_P));
}

// Given:
//  - first allocation will succeed
// Expected:
//  - ret != NULL
//  - ret->type == RUNTIME_VALUE_SYMBOL
//  - ret->as.sym == DUMMY_SYMBOL_P
static void make_symbol_success_when_first_allocation_succeeds(void **state) {
    (void)state;

    runtime_env_value *ret = runtime_env_make_symbol(DUMMY_SYMBOL_P);

    assert_non_null(ret);
    assert_int_equal(ret->type, RUNTIME_VALUE_SYMBOL);
    assert_ptr_equal(ret->as.sym, DUMMY_SYMBOL_P);

    fake_free(ret);
}



//-----------------------------------------------------------------------------
// TESTS runtime_env_value *runtime_env_make_error(int code, const char *msg);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// runtime_env_make_error

// fake:
//  - functions of standard libray which are used:
//    - malloc, free, strdup



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int make_error_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    return 0;
}

static int make_error_teardown(void **state) {
    (void)state;
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


// At every test:
//  - code == AN_ERROR_CODE
// Expected:
//  - no invalid free
//  - no double free
//  - no memory leak


// Given:
//  - msg == NULL
// Expected:
//  - ret == NULL
static void make_error_returns_null_when_msg_null(void **state) {
    (void)state;
    assert_null(runtime_env_make_error(AN_ERROR_CODE, NULL));

    fake_memory_fail_only_on_call(1);
    assert_null(runtime_env_make_error(AN_ERROR_CODE, NULL));

    fake_memory_fail_only_on_call(2);
    assert_null(runtime_env_make_error(AN_ERROR_CODE, NULL));
}

// Given:
//  - msg != NULL
//  - first allocation will fail
// Expected:
//  - ret == NULL
static void make_error_returns_null_when_first_allocation_fails(void **state) {
    (void)state;
    fake_memory_fail_only_on_call(1);

    assert_null(runtime_env_make_error(AN_ERROR_CODE, AN_ERROR_MESSAGE));
}

// Given:
//  - msg != NULL
//  - the failing allocation indexes: {2}
//    i.e.:
//      - allocation for runtime_env_value will succeed
//      - allocation for string duplication will fail
// Expected:
//  - ret == NULL
static void make_error_returns_null_when_second_allocation_fails(void **state) {
    (void)state;
    fake_memory_fail_only_on_call(2);

    assert_null(runtime_env_make_error(AN_ERROR_CODE, AN_ERROR_MESSAGE));
}

// Given:
//  - msg != NULL
//  - all allocations will succeed
//    i.e.:
//      - allocation for runtime_env_value will succeed
//      - allocation for string duplication will succeed
// Expected:
//  - ret != NULL
//  - ret->type == RUNTIME_VALUE_ERROR
//  - ret->as.err.code == code
//  - ret->as.err.msg is a copy of msg
static void make_error_success_when_two_allocations_succeed_and_msg_non_null(void **state) {
    (void)state;

    runtime_env_value *ret = runtime_env_make_error(AN_ERROR_CODE, AN_ERROR_MESSAGE);

    assert_non_null(ret);
    assert_int_equal(ret->type, RUNTIME_VALUE_ERROR);
    assert_int_equal(ret->as.err.code, AN_ERROR_CODE);
    assert_string_equal(ret->as.err.msg, AN_ERROR_MESSAGE);

    fake_free(ret->as.err.msg);
    fake_free(ret);
}



//-----------------------------------------------------------------------------
// TESTS runtime_env_value *runtime_env_make_function(const struct ast *function_node, runtime_env *closure);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// runtime_env_make_function

// dummy:
//  - function_node
//  - closure
// fake:
//  - functions of standard libray which are used:
//    - malloc, free



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int make_function_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    fake_memory_reset();

    return 0;
}

static int make_function_teardown(void **state) {
    (void)state;
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
//  - function_node == DUMMY_FUNCTION_NODE_P
//  - closure == DUMMY_CLOSURE_P
// Expected:
//  - no invalid free
//  - no double free
//  - no memory leak


// Given:
//  - first allocation will fail
// Expected:
//  - ret == NULL
static void make_function_returns_null_when_first_allocation_fails(void **state) {
    (void)state;
    fake_memory_fail_only_on_call(1);

    assert_null(runtime_env_make_function(DUMMY_FUNCTION_NODE_P, DUMMY_CLOSURE_P));
}

// Given:
//  - first allocation will succeed
// Expected:
//  - ret != NULL
//  - ret->type == RUNTIME_VALUE_FUNCTION
//  - ret->as.fn.function_node == function_node
//  - ret->as.fn.closure == closure
static void make_function_success_when_first_allocation_succeeds(void **state) {
    (void)state;

    runtime_env_value *ret = runtime_env_make_function(DUMMY_FUNCTION_NODE_P, DUMMY_CLOSURE_P);

    assert_non_null(ret);
    assert_int_equal(ret->type, RUNTIME_VALUE_FUNCTION);
    assert_ptr_equal(ret->as.fn.function_node, DUMMY_FUNCTION_NODE_P);
    assert_ptr_equal(ret->as.fn.closure, DUMMY_CLOSURE_P);

    fake_free(ret);
}



//-----------------------------------------------------------------------------
// TESTS runtime_env *runtime_env_unwind(runtime_env *e);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// runtime_env_unwind

// dummy:
//  - e->bindings
//  - e->parent
// mock:
//  - functions of the hashtable module which are used:
//    - hashtable_destroy
// fake:
//  - functions of standard libray which are used:
//    - malloc, free



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int unwind_setup(void **state) {
    (void)state;

	// mock
    set_hashtable_destroy(mock_hashtable_destroy);

    // fake
    set_allocators(fake_malloc, fake_free);
    fake_memory_reset();

    return 0;
}

static int unwind_teardown(void **state) {
    (void)state;
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
    set_hashtable_destroy(NULL);
    set_allocators(NULL, NULL);
    fake_memory_reset();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// At every test:
// Expected:
//  - no invalid free
//  - no double free
//  - no memory leak


// Given:
//  - e == NULL
// Expected:
//  - returns NULL
static void unwind_returns_null_when_parent_null(void **state) {
    (void)state;

    assert_null(runtime_env_unwind(NULL));
}

// Given:
//  - e != NULL
// Expected:
//  - calls hashtable_destroy with:
//    - ht: e->bindings
//  - frees e
//  - returns e->parent
static void unwind_success_when_parent_not_null(void **state) {
    (void)state;
	runtime_env *fake_runtime_env_p = fake_malloc(sizeof(runtime_env));
	fake_runtime_env_p->bindings = DUMMY_HASHTABLE_P;
	fake_runtime_env_p->refcount = A_INT;
	fake_runtime_env_p->is_root  = A_BOOLEAN_VALUE;
	fake_runtime_env_p->parent = DUMMY_RUNTIME_ENV_P;
    expect_value(mock_hashtable_destroy, ht, fake_runtime_env_p->bindings);

    assert_ptr_equal(runtime_env_unwind(fake_runtime_env_p), DUMMY_RUNTIME_ENV_P);
}



//-----------------------------------------------------------------------------
// TESTS void runtime_env_value_destroy(runtime_env_value *value);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// runtime_env_value_destroy
// owned or retained/released resources of value
// runtime_env_make_number
// runtime_env_make_string
// runtime_env_make_symbol
// runtime_env_make_error
// runtime_env_make_function
// runtime_env_release
// runtime_env_unwind

// dummy
//  - borrowed resources of value (those of type ast* or symbol*):
//    - value->as.sym
//    - value->as.fn.function_node
//  - observed-only:
//    - value->as.fn.closure.parent
//  - sentinel passed to the mock (owned by closure, not freed here):
//    - value->as.fn.closure.bindings
// mock:
//  - functions of the hashtable module which are used:
//    - hashtable_destroy
// fake:
//  - functions of standard libray which are used:
//    - malloc, free



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int value_destroy_setup(void **state) {
    (void)state;

	// mock
    runtime_env_set_destroy_bindings(mock_hashtable_destroy);

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    return 0;
}

static int value_destroy_teardown(void **state) {
    (void)state;
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
    runtime_env_set_destroy_bindings(NULL);
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
    fake_memory_reset();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// At every test:
// Expected:
//  - no invalid free
//  - no double free
//  - no memory leak


// Given:
//  - value == NULL
static void value_destroy_do_nothing_when_value_null(void **state) {
    (void)state;
    runtime_env_value_destroy(NULL);
}

// Given:
//  - value->type == RUNTIME_VALUE_NUMBER
static void value_destroy_success_when_type_number(void **state) {
    (void)state;
    runtime_env_value *value = runtime_env_make_number(A_INT);

    runtime_env_value_destroy(value);
}

// Given:
//  - value->type == RUNTIME_VALUE_STRING
static void value_destroy_success_when_type_string(void **state) {
    (void)state;
    runtime_env_value *value = runtime_env_make_string(A_NON_NULL_STRING);

    runtime_env_value_destroy(value);
}

// Given:
//  - value->type == RUNTIME_VALUE_SYMBOL
// Expected:
//  - value->as.sym is not freed
static void value_destroy_success_when_type_symbol(void **state) {
    (void)state;
    runtime_env_value *value = runtime_env_make_symbol(DUMMY_SYMBOL_P);

    runtime_env_value_destroy(value);
}

// Given:
//  - value->type == RUNTIME_VALUE_ERROR
static void value_destroy_success_when_type_error(void **state) {
    (void)state;
    runtime_env_value *value = runtime_env_make_error(AN_ERROR_CODE, AN_ERROR_MESSAGE);

    runtime_env_value_destroy(value);
}

// Given:
//  - value->type == RUNTIME_VALUE_FUNCTION
//  - value->as.fn.closure->refcount == 2
//  - value->as.fn.closure->is_root == false
// WHEN DENOTING:
//  - closure = value->as.fn.closure
//  - function_node = value->as.fn.function_node
// Expected:
//  - function_node is not freed
//  - closure is not freed
//  - closure->refcount == 1
//  - value is freed
static void value_destroy_success_when_type_function_and_closure_refcount_2_and_not_root(void **state) {
    (void)state;
	runtime_env *fake_closure = fake_malloc(sizeof(runtime_env));
	fake_closure->bindings = DUMMY_HASHTABLE_P;
	fake_closure->refcount = 2;
	fake_closure->is_root = false;
	fake_closure->parent = DUMMY_RUNTIME_ENV_P;
	runtime_env_value *fake_runtime_value_function = runtime_env_make_function(DUMMY_FUNCTION_NODE_P, fake_closure);

	runtime_env_value_destroy(fake_runtime_value_function);

	assert_int_equal(fake_closure->refcount, 1);

	fake_free(fake_closure);
}

// Given:
//  - value->type == RUNTIME_VALUE_FUNCTION
//  - value->as.fn.closure->refcount == 1
//  - value->as.fn.closure->is_root == false
// WHEN DENOTING:
//  - closure = value->as.fn.closure
//  - function_node = value->as.fn.function_node
// Expected:
//  - function_node is not freed
//  - closure is freed
//  - value is freed
static void value_destroy_success_when_type_function_and_closure_refcount_1_and_not_root(void **state) {
    (void)state;
	runtime_env *fake_closure = fake_malloc(sizeof(runtime_env));
	fake_closure->bindings = DUMMY_HASHTABLE_P;
	fake_closure->refcount = 1;
	fake_closure->is_root = false;
	fake_closure->parent = DUMMY_RUNTIME_ENV_P;
	runtime_env_value *fake_runtime_value_function = runtime_env_make_function(DUMMY_FUNCTION_NODE_P, fake_closure);
    expect_value(mock_hashtable_destroy, ht, DUMMY_HASHTABLE_P);

	runtime_env_value_destroy(fake_runtime_value_function);
}

// Given:
//  - value->type == RUNTIME_VALUE_FUNCTION
//  - value->as.fn.closure->refcount == 1
//  - value->as.fn.closure->is_root == true
// WHEN DENOTING:
//  - closure = value->as.fn.closure
//  - function_node = value->as.fn.function_node
// Expected:
//  - function_node is not freed
//  - closure is not freed
//  - closure->refcount == 1
//  - value is freed
static void value_destroy_success_when_type_function_and_closure_refcount_1_and_root(void **state) {
    (void)state;
	runtime_env *fake_closure = fake_malloc(sizeof(runtime_env));
	fake_closure->bindings = DUMMY_HASHTABLE_P;
	fake_closure->refcount = 1;
	fake_closure->is_root = true;
	fake_closure->parent = DUMMY_RUNTIME_ENV_P;
	runtime_env_value *fake_runtime_value_function = runtime_env_make_function(DUMMY_FUNCTION_NODE_P, fake_closure);

	runtime_env_value_destroy(fake_runtime_value_function);

	assert_int_equal(fake_closure->refcount, 1);

	fake_free(fake_closure);
}



//-----------------------------------------------------------------------------
// TESTS runtime_env *runtime_env_wind(runtime_env *parent);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// runtime_env_wind

// dummy:
//  - parent
//  - ret->bindings
// mock:
//  - functions of the hashtable module which are used:
//    - hashtable_create
// fake:
//  - functions of standard libray which are used:
//    - malloc, free



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int wind_setup(void **state) {
    (void)state;

	// mock
    set_hashtable_create(mock_hashtable_create);

    // fake
    set_allocators(fake_malloc, fake_free);
    fake_memory_reset();

    return 0;
}

static int wind_teardown(void **state) {
    (void)state;
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
    set_hashtable_create(NULL);
    set_allocators(NULL, NULL);
    fake_memory_reset();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// At every test:
// Expected:
//  - no invalid free
//  - no double free
//  - no memory leak


// Given:
//  - parent == NULL
//  - first allocation will fail
// Expected:
//  - ret == NULL
static void wind_returns_null_when_parent_null_and_first_allocation_fails(void **state) {
    (void)state;
    fake_memory_fail_only_on_call(1);

    assert_null(runtime_env_wind(NULL));
}

// Given:
//  - parent == NULL
//  - first allocation will succeed
//  - hashtable_create will fail
// Expected:
//  - calls hashtable_create with:
//    - size: RUNTIME_ENV_SIZE
//    - key_type: RUNTIME_ENV_KEY_TYPE
//    - destroy_value_fn: runtime_env_value_destroy_adapter
//  - ret == NULL
static void wind_returns_null_when_parent_null_and_hashtable_create_fails(void **state) {
    (void)state;
    expect_value(mock_hashtable_create, size, RUNTIME_ENV_SIZE);
    expect_value(mock_hashtable_create, key_type, RUNTIME_ENV_KEY_TYPE);
    expect_value(mock_hashtable_create, destroy_value_fn, runtime_env_value_destroy_adapter);
    will_return(mock_hashtable_create, NULL);

    assert_null(runtime_env_wind(NULL));
}

// Given:
//  - parent == NULL
//  - first allocation will succeed
//  - hashtable_create will succeed
// Expected:
//  - calls hashtable_create with:
//    - size: RUNTIME_ENV_SIZE
//    - key_type: RUNTIME_ENV_KEY_TYPE
//    - destroy_value_fn: runtime_env_value_destroy_adapter
//  - ret != NULL
//  - ret->bindings == <hashtable_create returned value>
//  - ret->refcount == 1
//  - ret->is_root = true
//  - ret->parent == NULL
static void wind_success_when_parent_null_and_hashtable_create_succeeds(void **state) {
    (void)state;
    expect_value(mock_hashtable_create, size, RUNTIME_ENV_SIZE);
    expect_value(mock_hashtable_create, key_type, RUNTIME_ENV_KEY_TYPE);
    expect_value(mock_hashtable_create, destroy_value_fn, runtime_env_value_destroy_adapter);
    will_return(mock_hashtable_create, DUMMY_HASHTABLE_P);

    runtime_env *ret = runtime_env_wind(NULL);

    assert_non_null(ret);
    assert_ptr_equal(ret->bindings, DUMMY_HASHTABLE_P);
    assert_int_equal(ret->refcount, 1);
    assert_true(ret->is_root);
    assert_ptr_equal(ret->parent, NULL);

    fake_free(ret);
}

// Given:
//  - parent != NULL
//  - first allocation will fail
// Expected:
//  - ret == NULL
static void wind_returns_null_when_parent_not_null_and_first_allocation_fails(void **state) {
    (void)state;
    fake_memory_fail_only_on_call(1);

    assert_null(runtime_env_wind(DUMMY_CLOSURE_P));
}

// Given:
//  - parent != NULL
//  - first allocation will succeed
//  - hashtable_create will fail
// Expected:
//  - calls hashtable_create with:
//    - size: RUNTIME_ENV_SIZE
//    - key_type: RUNTIME_ENV_KEY_TYPE
//    - destroy_value_fn: runtime_env_value_destroy_adapter
//  - ret == NULL
static void wind_returns_null_when_parent_not_null_and_hashtable_create_fails(void **state) {
    (void)state;
    expect_value(mock_hashtable_create, size, RUNTIME_ENV_SIZE);
    expect_value(mock_hashtable_create, key_type, RUNTIME_ENV_KEY_TYPE);
    expect_value(mock_hashtable_create, destroy_value_fn, runtime_env_value_destroy_adapter);
    will_return(mock_hashtable_create, NULL);

    assert_null(runtime_env_wind(DUMMY_CLOSURE_P));
}

// Given:
//  - parent != NULL
//  - first allocation will succeed
//  - hashtable_create will succeed
// Expected:
//  - calls hashtable_create with:
//    - size: RUNTIME_ENV_SIZE
//    - key_type: RUNTIME_ENV_KEY_TYPE
//    - destroy_value_fn: runtime_env_value_destroy_adapter
//  - ret != NULL
//  - ret->bindings == <hashtable_create returned value>
//  - ret->refcount == 1
//  - ret->is_root = false
//  - ret->parent == parent
static void wind_success_when_parent_not_null_and_hashtable_create_succeeds(void **state) {
    (void)state;
    expect_value(mock_hashtable_create, size, RUNTIME_ENV_SIZE);
    expect_value(mock_hashtable_create, key_type, RUNTIME_ENV_KEY_TYPE);
    expect_value(mock_hashtable_create, destroy_value_fn, runtime_env_value_destroy_adapter);
    will_return(mock_hashtable_create, DUMMY_HASHTABLE_P);

    runtime_env *ret = runtime_env_wind(DUMMY_CLOSURE_P);

    assert_non_null(ret);
    assert_ptr_equal(ret->bindings, DUMMY_HASHTABLE_P);
    assert_int_equal(ret->refcount, 1);
    assert_false(ret->is_root);
    assert_ptr_equal(ret->parent, DUMMY_CLOSURE_P);

    fake_free(ret);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
    const struct CMUnitTest make_number_tests[] = {
        cmocka_unit_test_setup_teardown(
            make_number_returns_null_when_first_allocation_fails,
            make_number_setup, make_number_teardown),
        cmocka_unit_test_setup_teardown(
            make_number_success_when_first_allocation_succeeds,
            make_number_setup, make_number_teardown),
    };

    const struct CMUnitTest make_string_tests[] = {
        cmocka_unit_test_setup_teardown(
            make_string_returns_null_when_s_null,
            make_string_setup, make_string_teardown),
        cmocka_unit_test_setup_teardown(
            make_string_returns_null_when_first_allocation_fails,
            make_string_setup, make_string_teardown),
        cmocka_unit_test_setup_teardown(
            make_string_returns_null_when_second_allocation_fails,
            make_string_setup, make_string_teardown),
        cmocka_unit_test_setup_teardown(
            make_string_success_when_two_allocations_succeed_and_s_non_null,
            make_string_setup, make_string_teardown),
    };

    const struct CMUnitTest make_symbol_tests[] = {
        cmocka_unit_test_setup_teardown(
            make_symbol_returns_null_when_first_allocation_fails,
            make_symbol_setup, make_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            make_symbol_success_when_first_allocation_succeeds,
            make_symbol_setup, make_symbol_teardown),
    };

    const struct CMUnitTest make_error_tests[] = {
        cmocka_unit_test_setup_teardown(
            make_error_returns_null_when_msg_null,
            make_error_setup, make_error_teardown),
        cmocka_unit_test_setup_teardown(
            make_error_returns_null_when_first_allocation_fails,
            make_error_setup, make_error_teardown),
        cmocka_unit_test_setup_teardown(
            make_error_returns_null_when_first_allocation_fails,
            make_error_setup, make_error_teardown),
        cmocka_unit_test_setup_teardown(
            make_error_success_when_two_allocations_succeed_and_msg_non_null,
            make_error_setup, make_error_teardown),
    };

    const struct CMUnitTest make_function_tests[] = {
        cmocka_unit_test_setup_teardown(
            make_function_returns_null_when_first_allocation_fails,
            make_function_setup, make_function_teardown),
        cmocka_unit_test_setup_teardown(
            make_function_success_when_first_allocation_succeeds,
            make_function_setup, make_function_teardown),
    };

    const struct CMUnitTest unwind_tests[] = {
        cmocka_unit_test_setup_teardown(
            unwind_returns_null_when_parent_null,
            unwind_setup, unwind_teardown),
        cmocka_unit_test_setup_teardown(
            unwind_success_when_parent_not_null,
            unwind_setup, unwind_teardown),
    };

    const struct CMUnitTest value_destroy_tests[] = {
        cmocka_unit_test_setup_teardown(
            value_destroy_do_nothing_when_value_null,
            value_destroy_setup, value_destroy_teardown),
        cmocka_unit_test_setup_teardown(
            value_destroy_success_when_type_number,
            value_destroy_setup, value_destroy_teardown),
        cmocka_unit_test_setup_teardown(
            value_destroy_success_when_type_string,
            value_destroy_setup, value_destroy_teardown),
        cmocka_unit_test_setup_teardown(
            value_destroy_success_when_type_symbol,
            value_destroy_setup, value_destroy_teardown),
        cmocka_unit_test_setup_teardown(
            value_destroy_success_when_type_error,
            value_destroy_setup, value_destroy_teardown),
        cmocka_unit_test_setup_teardown(
            value_destroy_success_when_type_function_and_closure_refcount_2_and_not_root,
            value_destroy_setup, value_destroy_teardown),
        cmocka_unit_test_setup_teardown(
            value_destroy_success_when_type_function_and_closure_refcount_1_and_not_root,
            value_destroy_setup, value_destroy_teardown),
        cmocka_unit_test_setup_teardown(
            value_destroy_success_when_type_function_and_closure_refcount_1_and_root,
            value_destroy_setup, value_destroy_teardown),
    };

    const struct CMUnitTest wind_tests[] = {
        cmocka_unit_test_setup_teardown(
            wind_returns_null_when_parent_null_and_first_allocation_fails,
            wind_setup, wind_teardown),
        cmocka_unit_test_setup_teardown(
            wind_returns_null_when_parent_null_and_hashtable_create_fails,
            wind_setup, wind_teardown),
        cmocka_unit_test_setup_teardown(
            wind_success_when_parent_null_and_hashtable_create_succeeds,
            wind_setup, wind_teardown),
        cmocka_unit_test_setup_teardown(
            wind_returns_null_when_parent_not_null_and_first_allocation_fails,
            wind_setup, wind_teardown),
        cmocka_unit_test_setup_teardown(
            wind_returns_null_when_parent_not_null_and_hashtable_create_fails,
            wind_setup, wind_teardown),
        cmocka_unit_test_setup_teardown(
            wind_success_when_parent_not_null_and_hashtable_create_succeeds,
            wind_setup, wind_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(make_number_tests, NULL, NULL);
    failed += cmocka_run_group_tests(make_string_tests, NULL, NULL);
    failed += cmocka_run_group_tests(make_symbol_tests, NULL, NULL);
    failed += cmocka_run_group_tests(make_error_tests, NULL, NULL);
    failed += cmocka_run_group_tests(make_function_tests, NULL, NULL);
    failed += cmocka_run_group_tests(unwind_tests, NULL, NULL);
    failed += cmocka_run_group_tests(value_destroy_tests, NULL, NULL);
    failed += cmocka_run_group_tests(wind_tests, NULL, NULL);

    return failed;
}
