// src/data_structures/tests/test_hashtable.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

#include "logger.h"
#include "memory_allocator.h"

#include "internal/hashtable_internal.h"



//-----------------------------------------------------------------------------
// CONSTS
//-----------------------------------------------------------------------------


static const char dummy;
static const void *DUMMY_MALLOC_RETURNED_VALUE = (void *) &dummy;
static const void *DUMMY_FREE_RETURNED_VALUE = (void *) &dummy;
static const hashtable_destroy_value_fn_t DUMMY_HASHTABLE_DESTROY_VALUE_FN = (hashtable_destroy_value_fn_t) &dummy;
#define MALLOC_ERROR_CODE NULL



//-----------------------------------------------------------------------------
// MOCKS, STUBS, FAKES, DUMMIES
//-----------------------------------------------------------------------------


void * mock_malloc(size_t size) {
    check_expected(size);
    return mock_type(void *);
}

static void *fake_malloc_returned_value_for_hashtable;
static void *fake_malloc_returned_value_for_buckets;

void mock_free(void *ptr) {
    check_expected_ptr(ptr);
}



//-----------------------------------------------------------------------------
// GENERAL PARAMETRIC TEST STRUCTURE
//-----------------------------------------------------------------------------


typedef struct {
    const char *label;
    size_t s;
    hashtable_destroy_value_fn_t f;
} hashtable_test_params_t;



//-----------------------------------------------------------------------------
// GENERAL HELPERS
//-----------------------------------------------------------------------------


static size_t param_s(void **state) {
    return ((hashtable_test_params_t *) *state)->s;
}

static hashtable_destroy_value_fn_t param_f(void **state) {
    return ((hashtable_test_params_t *) *state)->f;
}



//-----------------------------------------------------------------------------
// GENERAL PARAMS (CASES)
//-----------------------------------------------------------------------------


static hashtable_test_params_t s_0_f_null = {
    .label = "s == 0, f == NULL",
    .s = 0,
    .f = NULL,
};

static hashtable_test_params_t s_0_f_dummy_not_null = {
    .label = "s == 0, f == dummy not null",
    .s = 0,
    .f = DUMMY_HASHTABLE_DESTROY_VALUE_FN,
};

static hashtable_test_params_t s_1_f_null = {
    .label = "s == 1, f == NULL",
    .s = 1,
    .f = NULL,
};

static hashtable_test_params_t s_1_f_dummy_not_null = {
    .label = "s == 1, f == dummy not null",
    .s = 1,
    .f = DUMMY_HASHTABLE_DESTROY_VALUE_FN,
};

static hashtable_test_params_t s_2_f_null = {
    .label = "s == 2, f == NULL",
    .s = 2,
    .f = NULL,
};

static hashtable_test_params_t s_2_f_dummy_not_null = {
    .label = "s == 2, f == dummy not null",
    .s = 2,
    .f = DUMMY_HASHTABLE_DESTROY_VALUE_FN,
};



//-----------------------------------------------------------------------------
// hashtable_create TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int hashtable_create_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    fake_malloc_returned_value_for_hashtable = malloc(sizeof(hashtable));
    fake_malloc_returned_value_for_buckets = malloc(param_s(state) * sizeof(entry *));
    assert_non_null(fake_malloc_returned_value_for_hashtable);
    return 0;
}

static int hashtable_create_teardown(void **state) {
    set_allocators(NULL, NULL);
    free(fake_malloc_returned_value_for_hashtable);
    free(fake_malloc_returned_value_for_buckets);
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: s == 0
// Expected: returns NULL
// param:
//	- s_0_f_null
//	- s_0_f_dummy_not_null
static void hashtable_create_returns_null_when_s_0(void **state) {
    assert_null(hashtable_create(param_s(state), param_f(state)));
}

// Given: s == 0
// Expected: calls neither DATA_STRUCTURE_MALLOC nor DATA_STRUCTURE_FREE
// param:
//	- s_0_f_null
//	- s_0_f_dummy_not_null
static void hashtable_create_calls_neither_malloc_nor_free_when_s_0(void **state) {
    hashtable_create(param_s(state), param_f(state));
}

// Given: s > 0
// Expected: calls DATA_STRUCTURE_MALLOC(sizeof(hashtable)) for hashtable
// param:
//	- s_1_f_null
//	- s_1_f_dummy_not_null
//	- s_2_f_null
//	- s_2_f_dummy_not_null
static void hashtable_create_calls_malloc_for_hashtable_with_right_params_when_s_not_0(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid an other mock call
    hashtable_create(param_s(state), param_f(state));
}

// Given: malloc call for hashtable fail
// Expected: returns NULL
// param:
//	- s_1_f_null
//	- s_1_f_dummy_not_null
//	- s_2_f_null
//	- s_2_f_dummy_not_null
static void hashtable_create_returns_null_when_malloc_for_hashtable_fail(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(hashtable_create(param_s(state), param_f(state)));
}

// Given: malloc call for hashtable success
// Expected: calls DATA_STRUCTURE_MALLOC(size * sizeof(entry *)) for buckets field
// param:
//	- s_1_f_null
//	- s_1_f_dummy_not_null
//	- s_2_f_null
//	- s_2_f_dummy_not_null
static void hashtable_create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(entry *));
    will_return(mock_malloc, fake_malloc_returned_value_for_buckets);
    hashtable_create(param_s(state), param_f(state));
}

// Given: malloc call for buckets field fail
// Expected: frees the pointer return by malloc call for hashtable
// param:
//	- s_1_f_null
//	- s_1_f_dummy_not_null
//	- s_2_f_null
//	- s_2_f_dummy_not_null
static void hashtable_create_frees_hashtable_when_malloc_for_buckets_fail(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(entry *));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    expect_value(mock_free, ptr, fake_malloc_returned_value_for_hashtable);
    hashtable_create(param_s(state), param_f(state));
}

