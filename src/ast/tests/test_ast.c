// src/ast/tests/test_ast.c

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
#include "list.h"

#include "ast.h"

//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static list collected_ptr_to_be_freed = NULL;

static char dummy;
static const void *DUMMY_MALLOC_RETURNED_VALUE = (void *) &dummy;
static const void *DUMMY_STRDUP_RETURNED_VALUE = &dummy;
#define MALLOC_ERROR_CODE NULL
static const int DUMMY_INT = 7;
static char *const DUMMY_STRING = "dummy string";
#define STRDUP_ERROR_CODE NULL
static void *const DUMMY_SYMBOL = (void *) &dummy;



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

char *mock_strdup(const char *s) {
    check_expected_ptr(s);
    return mock_type(char *);
}

static void *fake_malloc_returned_value_for_a_typed_data_int = NULL;
static void *fake_malloc_returned_value_for_a_typed_data_string = NULL;
static void *fake_malloc_returned_value_for_a_typed_data_symbol = NULL;
static void *fake_typed_data_int = NULL;
static char *fake_strdup_returned_value_for_string_value = NULL;
static char *string_value = NULL;
static typed_data *typed_data_string = NULL;



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
// Expected: the malloc'ed typed_data is initialized and returned
static void create_typed_data_int_initializes_and_returns_malloced_typed_data_when_malloc_succeds(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_typed_data_int, sizeof(typed_data));
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_int);
    typed_data *ret = ast_create_typed_data_int(DUMMY_INT);
    assert_int_equal(ret->type, TYPE_INT);
    assert_int_equal((ret->data).int_value, DUMMY_INT);
    assert_ptr_equal(ret, fake_malloc_returned_value_for_a_typed_data_int);
}



//-----------------------------------------------------------------------------
// ast_destroy_typed_data_int TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_typed_data_int_setup(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_typed_data_int, sizeof(typed_data));
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int destroy_typed_data_int_teardown(void **state) {
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
// Expected: calls free with typed_data_int
static void destroy_typed_data_int_calls_free(void **state) {
    expect_value(mock_free, ptr, fake_typed_data_int);
    ast_destroy_typed_data_int(fake_typed_data_int);
}



//-----------------------------------------------------------------------------
// ast_create_typed_data_string TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int ast_create_typed_data_string_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    set_string_duplicate(mock_strdup);
    return 0;
}

static int ast_create_typed_data_string_teardown(void **state) {
    set_string_duplicate(NULL);
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
static void ast_create_typed_data_string_calls_malloc_for_a_typed_data(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid mock call
    ast_create_typed_data_string(DUMMY_STRING);
}

// Given: malloc fails
// Expected: returns NULL
static void ast_create_typed_data_string_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(ast_create_typed_data_string(DUMMY_STRING));
}

// Given: malloc succeeds
// Expected: calls strdup with s
static void ast_create_typed_data_string_calls_strdup_when_malloc_succeds(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(mock_strdup, s, DUMMY_STRING);
    will_return(mock_strdup, DUMMY_STRDUP_RETURNED_VALUE);
    ast_create_typed_data_string(DUMMY_STRING);
}

// Given: strdup fails
// Expected: calls free with pointer returned by malloc for a typed data and returns null
static void ast_create_typed_data_string_calls_free_and_returns_null_when_strdup_fails(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_typed_data_string, sizeof(typed_data));
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_string);
    expect_value(mock_strdup, s, DUMMY_STRING);
    will_return(mock_strdup, STRDUP_ERROR_CODE);
    expect_value(mock_free, ptr, fake_malloc_returned_value_for_a_typed_data_string);
    ast_create_typed_data_string(DUMMY_STRING);
}

// Given: strdup succeeds
// Expected: the malloc'ed typed_data is initialized and returned
static void ast_create_typed_data_string_initializes_and_returns_malloced_typed_data_when_strdup_succeds(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_typed_data_string, sizeof(typed_data));
    alloc_and_save_address_to_be_freed((void **)&fake_strdup_returned_value_for_string_value, strlen(DUMMY_STRING) + 1);
    strcpy(fake_strdup_returned_value_for_string_value, DUMMY_STRING);
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_string);
    expect_value(mock_strdup, s, DUMMY_STRING);
    will_return(mock_strdup, fake_strdup_returned_value_for_string_value);
    typed_data *ret = ast_create_typed_data_string(DUMMY_STRING);
    assert_ptr_equal(ret, fake_malloc_returned_value_for_a_typed_data_string);
    assert_int_equal(ret->type, TYPE_STRING);
    assert_ptr_equal(ret->data.string_value, fake_strdup_returned_value_for_string_value);
    assert_memory_equal(ret->data.string_value, DUMMY_STRING, strlen(DUMMY_STRING) + 1);
}



