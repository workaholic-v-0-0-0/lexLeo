// src/data_structures/tests/test_hashtable.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>

#include "logger.h"
#include "list.h"

#include "hashtable.h"



//-----------------------------------------------------------------------------
// CONSTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// MOCKS, STUBS, FAKES, DUMMIES
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// GENERAL HELPERS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// hashtable_create TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// PARAMETRIC TEST STRUCTURE
//-----------------------------------------------------------------------------


typedef struct {
    const char *label;

} hashtable_create_test_params_t;



//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// PARAMS (CASES)
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int hashtable_create_setup(void **state) {


    return 0;
}

static int hashtable_create_teardown(void **state) {


    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------

/* tests list
hashtable *hashtable_create(
    size_t size,
    hashtable_destroy_value
);
hashtable_create(0, NULL) returns NULL
hashtable_create(1, NULL) returns non null pointer
*/

// Given: s == 0, f == NULL
// Expected: returns NULL
// param: N/A yet
static void hashtable_create_returns_null_when_s_0_f_null(void **state) {
    assert_null(hashtable_create(0, NULL));
}


//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest hashtable_create_tests[] = {
        cmocka_unit_test(hashtable_create_returns_null_when_s_0_f_null),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(hashtable_create_tests, NULL, NULL);

    return failed;
}
