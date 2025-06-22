// src/ast/tests/test_ast.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <assert.h>

#include "memory_allocator.h"
//#include "string_utils.h"
#include "logger.h"
#include "list.h"

#include "ast.h"

//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static list collected_ptr_to_be_freed = NULL;

static char dummy;
static const void *DUMMY_MALLOC_RETURNED_VALUE = (void *) &dummy;
#define MALLOC_ERROR_CODE NULL
static const int DUMMY_INT = 7;



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

static void *fake_malloc_returned_value_for_a_typed_data_int;



//-----------------------------------------------------------------------------
// GENERAL HELPERS
//-----------------------------------------------------------------------------


static void alloc_and_save_address_to_be_freed(void **ptr, size_t size) {
    *ptr = malloc(size);
    assert_non_null(*ptr);
    set_allocators(NULL, NULL);
    collected_ptr_to_be_freed = list_push(collected_ptr_to_be_freed, *ptr);
    set_allocators(mock_malloc, mock_free);
}



//-----------------------------------------------------------------------------
// ast_create_typed_data_int TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_typed_data_int_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int create_typed_data_int_teardown(void **state) {
    set_allocators(NULL, NULL);
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: any
// Expected: calls malloc with sizeof(typed_data)
static void create_typed_data_int_calls_malloc_for_a_typed_data(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    ast_create_typed_data_int(DUMMY_INT);
}

// Given: malloc fails
// Expected: return NULL
static void create_typed_data_int_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(ast_create_typed_data_int(DUMMY_INT));
}

// Given: malloc succeeds
// Expected: structure is correctly initialized and pointer is the malloc'ed one
static void create_typed_data_int_initializes_fields_when_malloc_succeds(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_typed_data_int, sizeof(typed_data));
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_int);
    typed_data *ret = ast_create_typed_data_int(DUMMY_INT);
    assert_int_equal(ret->type, TYPE_INT);
    assert_int_equal((ret->data).int_value, DUMMY_INT);
    assert_ptr_equal(ret, fake_malloc_returned_value_for_a_typed_data_int);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest create_typed_data_int_tests[] = {
        cmocka_unit_test_setup_teardown(
            create_typed_data_int_calls_malloc_for_a_typed_data,
            create_typed_data_int_setup, create_typed_data_int_teardown),
        cmocka_unit_test_setup_teardown(
            create_typed_data_int_returns_null_when_malloc_fails,
            create_typed_data_int_setup, create_typed_data_int_teardown),
        cmocka_unit_test_setup_teardown(
            create_typed_data_int_initializes_fields_when_malloc_succeds,
            create_typed_data_int_setup, create_typed_data_int_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(create_typed_data_int_tests, NULL, NULL);
    return failed;
}
