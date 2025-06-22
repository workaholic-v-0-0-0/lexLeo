// src/symtab/test/test_symtab.h

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <assert.h>

#include "memory_allocator.h"
#include "string_utils.h"
#include "logger.h"

#include "internal/symtab_internal.h"



//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static char dummy;
static const void *DUMMY_MALLOC_RETURNED_VALUE = (void *) &dummy;
#define MALLOC_ERROR_CODE NULL



//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


void * mock_malloc(size_t size) {
    check_expected(size);
    return mock_type(void *);
}

void mock_free(void *ptr) {
    check_expected_ptr(ptr);
}



//-----------------------------------------------------------------------------
// create TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int create_teardown(void **state) {
    set_allocators(NULL, NULL);
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: any
// Expected: calls malloc with sizeof(symtab)
static void create_calls_malloc_for_a_symtab(void **state) {
    expect_value(mock_malloc, size, sizeof(symtab));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    symtab_create();
}

// Given: malloc fails
// Expected: return NULL
static void create_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(symtab));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(symtab_create());
}

// Given: malloc succeeds
// Expected: calls hashtable_create with SYMTAB_SIZE and ???
static void create_calls_hashtable_create_when_malloc_succeds(void **state) {
    expect_value(mock_malloc, size, sizeof(symtab));
    //will_return(mock_malloc, ???);

}





//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest create_tests[] = {
        cmocka_unit_test_setup_teardown(
            create_calls_malloc_for_a_symtab,
            create_setup, create_teardown),
        cmocka_unit_test_setup_teardown(
            create_returns_null_when_malloc_fails,
            create_setup, create_teardown),

    };
    int failed = 0;
    failed += cmocka_run_group_tests(create_tests, NULL, NULL);
    return failed;
}