//-----------------------------------------------------------------------------
// ast_destroy_typed_data_string TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_typed_data_string_setup(void **state) {
    alloc_and_save_address_to_be_freed((void **)&typed_data_string, sizeof(typed_data));
    alloc_and_save_address_to_be_freed((void **)&string_value, strlen(DUMMY_STRING)+1);
    typed_data_string->type = TYPE_STRING;
    typed_data_string->data.string_value = string_value;
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int destroy_typed_data_string_teardown(void **state) {
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
// Expected: calls free with typed_data_string->data.string_value and then with typed_data_string
static void destroy_typed_data_string_calls_free_for_string_value_field_then_for_typed_data_string(void **state) {
    expect_value(mock_free, ptr, typed_data_string->data.string_value);
    expect_value(mock_free, ptr, typed_data_string);
    ast_destroy_typed_data_string(typed_data_string);
}



//-----------------------------------------------------------------------------
// ast_create_typed_data_symbol TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_typed_data_symbol_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int create_typed_data_symbol_teardown(void **state) {
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
static void create_typed_data_symbol_calls_malloc_for_a_typed_data(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid mock call
    ast_create_typed_data_symbol(DUMMY_SYMBOL);
}

// Given: malloc fails
// Expected: return NULL
static void create_typed_data_symbol_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(ast_create_typed_data_symbol(DUMMY_SYMBOL));
}

// Given: malloc succeeds
// Expected: the malloc'ed typed_data is initialized and returned
static void create_typed_data_symbol_initializes_and_returns_malloced_typed_data_when_malloc_succeds(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_typed_data_symbol, sizeof(typed_data));
    expect_value(mock_malloc, size, sizeof(typed_data));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_typed_data_symbol);
    typed_data *ret = ast_create_typed_data_symbol(DUMMY_SYMBOL);
    assert_int_equal(ret->type, TYPE_SYMBOL);
    assert_ptr_equal((ret->data).symbol_value, DUMMY_SYMBOL);
    assert_ptr_equal(ret, fake_malloc_returned_value_for_a_typed_data_symbol);
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
            create_typed_data_int_initializes_and_returns_malloced_typed_data_when_malloc_succeds,
            create_typed_data_int_setup, create_typed_data_int_teardown),
    };

    const struct CMUnitTest ast_destroy_typed_data_int_tests[] = {
        cmocka_unit_test_setup_teardown(
            destroy_typed_data_int_calls_free,
            destroy_typed_data_int_setup, destroy_typed_data_int_teardown),
    };

    const struct CMUnitTest ast_create_typed_data_string_tests[] = {
        cmocka_unit_test_setup_teardown(
            ast_create_typed_data_string_calls_malloc_for_a_typed_data,
            ast_create_typed_data_string_setup, ast_create_typed_data_string_teardown),
        cmocka_unit_test_setup_teardown(
            ast_create_typed_data_string_returns_null_when_malloc_fails,
            ast_create_typed_data_string_setup, ast_create_typed_data_string_teardown),
        cmocka_unit_test_setup_teardown(
            ast_create_typed_data_string_calls_strdup_when_malloc_succeds,
            ast_create_typed_data_string_setup, ast_create_typed_data_string_teardown),
        cmocka_unit_test_setup_teardown(
            ast_create_typed_data_string_calls_free_and_returns_null_when_strdup_fails,
            ast_create_typed_data_string_setup, ast_create_typed_data_string_teardown),
        cmocka_unit_test_setup_teardown(
            ast_create_typed_data_string_initializes_and_returns_malloced_typed_data_when_strdup_succeds,
            ast_create_typed_data_string_setup, ast_create_typed_data_string_teardown),
    };

    const struct CMUnitTest ast_destroy_typed_data_string_tests[] = {
        cmocka_unit_test_setup_teardown(
            destroy_typed_data_string_calls_free_for_string_value_field_then_for_typed_data_string,
            destroy_typed_data_string_setup, destroy_typed_data_string_teardown),
    };

    const struct CMUnitTest ast_create_typed_data_symbol_tests[] = {
        cmocka_unit_test_setup_teardown(
            create_typed_data_symbol_calls_malloc_for_a_typed_data,
            create_typed_data_symbol_setup, create_typed_data_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            create_typed_data_symbol_returns_null_when_malloc_fails,
            create_typed_data_symbol_setup, create_typed_data_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            create_typed_data_symbol_initializes_and_returns_malloced_typed_data_when_malloc_succeds,
            create_typed_data_symbol_setup, create_typed_data_symbol_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(create_typed_data_int_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_typed_data_int_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_typed_data_string_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_destroy_typed_data_string_tests, NULL, NULL);
    failed += cmocka_run_group_tests(ast_create_typed_data_symbol_tests, NULL, NULL);

    return failed;
}
