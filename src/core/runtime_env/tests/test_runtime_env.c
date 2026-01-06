// src/core/runtime_env/tests/test_runtime_env.c

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

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


// dummies

static const max_align_t DUMMY_SYMBOL;
static const struct symbol *const DUMMY_SYMBOL_P = (const struct symbol *)&DUMMY_SYMBOL;
static const max_align_t DUMMY_SYMBOL_2;
static const struct symbol *const DUMMY_SYMBOL_2_P = (const struct symbol *)&DUMMY_SYMBOL_2;
static const max_align_t DUMMY_FUNCTION_NODE;
static const struct ast *const DUMMY_FUNCTION_NODE_P = (const struct ast *)&DUMMY_FUNCTION_NODE;
static runtime_env DUMMY_CLOSURE = {.bindings = NULL, .refcount = 2, .is_root = false, .parent = NULL};
static runtime_env *const DUMMY_CLOSURE_P = &DUMMY_CLOSURE;
static runtime_env *const DUMMY_RUNTIME_ENV_P = &DUMMY_CLOSURE;
static const max_align_t DUMMY_AST;
static const struct ast *const DUMMY_AST_P = (const struct ast *)&DUMMY_AST;
static max_align_t DUMMY_HASHTABLE;
static struct hashtable *DUMMY_HASHTABLE_P = (struct hashtable *)&DUMMY_HASHTABLE;
static bool DUMMY_BOOL = false;
static runtime_env_value DUMMY_RUNTIME_ENV_VALUE;
static runtime_env_value *DUMMY_RUNTIME_ENV_VALUE_P = &DUMMY_RUNTIME_ENV_VALUE;


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

// spy

typedef struct {
    bool key_is_in_use_has_been_called;
    hashtable *key_is_in_use_arg_ht;
    const void *key_is_in_use_arg_key;

    bool add_has_been_called;
    hashtable *add_arg_ht;
    const void *add_arg_key;
    void *add_arg_value;

    bool get_has_been_called;
    const hashtable *get_arg_ht;
    const void *get_arg_key;

    bool reset_value_has_been_called;
    hashtable *reset_value_arg_ht;
    const void *reset_value_arg_key;
    void *reset_value_arg_value;
} hashtable_spy_t;

static hashtable_spy_t *g_hashtable_spy = NULL;

int spy_hashtable_key_is_in_use(hashtable *ht, const void *key) {
    assert_non_null(g_hashtable_spy);
    g_hashtable_spy->key_is_in_use_has_been_called = true;
    g_hashtable_spy->key_is_in_use_arg_ht = ht;
    g_hashtable_spy->key_is_in_use_arg_key = key;
    return mock_type(int);
}

int spy_hashtable_add(hashtable *ht, const void *key, void *value) {
    assert_non_null(g_hashtable_spy);
    g_hashtable_spy->add_has_been_called = true;
    g_hashtable_spy->add_arg_ht = ht;
    g_hashtable_spy->add_arg_key = key;
    g_hashtable_spy->add_arg_value = value;
    return mock_type(int);
}

void *spy_hashtable_get(const hashtable *ht, const void *key) {
    assert_non_null(g_hashtable_spy);
    g_hashtable_spy->get_has_been_called = true;
    g_hashtable_spy->get_arg_ht = ht;
    g_hashtable_spy->get_arg_key = key;
    return mock_type(void *);
}

int spy_hashtable_reset_value(hashtable *ht, const void *key, void *value) {
    assert_non_null(g_hashtable_spy);
    g_hashtable_spy->reset_value_has_been_called = true;
    g_hashtable_spy->reset_value_arg_ht = ht;
    g_hashtable_spy->reset_value_arg_key = key;
    g_hashtable_spy->reset_value_arg_value = value;
    return mock_type(int);
}


// stubs

void stub_hashtable_destroy(hashtable *ht) {}

static runtime_env STUB_RUNTIME_ENV_ROOT = {.bindings = NULL, .refcount = 1, .is_root = true, .parent = NULL};
static runtime_env *const STUB_RUNTIME_ENV_ROOT_P = &STUB_RUNTIME_ENV_ROOT;
static runtime_env STUB_RUNTIME_ENV_CHILD_OF_ROOT = {.bindings = NULL, .refcount = 2, .is_root = false, .parent = STUB_RUNTIME_ENV_ROOT_P};
static runtime_env *const STUB_RUNTIME_ENV_CHILD_OF_ROOT_P = &STUB_RUNTIME_ENV_CHILD_OF_ROOT;


// fakes

#define FAKABLE_MALLOC(n) (get_current_malloc()(n))
#define FAKABLE_FREE(p) (get_current_free()(p))
#define FAKABLE_STRDUP(s) (get_current_string_duplicate()(s))



//-----------------------------------------------------------------------------
// TESTS runtime_env_value *runtime_env_make_number(int i);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// runtime_env_make_number

