// src/runtime_env/tests/test_runtime_env.c

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "internal/runtime_env_internal.h"
#include "internal/runtime_env_test_utils.h"
#include "fake_memory.h"
#include "memory_allocator.h"
#include "string_utils.h"



//-----------------------------------------------------------------------------
// GLOBALS NOT DOUBLES
//-----------------------------------------------------------------------------


static const int A_INT = 7;
static const char *A_NON_NULL_STRING = "a non null string";



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


// mocks



// stubs



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

// mock:
//  - none
// stub:
//  - none
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

// mock:
//  - none
// stub:
//  - none
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

    int failed = 0;
    failed += cmocka_run_group_tests(make_number_tests, NULL, NULL);
    failed += cmocka_run_group_tests(make_string_tests, NULL, NULL);

    return failed;
}
