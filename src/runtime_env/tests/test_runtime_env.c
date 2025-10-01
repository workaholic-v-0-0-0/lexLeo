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

    int failed = 0;
    failed += cmocka_run_group_tests(make_number_tests, NULL, NULL);

    return failed;
}