// fake:
//  - functions of standard library which are used:
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
//  - ret->refcount == 1
//  - ret->type == RUNTIME_VALUE_NUMBER
//  - ret->as.i == A_INT
static void make_number_success_when_first_allocation_succeeds(void **state) {
    (void)state;

    runtime_env_value *ret = runtime_env_make_number(A_INT);

    assert_non_null(ret);
    assert_int_equal(ret->refcount, 1);
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
//  - functions of standard library which are used:
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
//  - ret->refcount == 1
//  - ret->type == RUNTIME_VALUE_STRING
//  - ret->as.s is a copy of s
static void make_string_success_when_two_allocations_succeed_and_s_non_null(void **state) {
    (void)state;

    runtime_env_value *ret = runtime_env_make_string(A_NON_NULL_STRING);

    assert_non_null(ret);
    assert_int_equal(ret->refcount, 1);
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
//  - functions of standard library which are used:
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
//  - ret->refcount == 1
//  - ret->type == RUNTIME_VALUE_SYMBOL
//  - ret->as.sym == DUMMY_SYMBOL_P
static void make_symbol_success_when_first_allocation_succeeds(void **state) {
    (void)state;

    runtime_env_value *ret = runtime_env_make_symbol(DUMMY_SYMBOL_P);

    assert_non_null(ret);
    assert_int_equal(ret->refcount, 1);
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
//  - functions of standard library which are used:
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
//  - ret->refcount == 1
//  - ret->type == RUNTIME_VALUE_ERROR
//  - ret->as.err.code == code
//  - ret->as.err.msg is a copy of msg
static void make_error_success_when_two_allocations_succeed_and_msg_non_null(void **state) {
    (void)state;

    runtime_env_value *ret = runtime_env_make_error(AN_ERROR_CODE, AN_ERROR_MESSAGE);

    assert_non_null(ret);
    assert_int_equal(ret->refcount, 1);
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
// closure->refcount

// dummy:
//  - function_node
//  - closure->bindings
//  - closure->is_root
//  - closure->parent
// fake:
//  - functions of standard library which are used:
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
//  - closure->refcount == A_INT
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
    runtime_env *fake_closure = fake_malloc(sizeof(runtime_env));
    fake_closure->bindings = DUMMY_HASHTABLE_P;
    fake_closure->refcount = A_INT;
    fake_closure->is_root = DUMMY_BOOL;
    fake_closure->parent = DUMMY_RUNTIME_ENV_P;
    fake_memory_fail_only_on_call(1);

    runtime_env_value *ret = runtime_env_make_function(DUMMY_FUNCTION_NODE_P, fake_closure);

    assert_null(ret);
    assert_non_null(fake_closure);
    assert_int_equal(fake_closure->refcount, A_INT);

    fake_free(fake_closure);
}

// Given:
//  - first allocation will succeed
// Expected:
//  - ret != NULL
//  - ret->refcount == 1
//  - ret->type == RUNTIME_VALUE_FUNCTION
//  - ret->as.fn.function_node == function_node
//  - ret->as.fn.closure == closure
//  - closure->refcount == A_INT + 1
static void make_function_success_when_first_allocation_succeeds(void **state) {
    (void)state;
    runtime_env *fake_closure = fake_malloc(sizeof(runtime_env));
    fake_closure->bindings = DUMMY_HASHTABLE_P;
    fake_closure->refcount = A_INT;
    fake_closure->is_root = DUMMY_BOOL;
    fake_closure->parent = DUMMY_RUNTIME_ENV_P;

    runtime_env_value *ret = runtime_env_make_function(DUMMY_FUNCTION_NODE_P, fake_closure);

    assert_non_null(ret);
    assert_int_equal(ret->refcount, 1);
    assert_int_equal(ret->type, RUNTIME_VALUE_FUNCTION);
    assert_ptr_equal(ret->as.fn.function_node, DUMMY_FUNCTION_NODE_P);
    assert_ptr_equal(ret->as.fn.closure, fake_closure);
    assert_non_null(fake_closure);
    assert_int_equal(fake_closure->refcount, A_INT + 1);

    fake_free(fake_closure);
    fake_free(ret);
}



//-----------------------------------------------------------------------------
// TESTS runtime_env_value *runtime_env_make_quoted(const struct ast *quoted);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// runtime_env_make_quoted

// dummy:
//  - quoted
// fake:
//  - functions of standard library which are used:
//    - malloc, free



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int make_quoted_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    fake_memory_reset();

    return 0;
}

static int make_quoted_teardown(void **state) {
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
//  - quoted == DUMMY_AST_P
// Expected:
//  - no invalid free
//  - no double free
//  - no memory leak


// Given:
//  - first allocation will fail
// Expected:
//  - ret == NULL
static void make_quoted_returns_null_when_first_allocation_fails(void **state) {
    (void)state;
    fake_memory_fail_only_on_call(1);

    assert_null(runtime_env_make_quoted(DUMMY_AST_P));
}

// Given:
//  - first allocation will succeed
// Expected:
//  - ret != NULL
//  - ret->refcount == 1
//  - ret->type == RUNTIME_VALUE_QUOTED
//  - ret->as.sym == DUMMY_AST_P
static void make_quoted_success_when_first_allocation_succeeds(void **state) {
    (void)state;

    runtime_env_value *ret = runtime_env_make_quoted(DUMMY_AST_P);

    assert_non_null(ret);
    assert_int_equal(ret->refcount, 1);
    assert_int_equal(ret->type, RUNTIME_VALUE_QUOTED);
    assert_ptr_equal(ret->as.quoted, DUMMY_AST_P);

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
//  - functions of standard library which are used:
//    - malloc, free



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int unwind_setup(void **state) {
    (void)state;

	// mock
    runtime_env_set_destroy_bindings(mock_hashtable_destroy);

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
    runtime_env_set_destroy_bindings(NULL);
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
// TESTS void runtime_env_value_release(const runtime_env_value *v);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// runtime_env_value_release
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
//  - functions of standard library which are used:
//    - malloc, free



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int value_release_setup(void **state) {
    (void)state;

	// mock
    runtime_env_set_destroy_bindings(mock_hashtable_destroy);

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    return 0;
}

static int value_release_teardown(void **state) {
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
static void value_release_do_nothing_when_value_null(void **state) {
    (void)state;
    runtime_env_value_release(NULL);
}

// Given:
//  - value->type == RUNTIME_VALUE_NUMBER
//  - value->refcount == 1
static void value_release_success_when_type_number(void **state) {
    (void)state;
    runtime_env_value *value = runtime_env_make_number(A_INT);

    runtime_env_value_release(value);
}

// Given:
//  - value->type == RUNTIME_VALUE_STRING
//  - value->refcount == 1
static void value_release_success_when_type_string(void **state) {
    (void)state;
    runtime_env_value *value = runtime_env_make_string(A_NON_NULL_STRING);

    runtime_env_value_release(value);
}

// Given:
//  - value->type == RUNTIME_VALUE_SYMBOL
//  - value->refcount == 1
// Expected:
//  - value->as.sym is not freed
static void value_release_success_when_type_symbol(void **state) {
    (void)state;
    runtime_env_value *value = runtime_env_make_symbol(DUMMY_SYMBOL_P);

    runtime_env_value_release(value);
}

// Given:
//  - value->type == RUNTIME_VALUE_ERROR
//  - value->refcount == 1
static void value_release_success_when_type_error(void **state) {
    (void)state;
    runtime_env_value *value = runtime_env_make_error(AN_ERROR_CODE, AN_ERROR_MESSAGE);

    runtime_env_value_release(value);
}

// Given:
//  - value->type == RUNTIME_VALUE_FUNCTION
//  - value->refcount == 1
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
static void value_release_success_when_type_function_and_closure_refcount_2_and_not_root(void **state) {
    (void)state;
	runtime_env *fake_closure = fake_malloc(sizeof(runtime_env));
	fake_closure->bindings = DUMMY_HASHTABLE_P;
	fake_closure->refcount = 1; // will be 2 after runtime_env_make_function call
	fake_closure->is_root = false;
	fake_closure->parent = DUMMY_RUNTIME_ENV_P;
	runtime_env_value *fake_runtime_value_function = runtime_env_make_function(DUMMY_FUNCTION_NODE_P, fake_closure);

	runtime_env_value_release(fake_runtime_value_function);

	assert_int_equal(fake_closure->refcount, 1);

	fake_free(fake_closure);
}

// Given:
//  - value->type == RUNTIME_VALUE_FUNCTION
//  - value->refcount == 1
//  - value->as.fn.closure->refcount == 1
//  - value->as.fn.closure->is_root == false
// WHEN DENOTING:
//  - closure = value->as.fn.closure
//  - function_node = value->as.fn.function_node
// Expected:
//  - function_node is not freed
//  - closure is freed
//  - value is freed
static void value_release_success_when_type_function_and_closure_refcount_1_and_not_root(void **state) {
    (void)state;
	runtime_env *fake_closure = fake_malloc(sizeof(runtime_env));
	fake_closure->bindings = DUMMY_HASHTABLE_P;
	fake_closure->refcount = 0; // will be 1 after runtime_env_make_function call
	fake_closure->is_root = false;
	fake_closure->parent = DUMMY_RUNTIME_ENV_P;
	runtime_env_value *fake_runtime_value_function = runtime_env_make_function(DUMMY_FUNCTION_NODE_P, fake_closure);
    expect_value(mock_hashtable_destroy, ht, DUMMY_HASHTABLE_P);

	runtime_env_value_release(fake_runtime_value_function);
}

// Given:
//  - value->type == RUNTIME_VALUE_FUNCTION
//  - value->refcount == 1
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
static void value_release_success_when_type_function_and_closure_refcount_1_and_root(void **state) {
    (void)state;
	runtime_env *fake_closure = fake_malloc(sizeof(runtime_env));
	fake_closure->bindings = DUMMY_HASHTABLE_P;
	fake_closure->refcount = 0; // will be 1 after runtime_env_make_function call
	fake_closure->is_root = true;
	fake_closure->parent = DUMMY_RUNTIME_ENV_P;
	runtime_env_value *fake_runtime_value_function = runtime_env_make_function(DUMMY_FUNCTION_NODE_P, fake_closure);

	runtime_env_value_release(fake_runtime_value_function);

	assert_int_equal(fake_closure->refcount, 1);

	fake_free(fake_closure);
}

// Given:
//  - value->type == RUNTIME_VALUE_QUOTED
//  - value->refcount == 1
// Expected:
//  - value->as.quoted is not freed
static void value_release_success_when_type_quoted(void **state) {
    (void)state;
    runtime_env_value *value = runtime_env_make_quoted(DUMMY_AST_P);

    runtime_env_value_release(value);
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
//  - functions of standard library which are used:
//    - malloc, free



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int wind_setup(void **state) {
    (void)state;

	// mock
    runtime_env_set_create_bindings(mock_hashtable_create);

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
    runtime_env_set_create_bindings(mock_hashtable_create);
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
// TESTS bool runtime_env_set_local(runtime_env *e, const struct symbol *key, const runtime_env_value *value);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// runtime_env_set_local
// in cases (value->type != RUNTIME_VALUE_FUNCTION)
//   - value
// in case (value->type == RUNTIME_VALUE_FUNCTION)
//   - value->type
//   - value->as.fn.closure->refcount
// runtime_env_make_number
// runtime_env_make_string
// runtime_env_make_symbol
// runtime_env_make_error
// runtime_env_make_function
// runtime_env_value_destroy

// dummy:
//  - e->bindings
//  - e->refcount
//  - e->is_root
//  - e->parent
//  - key
//  - in case (value->type == RUNTIME_VALUE_FUNCTION)
//    - value->as.fn.closure->bindings
//    - value->as.fn.closure->is_root
//    - value->as.fn.closure->parent
//    - value->as.fn.function_node
// mock:
//  - functions of the hashtable module which are used:
//    - hashtable_key_is_in_use
//    - hashtable_add
//    - hashtable_reset_value
//  - functions of standard library which are used:
//    - malloc, free, strdup



//-----------------------------------------------------------------------------
// PARAMETRIC CASE STRUCTURE
//-----------------------------------------------------------------------------


typedef struct {
    runtime_env_value_type type;
    size_t refcount;
    union {
        int i;
        const char *s;
        const struct symbol *sym;
        struct {
            int code;
            const char *msg;
        } err;
        struct {
            const struct ast *fn_node;
            const runtime_env *closure;
            size_t closure_refcount;
        } fn;
        const struct ast *quoted;
    } as;
} value_snapshot;

typedef struct {
    runtime_env *arg_e;
    const struct symbol *arg_key;
    const runtime_env_value *arg_value;
    runtime_env_value_type type;
    value_snapshot snapshot;
    runtime_env_value a_value_in_runtime_env;
    hashtable_spy_t hashtable_spy;
} set_local_ctx;

typedef struct {
    const char *name;

    // GIVEN
    bool arg_e_is_null;
    bool arg_key_is_null;
    bool arg_value_is_null;
    void (*the_arg_value)(set_local_ctx *ctx);
    bool hashtable_key_is_in_use_will_be_called;
    bool arg_key_is_in_bindings;
    bool hashtable_add_will_be_called;
    bool hashtable_add_will_succeed;
    bool hashtable_reset_value_will_be_called;
    bool hashtable_reset_value_will_succeed;

    // EXPECTED
    bool expected_returned_value;
    void (*expected_hashtable_usage_fn)(set_local_ctx *ctx);
    bool arg_value_is_unchanged_except_refcount;
    bool arg_value_refcount_is_incremented;
    bool arg_value_refcount_is_unchanged_too;

    // test infrastructure cleanup
    void (*test_infrastructure_garbage)(set_local_ctx *ctx);

    // context
    set_local_ctx *ctx;

} set_local_case;



//-----------------------------------------------------------------------------
// TEST RUNNER HELPERS AND FIXTURES HELPERS
//-----------------------------------------------------------------------------


static void take_arg_value_snapshot(set_local_ctx *ctx) {
    const runtime_env_value *arg_value = ctx->arg_value;
    ctx->snapshot.type = arg_value->type;
    ctx->snapshot.refcount = arg_value->refcount;
    switch (arg_value->type) {
    case RUNTIME_VALUE_NUMBER:
        ctx->snapshot.as.i = arg_value->as.i;
        break;
    case RUNTIME_VALUE_STRING:
        ctx->snapshot.as.s = arg_value->as.s;
        break;
    case RUNTIME_VALUE_SYMBOL:
        ctx->snapshot.as.sym = arg_value->as.sym;
        break;
    case RUNTIME_VALUE_ERROR:
        ctx->snapshot.as.err.code = arg_value->as.err.code;
        ctx->snapshot.as.err.msg = arg_value->as.err.msg;
        break;
    case RUNTIME_VALUE_FUNCTION:
        ctx->snapshot.as.fn.fn_node = arg_value->as.fn.function_node;
        ctx->snapshot.as.fn.closure = arg_value->as.fn.closure;
        ctx->snapshot.as.fn.closure_refcount = arg_value->as.fn.closure ? arg_value->as.fn.closure->refcount : 0;
        break;
    case RUNTIME_VALUE_QUOTED:
        ctx->snapshot.as.quoted = arg_value->as.quoted;
        break;
    default:
        assert_false(true);
    }
}

static inline void assert_value_invariance_except_refcount(set_local_ctx *ctx) {
    const runtime_env_value *arg_value = ctx->arg_value;
    assert_non_null(arg_value);
    assert_int_equal(arg_value->type, ctx->snapshot.type);
    switch (ctx->snapshot.type) {
    case RUNTIME_VALUE_NUMBER:
        assert_int_equal(arg_value->as.i, ctx->snapshot.as.i);
        break;
    case RUNTIME_VALUE_STRING:
        assert_ptr_equal(arg_value->as.s, ctx->snapshot.as.s);
        assert_string_equal(arg_value->as.s, ctx->snapshot.as.s);
        break;
    case RUNTIME_VALUE_SYMBOL:
        assert_ptr_equal(arg_value->as.sym, ctx->snapshot.as.sym);
        break;
    case RUNTIME_VALUE_ERROR:
        assert_int_equal(arg_value->as.err.code, ctx->snapshot.as.err.code);
        assert_ptr_equal(arg_value->as.err.msg, ctx->snapshot.as.err.msg);
        assert_string_equal(arg_value->as.err.msg, ctx->snapshot.as.err.msg);
        break;
    case RUNTIME_VALUE_FUNCTION:
        assert_ptr_equal(arg_value->as.fn.function_node, ctx->snapshot.as.fn.fn_node);
        assert_ptr_equal(arg_value->as.fn.closure, ctx->snapshot.as.fn.closure);
        assert_int_equal(arg_value->as.fn.closure ? arg_value->as.fn.closure->refcount : 0, ctx->snapshot.as.fn.closure_refcount);
        break;
    case RUNTIME_VALUE_QUOTED:
        assert_ptr_equal(arg_value->as.quoted, ctx->snapshot.as.quoted);
        break;
    default:
        assert_false(true);
    }
}

static void mock_spy_arrange(const set_local_case *p) {
    if (p->hashtable_key_is_in_use_will_be_called)
        will_return(
            spy_hashtable_key_is_in_use,
            p->arg_key_is_in_bindings ? true : false );
    if (p->hashtable_add_will_be_called)
        will_return(
            spy_hashtable_add,
            p->hashtable_add_will_succeed ? 0 : 1 );
/*
    if (p->hashtable_get_will_be_called)
        will_return(
            spy_hashtable_get,
            p->hashtable_get_will_succeed ?
                (void *) &p->ctx->a_value_in_runtime_env
                :
                NULL );
*/
    if (p->hashtable_reset_value_will_be_called)
        will_return(
            spy_hashtable_reset_value,
            p->hashtable_reset_value_will_succeed ? 0 : 1 );
}



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int set_local_parametric_setup(void **state) {
    const set_local_case *p = (const set_local_case *) *state;
    memset(p->ctx, 0, sizeof p->ctx);

    // initialise a value one should get from env
/*
    p->ctx->a_value_in_runtime_env = (runtime_env_value) {
        .refcount = 2,
        .type = RUNTIME_VALUE_NUMBER,
        .as.i = A_INT,
    };
*/

    // spy
    runtime_env_set_hashtable_ops(&(hashtable_ops_t){
        .hashtable_key_is_in_use = spy_hashtable_key_is_in_use,
        .hashtable_add = spy_hashtable_add,
        .hashtable_get = spy_hashtable_get,
        .hashtable_reset_value = spy_hashtable_reset_value,
    });
    p->ctx->hashtable_spy = (hashtable_spy_t){0};
    g_hashtable_spy = &p->ctx->hashtable_spy;

	// stub
    runtime_env_set_destroy_bindings(stub_hashtable_destroy);

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    return 0;
}

static int set_local_parametric_teardown(void **state) {
    const set_local_case *p = (const set_local_case *) *state;
    p->test_infrastructure_garbage(p->ctx);

    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());

    g_hashtable_spy = NULL;
    runtime_env_reset_hashtable_ops();
    runtime_env_set_destroy_bindings(NULL);
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
    fake_memory_reset();

    return 0;
}



//-----------------------------------------------------------------------------
// TEST RUNNER
//-----------------------------------------------------------------------------


static void set_local_parametric_test(void **state) {
    const set_local_case *p = (const set_local_case *) *state;

    // ARRANGE

    p->ctx->arg_e = p->arg_e_is_null ? NULL : DUMMY_RUNTIME_ENV_P;
    p->ctx->arg_key = p->arg_key_is_null ? NULL : DUMMY_SYMBOL_P;
    if (!p->arg_value_is_null && p->the_arg_value) {
        p->the_arg_value(p->ctx);
        take_arg_value_snapshot(p->ctx);
    }
    mock_spy_arrange(p);


    // ACT

    bool ret =
        runtime_env_set_local(
            p->ctx->arg_e,
            p->ctx->arg_key,
            p->ctx->arg_value );


    // ASSERT

    assert_int_equal(ret, p->expected_returned_value);
    if (p->ctx->arg_value) {
        assert_value_invariance_except_refcount(p->ctx);
        assert_int_equal(
            p->ctx->arg_value->refcount,
            p->arg_value_refcount_is_incremented ?
                p->ctx->snapshot.refcount + 1
                :
                p->ctx->snapshot.refcount );
    }
    if (p->expected_hashtable_usage_fn)
        p->expected_hashtable_usage_fn(p->ctx);
}



//-----------------------------------------------------------------------------
// PARAMETRIC CASES HELPERS
//-----------------------------------------------------------------------------


// RUNTIME VALUE INITIALIZERS

static void a_runtime_value_of_type_number(set_local_ctx *ctx) {
    ctx->arg_value = runtime_env_make_number(A_INT);
}

static void a_runtime_value_of_type_string(set_local_ctx *ctx) {
    ctx->arg_value = runtime_env_make_string(A_NON_NULL_STRING);
}

static void a_runtime_value_of_type_symbol(set_local_ctx *ctx) {
    ctx->arg_value = runtime_env_make_symbol(DUMMY_SYMBOL_P);
}

static void a_runtime_value_of_type_error(set_local_ctx *ctx) {
    ctx->arg_value = runtime_env_make_error(AN_ERROR_CODE, AN_ERROR_MESSAGE);
}

static void a_runtime_value_of_type_function(set_local_ctx *ctx) {
    ctx->arg_value = runtime_env_make_function(DUMMY_AST_P, DUMMY_CLOSURE_P);
}

static void a_runtime_value_of_type_quoted(set_local_ctx *ctx) {
    ctx->arg_value = runtime_env_make_quoted(DUMMY_AST_P);
}


// RUNTIME VALUE DESTRUCTOR FUNCTIONS

void nothing(set_local_ctx *ctx) {
    // no garbage to be destroyed
}

void destroy_arg_value(set_local_ctx *ctx) {
    while (ctx->arg_value->refcount)
        runtime_env_value_release((runtime_env_value *) ctx->arg_value);
}

#define the_runtime_value_of_type_number destroy_arg_value
#define the_runtime_value_of_type_string destroy_arg_value
#define the_runtime_value_of_type_symbol destroy_arg_value
#define the_runtime_value_of_type_error destroy_arg_value
#define the_runtime_value_of_type_function destroy_arg_value
#define the_runtime_value_of_type_quoted destroy_arg_value


// HASHTABLE USAGE EXPECTATION CHECKING

static void no_hashtable_usage(set_local_ctx *ctx) {
    assert_false(ctx->hashtable_spy.key_is_in_use_has_been_called);
    assert_false(ctx->hashtable_spy.add_has_been_called);
    assert_false(ctx->hashtable_spy.get_has_been_called);
    assert_false(ctx->hashtable_spy.reset_value_has_been_called);
}

static void check_if_arg_key_is_in_bindings(set_local_ctx *ctx) {
    assert_true(ctx->hashtable_spy.key_is_in_use_has_been_called);
    assert_ptr_equal(
        ctx->hashtable_spy.key_is_in_use_arg_ht,
        ctx->arg_e->bindings );
    assert_ptr_equal(
        ctx->hashtable_spy.key_is_in_use_arg_key,
        ctx->arg_key );
}

static void try_to_bind_arg_key_to_arg_value(set_local_ctx *ctx) {
    assert_true(ctx->hashtable_spy.add_has_been_called);
    assert_ptr_equal(
        ctx->hashtable_spy.add_arg_ht,
        ctx->arg_e->bindings );
    assert_ptr_equal(
        ctx->hashtable_spy.add_arg_key,
        ctx->arg_key );
    assert_ptr_equal(
        ctx->hashtable_spy.add_arg_value,
        ctx->arg_value );
}

static void try_to_reset_value_binded_to_arg_key_with_arg_value(set_local_ctx *ctx) {
    assert_true(ctx->hashtable_spy.reset_value_has_been_called);
    assert_ptr_equal(
        ctx->hashtable_spy.reset_value_arg_ht,
        ctx->arg_e->bindings );
    assert_ptr_equal(
        ctx->hashtable_spy.reset_value_arg_key,
        ctx->arg_key );
    assert_ptr_equal(
        ctx->hashtable_spy.reset_value_arg_value,
        ctx->arg_value );
}

static void check_if_arg_key_is_in_bindings_and_try_to_bind_it_to_arg_value(set_local_ctx *ctx) {
    check_if_arg_key_is_in_bindings(ctx);
    try_to_bind_arg_key_to_arg_value(ctx);
    assert_false(ctx->hashtable_spy.get_has_been_called);
    assert_false(ctx->hashtable_spy.reset_value_has_been_called);
}

static void check_if_arg_key_is_in_bindings_and_try_to_reset_arg_value(set_local_ctx *ctx) {
    check_if_arg_key_is_in_bindings(ctx);
    try_to_reset_value_binded_to_arg_key_with_arg_value(ctx);
}



//-----------------------------------------------------------------------------
// PARAMETRIC CASES
//-----------------------------------------------------------------------------


// INVALID ARGS

static set_local_ctx CTX_SET_LOCAL_CASE_ARG_VALUE_NULL = {0};
static const set_local_case SET_LOCAL_CASE_ARG_VALUE_NULL = {
    .name = "set_local_returns_false_when_arg_value_null",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .arg_value_is_null = true,

    // EXPECTED
    .expected_returned_value = false,
    .expected_hashtable_usage_fn = no_hashtable_usage,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = nothing,
    .ctx =&CTX_SET_LOCAL_CASE_ARG_VALUE_NULL,
};

static set_local_ctx CTX_SET_LOCAL_CASE_ARG_E_NULL = {0};
static const set_local_case SET_LOCAL_CASE_ARG_E_NULL = {
    .name = "set_local_returns_false_when_arg_e_null",

    // GIVEN
    .arg_e_is_null = true,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_number,

    // EXPECTED
    .expected_returned_value = false,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_unchanged_too = true,
    .expected_hashtable_usage_fn = no_hashtable_usage,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_number,
    .ctx =&CTX_SET_LOCAL_CASE_ARG_E_NULL,
};

static set_local_ctx CTX_SET_LOCAL_CASE_ARG_KEY_NULL = {0};
static const set_local_case SET_LOCAL_CASE_ARG_KEY_NULL = {
    .name = "set_local_returns_false_when_arg_key_null",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = true,
    .the_arg_value = a_runtime_value_of_type_number,

    // EXPECTED
    .expected_returned_value = false,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_unchanged_too = true,
    .expected_hashtable_usage_fn = no_hashtable_usage,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_number,
    .ctx =&CTX_SET_LOCAL_CASE_ARG_KEY_NULL,
};


// RUNTIME_VALUE_NUMBER CASES

static set_local_ctx CTX_SET_LOCAL_CASE_NUMBER_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS = {0};
static const set_local_case SET_LOCAL_CASE_NUMBER_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS = {
    .name = "set_local_returns_false_when_number_and_key_not_in_use_and_add_fails",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_number,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = false,
    .hashtable_add_will_be_called = true,
    .hashtable_add_will_succeed = false,

    // EXPECTED
    .expected_returned_value = false,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_unchanged_too = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_bind_it_to_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_number,
    .ctx =&CTX_SET_LOCAL_CASE_NUMBER_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_NUMBER_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS = {0};
static const set_local_case SET_LOCAL_CASE_NUMBER_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS = {
    .name = "set_local_returns_true_when_number_and_key_not_in_use_and_add_succeeds",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_number,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = false,
    .hashtable_add_will_be_called = true,
    .hashtable_add_will_succeed = true,

    // EXPECTED
    .expected_returned_value = true,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_incremented = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_bind_it_to_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_number,
    .ctx =&CTX_SET_LOCAL_CASE_NUMBER_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_NUMBER_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS = {0};
static const set_local_case SET_LOCAL_CASE_NUMBER_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS = {
    .name = "set_local_returns_false_when_number_and_key_in_use_and_reset_value_fails",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_number,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = true,
    .hashtable_reset_value_will_be_called = true,
    .hashtable_reset_value_will_succeed = false,

    // EXPECTED
    .expected_returned_value = false,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_unchanged_too = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_reset_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_number,
    .ctx =&CTX_SET_LOCAL_CASE_NUMBER_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_NUMBER_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS = {0};
static const set_local_case SET_LOCAL_CASE_NUMBER_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS = {
    .name = "set_local_returns_true_when_number_and_key_in_use_and_reset_value_succeeds",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_number,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = true,
    .hashtable_reset_value_will_be_called = true,
    .hashtable_reset_value_will_succeed = true,

    // EXPECTED
    .expected_returned_value = true,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_incremented = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_reset_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_number,
    .ctx =&CTX_SET_LOCAL_CASE_NUMBER_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS,
};


// RUNTIME_VALUE_STRING CASES

static set_local_ctx CTX_SET_LOCAL_CASE_STRING_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS = {0};
static const set_local_case SET_LOCAL_CASE_STRING_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS = {
    .name = "set_local_returns_false_when_string_and_key_not_in_use_and_add_fails",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_string,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = false,
    .hashtable_add_will_be_called = true,
    .hashtable_add_will_succeed = false,

    // EXPECTED
    .expected_returned_value = false,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_unchanged_too = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_bind_it_to_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_string,
    .ctx =&CTX_SET_LOCAL_CASE_STRING_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_STRING_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS = {0};
static const set_local_case SET_LOCAL_CASE_STRING_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS = {
    .name = "set_local_returns_true_when_string_and_key_not_in_use_and_add_succeeds",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_string,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = false,
    .hashtable_add_will_be_called = true,
    .hashtable_add_will_succeed = true,

    // EXPECTED
    .expected_returned_value = true,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_incremented = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_bind_it_to_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_string,
    .ctx =&CTX_SET_LOCAL_CASE_STRING_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_STRING_ARG_KEY_IN_BINDINGS_AND_RESET_FAILS = {0};
static const set_local_case SET_LOCAL_CASE_STRING_ARG_KEY_IN_BINDINGS_AND_RESET_FAILS = {
    .name = "set_local_returns_false_when_string_and_key_in_use_and_reset_value_fails",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_string,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = true,
    .hashtable_reset_value_will_be_called = true,
    .hashtable_reset_value_will_succeed = false,

    // EXPECTED
    .expected_returned_value = false,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_unchanged_too = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_reset_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_string,
    .ctx =&CTX_SET_LOCAL_CASE_STRING_ARG_KEY_IN_BINDINGS_AND_RESET_FAILS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_STRING_ARG_KEY_IN_BINDINGS_AND_RESET_SUCCEEDS = {0};
static const set_local_case SET_LOCAL_CASE_STRING_ARG_KEY_IN_BINDINGS_AND_RESET_SUCCEEDS = {
    .name = "set_local_returns_true_when_string_and_key_in_use_and_reset_value_succeeds",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_string,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = true,
    .hashtable_reset_value_will_be_called = true,
    .hashtable_reset_value_will_succeed = true,

    // EXPECTED
    .expected_returned_value = true,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_incremented = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_reset_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_string,
    .ctx =&CTX_SET_LOCAL_CASE_STRING_ARG_KEY_IN_BINDINGS_AND_RESET_SUCCEEDS,
};


// RUNTIME_VALUE_SYMBOL CASES

static set_local_ctx CTX_SET_LOCAL_CASE_SYMBOL_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS = {0};
static const set_local_case SET_LOCAL_CASE_SYMBOL_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS = {
    .name = "set_local_returns_false_when_symbol_and_key_not_in_use_and_add_fails",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_symbol,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = false,
    .hashtable_add_will_be_called = true,
    .hashtable_add_will_succeed = false,

    // EXPECTED
    .expected_returned_value = false,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_unchanged_too = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_bind_it_to_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_symbol,
    .ctx =&CTX_SET_LOCAL_CASE_SYMBOL_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_SYMBOL_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS = {0};
static const set_local_case SET_LOCAL_CASE_SYMBOL_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS = {
    .name = "set_local_returns_true_when_symbol_and_key_not_in_use_and_add_succeeds",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_symbol,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = false,
    .hashtable_add_will_be_called = true,
    .hashtable_add_will_succeed = true,

    // EXPECTED
    .expected_returned_value = true,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_incremented = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_bind_it_to_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_symbol,
    .ctx =&CTX_SET_LOCAL_CASE_SYMBOL_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_SYMBOL_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS = {0};
static const set_local_case SET_LOCAL_CASE_SYMBOL_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS = {
    .name = "set_local_returns_false_when_symbol_and_key_in_use_and_reset_value_fails",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_symbol,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = true,
    .hashtable_reset_value_will_be_called = true,
    .hashtable_reset_value_will_succeed = false,

    // EXPECTED
    .expected_returned_value = false,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_unchanged_too = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_reset_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_symbol,
    .ctx =&CTX_SET_LOCAL_CASE_SYMBOL_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_SYMBOL_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS = {0};
static const set_local_case SET_LOCAL_CASE_SYMBOL_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS = {
    .name = "set_local_returns_true_when_symbol_and_key_in_use_and_reset_value_succeeds",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_symbol,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = true,
    .hashtable_reset_value_will_be_called = true,
    .hashtable_reset_value_will_succeed = true,

    // EXPECTED
    .expected_returned_value = true,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_incremented = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_reset_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_symbol,
    .ctx =&CTX_SET_LOCAL_CASE_SYMBOL_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS,
};


// RUNTIME_VALUE_ERROR CASES

static set_local_ctx CTX_SET_LOCAL_CASE_ERROR_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS = {0};
static const set_local_case SET_LOCAL_CASE_ERROR_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS = {
    .name = "set_local_returns_false_when_error_and_key_not_in_use_and_add_fails",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_error,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = false,
    .hashtable_add_will_be_called = true,
    .hashtable_add_will_succeed = false,

    // EXPECTED
    .expected_returned_value = false,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_unchanged_too = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_bind_it_to_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_error,
    .ctx =&CTX_SET_LOCAL_CASE_ERROR_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_ERROR_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS = {0};
static const set_local_case SET_LOCAL_CASE_ERROR_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS = {
    .name = "set_local_returns_true_when_error_and_key_not_in_use_and_add_succeeds",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_error,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = false,
    .hashtable_add_will_be_called = true,
    .hashtable_add_will_succeed = true,

    // EXPECTED
    .expected_returned_value = true,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_incremented = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_bind_it_to_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_error,
    .ctx =&CTX_SET_LOCAL_CASE_ERROR_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_ERROR_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS = {0};
static const set_local_case SET_LOCAL_CASE_ERROR_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS = {
    .name = "set_local_returns_false_when_error_and_key_in_use_and_reset_value_fails",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_error,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = true,
    .hashtable_reset_value_will_be_called = true,
    .hashtable_reset_value_will_succeed = false,

    // EXPECTED
    .expected_returned_value = false,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_unchanged_too = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_reset_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_error,
    .ctx =&CTX_SET_LOCAL_CASE_ERROR_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_ERROR_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS = {0};
static const set_local_case SET_LOCAL_CASE_ERROR_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS = {
    .name = "set_local_returns_true_when_error_and_key_in_use_and_reset_value_succeeds",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_error,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = true,
    .hashtable_reset_value_will_be_called = true,
    .hashtable_reset_value_will_succeed = true,

    // EXPECTED
    .expected_returned_value = true,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_incremented = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_reset_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_error,
    .ctx =&CTX_SET_LOCAL_CASE_ERROR_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS,
};


// RUNTIME_VALUE_FUNCTION CASES

static set_local_ctx CTX_SET_LOCAL_CASE_FUNCTION_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS = {0};
static const set_local_case SET_LOCAL_CASE_FUNCTION_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS = {
    .name = "set_local_returns_false_when_function_and_key_not_in_use_and_add_fails",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_function,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = false,
    .hashtable_add_will_be_called = true,
    .hashtable_add_will_succeed = false,

    // EXPECTED
    .expected_returned_value = false,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_unchanged_too = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_bind_it_to_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_function,
    .ctx =&CTX_SET_LOCAL_CASE_FUNCTION_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_FUNCTION_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS = {0};
static const set_local_case SET_LOCAL_CASE_FUNCTION_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS = {
    .name = "set_local_returns_true_when_function_and_key_not_in_use_and_add_succeeds",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_function,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = false,
    .hashtable_add_will_be_called = true,
    .hashtable_add_will_succeed = true,

    // EXPECTED
    .expected_returned_value = true,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_incremented = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_bind_it_to_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_function,
    .ctx =&CTX_SET_LOCAL_CASE_FUNCTION_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_FUNCTION_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS = {0};
static const set_local_case SET_LOCAL_CASE_FUNCTION_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS = {
    .name = "set_local_returns_false_when_function_and_key_in_use_and_reset_value_fails",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_function,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = true,
    .hashtable_reset_value_will_be_called = true,
    .hashtable_reset_value_will_succeed = false,

    // EXPECTED
    .expected_returned_value = false,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_unchanged_too = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_reset_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_function,
    .ctx =&CTX_SET_LOCAL_CASE_FUNCTION_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_FUNCTION_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS = {0};
static const set_local_case SET_LOCAL_CASE_FUNCTION_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS = {
    .name = "set_local_returns_true_when_function_and_key_in_use_and_reset_value_fails",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_function,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = true,
    .hashtable_reset_value_will_be_called = true,
    .hashtable_reset_value_will_succeed = true,

    // EXPECTED
    .expected_returned_value = true,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_incremented = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_reset_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_function,
    .ctx =&CTX_SET_LOCAL_CASE_FUNCTION_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS,
};


// RUNTIME_VALUE_QUOTED CASES

static set_local_ctx CTX_SET_LOCAL_CASE_QUOTED_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS = {0};
static const set_local_case SET_LOCAL_CASE_QUOTED_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS = {
    .name = "set_local_returns_false_when_quoted_and_key_not_in_use_and_add_fails",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_quoted,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = false,
    .hashtable_add_will_be_called = true,
    .hashtable_add_will_succeed = false,

    // EXPECTED
    .expected_returned_value = false,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_unchanged_too = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_bind_it_to_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_quoted,
    .ctx =&CTX_SET_LOCAL_CASE_QUOTED_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_QUOTED_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS = {0};
static const set_local_case SET_LOCAL_CASE_QUOTED_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS = {
    .name = "set_local_returns_true_when_quoted_and_key_not_in_use_and_add_succeeds",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_quoted,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = false,
    .hashtable_add_will_be_called = true,
    .hashtable_add_will_succeed = true,

    // EXPECTED
    .expected_returned_value = true,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_incremented = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_bind_it_to_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_quoted,
    .ctx =&CTX_SET_LOCAL_CASE_QUOTED_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_QUOTED_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS = {0};
static const set_local_case SET_LOCAL_CASE_QUOTED_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS = {
    .name = "set_local_returns_false_when_quoted_and_key_in_use_and_reset_value_fails",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_quoted,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = true,
    .hashtable_reset_value_will_be_called = true,
    .hashtable_reset_value_will_succeed = false,

    // EXPECTED
    .expected_returned_value = false,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_unchanged_too = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_reset_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_quoted,
    .ctx =&CTX_SET_LOCAL_CASE_QUOTED_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS,
};

static set_local_ctx CTX_SET_LOCAL_CASE_QUOTED_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS = {0};
static const set_local_case SET_LOCAL_CASE_QUOTED_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS = {
    .name = "set_local_returns_true_when_quoted_and_key_in_use_and_reset_value_succeeds",

    // GIVEN
    .arg_e_is_null = false,
    .arg_key_is_null = false,
    .the_arg_value = a_runtime_value_of_type_quoted,
    .hashtable_key_is_in_use_will_be_called = true,
    .arg_key_is_in_bindings = true,
    .hashtable_reset_value_will_be_called = true,
    .hashtable_reset_value_will_succeed = true,

    // EXPECTED
    .expected_returned_value = true,
    .arg_value_is_unchanged_except_refcount = true,
    .arg_value_refcount_is_incremented = true,
    .expected_hashtable_usage_fn = check_if_arg_key_is_in_bindings_and_try_to_reset_arg_value,

    // test infrastructure cleanup and context
    .test_infrastructure_garbage = the_runtime_value_of_type_quoted,
    .ctx =&CTX_SET_LOCAL_CASE_QUOTED_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS,
};



//-----------------------------------------------------------------------------
// PARAMETRIC CASES REGISTRY
//-----------------------------------------------------------------------------
//
// Centralized registry of all parametric test cases for set_local.
// Each case is defined once here, then automatically expanded into a
// CMocka CMUnitTest array below.
//
// To add a new test case:
//     1. Define its `set_local_case` struct
//     2. Add one line in RUNTIME_ENV_SET_LOCAL_PARAM_CASES() below
//

#define RUNTIME_ENV_SET_LOCAL_PARAM_CASES(X) \
    X(SET_LOCAL_CASE_ARG_VALUE_NULL) \
    X(SET_LOCAL_CASE_ARG_E_NULL) \
    X(SET_LOCAL_CASE_ARG_KEY_NULL) \
    X(SET_LOCAL_CASE_NUMBER_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS) \
    X(SET_LOCAL_CASE_NUMBER_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS) \
    X(SET_LOCAL_CASE_NUMBER_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS) \
    X(SET_LOCAL_CASE_NUMBER_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS) \
    X(SET_LOCAL_CASE_STRING_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS) \
    X(SET_LOCAL_CASE_STRING_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS) \
    X(SET_LOCAL_CASE_STRING_ARG_KEY_IN_BINDINGS_AND_RESET_FAILS) \
    X(SET_LOCAL_CASE_STRING_ARG_KEY_IN_BINDINGS_AND_RESET_SUCCEEDS) \
    X(SET_LOCAL_CASE_SYMBOL_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS) \
    X(SET_LOCAL_CASE_SYMBOL_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS) \
    X(SET_LOCAL_CASE_SYMBOL_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS) \
    X(SET_LOCAL_CASE_ERROR_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS) \
    X(SET_LOCAL_CASE_ERROR_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS) \
    X(SET_LOCAL_CASE_ERROR_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS) \
    X(SET_LOCAL_CASE_ERROR_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS) \
    X(SET_LOCAL_CASE_FUNCTION_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS) \
    X(SET_LOCAL_CASE_FUNCTION_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS) \
    X(SET_LOCAL_CASE_FUNCTION_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS) \
    X(SET_LOCAL_CASE_FUNCTION_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS) \
    X(SET_LOCAL_CASE_QUOTED_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_FAILS) \
    X(SET_LOCAL_CASE_QUOTED_ARG_KEY_NOT_IN_BINDINGS_AND_ADD_SUCCEEDS) \
    X(SET_LOCAL_CASE_QUOTED_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_FAILS) \
    X(SET_LOCAL_CASE_QUOTED_ARG_KEY_IN_BINDINGS_AND_RESET_VALUE_SUCCEEDS)

#define MAKE_TEST(CASE_SYM) \
    { .name = CASE_SYM.name, \
    .test_func = set_local_parametric_test, \
    .setup_func = set_local_parametric_setup, \
    .teardown_func = set_local_parametric_teardown, \
    .initial_state = (void*)&CASE_SYM },

static const struct CMUnitTest set_local_parametric_tests[] = {
    RUNTIME_ENV_SET_LOCAL_PARAM_CASES(MAKE_TEST)
};

#undef MAKE_TEST



//-----------------------------------------------------------------------------
// TESTS runtime_env_value *runtime_env_get_local(const runtime_env *e, const struct symbol *key);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// runtime_env_get_local

// dummy:
//  - e->bindings
//  - e->refcount
//  - e->is_root
//  - e->parent
//  - key
// mock:
//  - functions of the hashtable module which are used:
//    - hashtable_get
// fake:
//  - functions of standard library which are used:
//    - malloc, free



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int get_local_setup(void **state) {
    (void)state;

	// mock
	runtime_env_set_hashtable_get(spy_hashtable_get);

    // fake
    set_allocators(fake_malloc, fake_free);
    fake_memory_reset();

    return 0;
}

static int get_local_teardown(void **state) {
    (void)state;
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
    runtime_env_reset_hashtable_ops();
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
//  - (e == NULL) || (key == NULL)
// Expected:
//  - ret == NULL
static void get_local_returns_null_when_e_or_key_null(void **state) {
    (void)state;
    assert_null(runtime_env_get_local(NULL, DUMMY_SYMBOL_P));
	assert_null(runtime_env_get_local(DUMMY_RUNTIME_ENV_P, NULL));
}

// Given:
//  - e == DUMMY_RUNTIME_ENV_P
//  - key == DUMMY_SYMBOL_P
// Expected:
//  - calls hashtable_get with:
//    - ht: DUMMY_RUNTIME_ENV_P->bindings
//    - key: DUMMY_SYMBOL_P
//  - ret == <returned value of hashtable_get>
static void get_local_success_when_e_and_key_not_null(void **state) {
    (void)state;
	expect_value(spy_hashtable_get, ht, DUMMY_RUNTIME_ENV_P->bindings);
	expect_value(spy_hashtable_get, key, DUMMY_SYMBOL_P);
	will_return(spy_hashtable_get, DUMMY_RUNTIME_ENV_VALUE_P);

    runtime_env_value *ret = runtime_env_get_local(DUMMY_RUNTIME_ENV_P, DUMMY_SYMBOL_P);

	assert_ptr_equal(ret, DUMMY_RUNTIME_ENV_VALUE_P);
}



//-----------------------------------------------------------------------------
// TESTS runtime_env_value *runtime_env_get(const runtime_env *e, const struct symbol *key);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// runtime_env_get

// - for arguments validity test:
//   - dummy:
//     - e
// - for not arguments validity tests:
//   - stub:
//     - e
// mock:
//  - functions of the hashtable module which are used:
//    - hashtable_key_is_in_use
//    - hashtable_get
// fake:
//  - functions of standard library which are used:
//    - malloc, free



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int get_setup(void **state) {
    (void)state;

	// mock
	runtime_env_set_hashtable_key_is_in_use(spy_hashtable_key_is_in_use);
	runtime_env_set_hashtable_get(spy_hashtable_get);

    // fake
    set_allocators(fake_malloc, fake_free);
    fake_memory_reset();

    return 0;
}

static int get_teardown(void **state) {
    (void)state;
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
    runtime_env_reset_hashtable_ops();
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
//  - (e == NULL) || (key == NULL)
// Expected:
//  - ret == NULL
static void get_returns_null_when_e_or_key_null(void **state) {
    (void)state;
    assert_null(runtime_env_get(NULL, DUMMY_SYMBOL_P));
	assert_null(runtime_env_get(DUMMY_RUNTIME_ENV_P, NULL));
}

// Given:
//  - e == STUB_RUNTIME_ENV_CHILD_OF_ROOT_P
//  - key == DUMMY_SYMBOL_P
//  - DUMMY_SYMBOL_P is a key of the hashtable STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->bindings
// Expected:
//  - calls hashtable_key_is_in_use with:
//    - ht: STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->bindings
//    - key: DUMMY_SYMBOL_P
//  - calls hashtable_get with:
//    - ht: STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->bindings
//    - key: DUMMY_SYMBOL_P
//  - ret == <returned value of hashtable_get>
static void get_success_when_key_in_use_in_current_scope(void **state) {
    (void)state;
	expect_value(spy_hashtable_key_is_in_use, ht, STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->bindings);
	expect_value(spy_hashtable_key_is_in_use, key, DUMMY_SYMBOL_P);
	will_return(spy_hashtable_key_is_in_use, true);
	expect_value(spy_hashtable_get, ht, STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->bindings);
	expect_value(spy_hashtable_get, key, DUMMY_SYMBOL_P);
	will_return(spy_hashtable_get, DUMMY_RUNTIME_ENV_VALUE_P);

	runtime_env_value *ret = runtime_env_get(STUB_RUNTIME_ENV_CHILD_OF_ROOT_P, DUMMY_SYMBOL_P);

	assert_ptr_equal(ret, DUMMY_RUNTIME_ENV_VALUE_P);
}

// Given:
//  - e == STUB_RUNTIME_ENV_CHILD_OF_ROOT_P
//  - key == DUMMY_SYMBOL_P
//  - DUMMY_SYMBOL_P is a key of the parent's scope
//    - ie it's a key of the hashtable STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->parent->bindings
// Expected:
//  - calls hashtable_key_is_in_use with:
//    - ht: STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->bindings
//    - key: DUMMY_SYMBOL_P
//  - calls hashtable_key_is_in_use with:
//    - ht: STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->parent->bindings
//    - key: DUMMY_SYMBOL_P
//  - calls hashtable_get with:
//    - ht: STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->parent->bindings
//    - key: DUMMY_SYMBOL_P
//  - ret == <returned value of hashtable_get>
static void get_success_when_key_in_use_in_parent_scope(void **state) {
    (void)state;
	expect_value(spy_hashtable_key_is_in_use, ht, STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->bindings);
	expect_value(spy_hashtable_key_is_in_use, key, DUMMY_SYMBOL_P);
	will_return(spy_hashtable_key_is_in_use, false);
	expect_value(spy_hashtable_key_is_in_use, ht, STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->parent->bindings);
	expect_value(spy_hashtable_key_is_in_use, key, DUMMY_SYMBOL_P);
	will_return(spy_hashtable_key_is_in_use, true);
	expect_value(spy_hashtable_get, ht, STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->parent->bindings);
	expect_value(spy_hashtable_get, key, DUMMY_SYMBOL_P);
	will_return(spy_hashtable_get, DUMMY_RUNTIME_ENV_VALUE_P);

	runtime_env_value *ret = runtime_env_get(STUB_RUNTIME_ENV_CHILD_OF_ROOT_P, DUMMY_SYMBOL_P);

	assert_ptr_equal(ret, DUMMY_RUNTIME_ENV_VALUE_P);
}

// Given:
//  - e == STUB_RUNTIME_ENV_CHILD_OF_ROOT_P
//  - key == DUMMY_SYMBOL_P
//  - DUMMY_SYMBOL_P is not a resolved symbol
//    - ie, it's not a key of the hashtable STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->bindings,
//      nor of the hashtable STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->parent->bindings
// Expected:
//  - calls hashtable_key_is_in_use with:
//    - ht: STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->bindings
//    - key: DUMMY_SYMBOL_P
//  - calls hashtable_key_is_in_use with:
//    - ht: STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->parent->bindings
//    - key: DUMMY_SYMBOL_P
//  - ret == NULL
static void get_returns_null_when_key_not_a_resolved_symbol(void **state) {
    (void)state;
	expect_value(spy_hashtable_key_is_in_use, ht, STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->bindings);
	expect_value(spy_hashtable_key_is_in_use, key, DUMMY_SYMBOL_P);
	will_return(spy_hashtable_key_is_in_use, false);
	expect_value(spy_hashtable_key_is_in_use, ht, STUB_RUNTIME_ENV_CHILD_OF_ROOT_P->parent->bindings);
	expect_value(spy_hashtable_key_is_in_use, key, DUMMY_SYMBOL_P);
	will_return(spy_hashtable_key_is_in_use, false);

	assert_null(runtime_env_get(STUB_RUNTIME_ENV_CHILD_OF_ROOT_P, DUMMY_SYMBOL_P));
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

    const struct CMUnitTest make_quoted_tests[] = {
        cmocka_unit_test_setup_teardown(
            make_quoted_returns_null_when_first_allocation_fails,
            make_quoted_setup, make_quoted_teardown),
        cmocka_unit_test_setup_teardown(
            make_quoted_success_when_first_allocation_succeeds,
            make_quoted_setup, make_quoted_teardown),
    };

    const struct CMUnitTest unwind_tests[] = {
        cmocka_unit_test_setup_teardown(
            unwind_returns_null_when_parent_null,
            unwind_setup, unwind_teardown),
        cmocka_unit_test_setup_teardown(
            unwind_success_when_parent_not_null,
            unwind_setup, unwind_teardown),
    };

    const struct CMUnitTest value_release_tests[] = {
        cmocka_unit_test_setup_teardown(
            value_release_do_nothing_when_value_null,
            value_release_setup, value_release_teardown),
        cmocka_unit_test_setup_teardown(
            value_release_success_when_type_number,
            value_release_setup, value_release_teardown),
        cmocka_unit_test_setup_teardown(
            value_release_success_when_type_string,
            value_release_setup, value_release_teardown),
        cmocka_unit_test_setup_teardown(
            value_release_success_when_type_symbol,
            value_release_setup, value_release_teardown),
        cmocka_unit_test_setup_teardown(
            value_release_success_when_type_error,
            value_release_setup, value_release_teardown),
        cmocka_unit_test_setup_teardown(
            value_release_success_when_type_function_and_closure_refcount_2_and_not_root,
            value_release_setup, value_release_teardown),
        cmocka_unit_test_setup_teardown(
            value_release_success_when_type_function_and_closure_refcount_1_and_not_root,
            value_release_setup, value_release_teardown),
        cmocka_unit_test_setup_teardown(
            value_release_success_when_type_function_and_closure_refcount_1_and_root,
            value_release_setup, value_release_teardown),
        cmocka_unit_test_setup_teardown(
            value_release_success_when_type_quoted,
            value_release_setup, value_release_teardown),
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

/*
    const struct CMUnitTest get_local_tests[] = {
        cmocka_unit_test_setup_teardown(
            get_local_returns_null_when_e_or_key_null,
            get_local_setup, get_local_teardown),
        cmocka_unit_test_setup_teardown(
            get_local_success_when_e_and_key_not_null,
            get_local_setup, get_local_teardown),
    };

    const struct CMUnitTest get_tests[] = {
        cmocka_unit_test_setup_teardown(
            get_returns_null_when_e_or_key_null,
            get_setup, get_teardown),
        cmocka_unit_test_setup_teardown(
            get_success_when_key_in_use_in_current_scope,
            get_setup, get_teardown),
        cmocka_unit_test_setup_teardown(
            get_success_when_key_in_use_in_parent_scope,
            get_setup, get_teardown),
        cmocka_unit_test_setup_teardown(
            get_returns_null_when_key_not_a_resolved_symbol,
            get_setup, get_teardown),

    };
*/

    int failed = 0;
    failed += cmocka_run_group_tests(make_number_tests, NULL, NULL);
    failed += cmocka_run_group_tests(make_string_tests, NULL, NULL);
    failed += cmocka_run_group_tests(make_symbol_tests, NULL, NULL);
    failed += cmocka_run_group_tests(make_error_tests, NULL, NULL);
    failed += cmocka_run_group_tests(make_function_tests, NULL, NULL);
    failed += cmocka_run_group_tests(make_quoted_tests, NULL, NULL);
    failed += cmocka_run_group_tests(unwind_tests, NULL, NULL);
    failed += cmocka_run_group_tests(value_release_tests, NULL, NULL);
    failed += cmocka_run_group_tests(wind_tests, NULL, NULL);
    failed += cmocka_run_group_tests(set_local_parametric_tests, NULL, NULL);
    //failed += cmocka_run_group_tests(get_local_tests, NULL, NULL);
    //failed += cmocka_run_group_tests(get_tests, NULL, NULL);

    return failed;
}