// Given: malloc call for buckets success
// Expected: returns a hashtable * with correctly initialized buckets field
// param:
//	- s_1_f_null
//	- s_1_f_dummy_not_null
//	- s_2_f_null
//	- s_2_f_dummy_not_null
static void hashtable_create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(entry *));
    will_return(mock_malloc, fake_malloc_returned_value_for_buckets);
    hashtable *ret = hashtable_create(param_s(state), param_f(state));
    assert_non_null(ret);
    assert_ptr_equal(ret->buckets, fake_malloc_returned_value_for_buckets);
    size_t memory_area_size = param_s(state) * sizeof(entry *);
    unsigned char zeros[memory_area_size];
    memset(zeros, 0, memory_area_size);
    assert_memory_equal(ret->buckets, zeros, memory_area_size);
}

// Given: malloc call for buckets success
// Expected: returns a hashtable * with correctly initialized size field
// param:
//	- s_1_f_null
//	- s_1_f_dummy_not_null
//	- s_2_f_null
//	- s_2_f_dummy_not_null
static void hashtable_create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(entry *));
    will_return(mock_malloc, fake_malloc_returned_value_for_buckets);
    hashtable *ret = hashtable_create(param_s(state), param_f(state));
    assert_non_null(ret);
    assert_int_equal(ret->size, param_s(state));
}

// Given: malloc call for buckets success
// Expected: returns a hashtable * with correctly initialized destroy_value_fn field
// param:
//	- s_1_f_null
//	- s_1_f_dummy_not_null
//	- s_2_f_null
//	- s_2_f_dummy_not_null
static void hashtable_create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success(void **state) {
    expect_value(mock_malloc, size, sizeof(hashtable));
    will_return(mock_malloc, fake_malloc_returned_value_for_hashtable);
    expect_value(mock_malloc, size, param_s(state) * sizeof(entry *));
    will_return(mock_malloc, fake_malloc_returned_value_for_buckets);
    hashtable *ret = hashtable_create(param_s(state), param_f(state));
    assert_non_null(ret);
    assert_ptr_equal(ret->destroy_value_fn, param_f(state));
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest hashtable_create_tests[] = {
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_null_when_s_0,
            hashtable_create_setup, hashtable_create_teardown, &s_0_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_null_when_s_0,
            hashtable_create_setup, hashtable_create_teardown, &s_0_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_calls_neither_malloc_nor_free_when_s_0,
            hashtable_create_setup, hashtable_create_teardown, &s_0_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_calls_neither_malloc_nor_free_when_s_0,
            hashtable_create_setup, hashtable_create_teardown, &s_0_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            hashtable_create_setup, hashtable_create_teardown, &s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            hashtable_create_setup, hashtable_create_teardown, &s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            hashtable_create_setup, hashtable_create_teardown, &s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_calls_malloc_for_hashtable_with_right_params_when_s_not_0,
            hashtable_create_setup, hashtable_create_teardown, &s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_null_when_malloc_for_hashtable_fail,
            hashtable_create_setup, hashtable_create_teardown, &s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_null_when_malloc_for_hashtable_fail,
            hashtable_create_setup, hashtable_create_teardown, &s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_null_when_malloc_for_hashtable_fail,
            hashtable_create_setup, hashtable_create_teardown, &s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_null_when_malloc_for_hashtable_fail,
            hashtable_create_setup, hashtable_create_teardown, &s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            hashtable_create_setup, hashtable_create_teardown, &s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            hashtable_create_setup, hashtable_create_teardown, &s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            hashtable_create_setup, hashtable_create_teardown, &s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_calls_malloc_for_buckets_with_right_params_when_malloc_for_hashtable_success,
            hashtable_create_setup, hashtable_create_teardown, &s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_frees_hashtable_when_malloc_for_buckets_fail,
            hashtable_create_setup, hashtable_create_teardown, &s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_frees_hashtable_when_malloc_for_buckets_fail,
            hashtable_create_setup, hashtable_create_teardown, &s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_frees_hashtable_when_malloc_for_buckets_fail,
            hashtable_create_setup, hashtable_create_teardown, &s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_frees_hashtable_when_malloc_for_buckets_fail,
            hashtable_create_setup, hashtable_create_teardown, &s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            hashtable_create_setup, hashtable_create_teardown, &s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            hashtable_create_setup, hashtable_create_teardown, &s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            hashtable_create_setup, hashtable_create_teardown, &s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_hashtable_pointer_with_correctly_initialized_buckets_field_when_malloc_for_buckets_success,
            hashtable_create_setup, hashtable_create_teardown, &s_2_f_dummy_not_null),





        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            hashtable_create_setup, hashtable_create_teardown, &s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            hashtable_create_setup, hashtable_create_teardown, &s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            hashtable_create_setup, hashtable_create_teardown, &s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_hashtable_pointer_with_correctly_initialized_size_field_when_malloc_for_buckets_success,
            hashtable_create_setup, hashtable_create_teardown, &s_2_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            hashtable_create_setup, hashtable_create_teardown, &s_1_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            hashtable_create_setup, hashtable_create_teardown, &s_1_f_dummy_not_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            hashtable_create_setup, hashtable_create_teardown, &s_2_f_null),
        cmocka_unit_test_prestate_setup_teardown(
            hashtable_create_returns_hashtable_pointer_with_correctly_initialized_destroy_value_fn_field_when_malloc_for_buckets_success,
            hashtable_create_setup, hashtable_create_teardown, &s_2_f_dummy_not_null),

    };

    int failed = 0;
    failed += cmocka_run_group_tests(hashtable_create_tests, NULL, NULL);

    return failed;
}
