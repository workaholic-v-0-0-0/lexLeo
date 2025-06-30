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
#include "list.h"
#include "ast.h"

#include "internal/symtab_internal.h"



//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static const char DUMMY[10];
static const void *const DUMMY_MALLOC_RETURNED_VALUE = (void *) &DUMMY[0];
static const char *const DUMMY_STRING = (char *) &DUMMY[1];
#define MALLOC_ERROR_CODE NULL
static const ast *const DUMMY_AST_P = (ast *) &DUMMY[2];
static const ast_children_t *const DUMMY_CHILDREN_INFO_P = (ast_children_t *) &DUMMY[3];
static const typed_data *const DUMMY_TYPED_DATA_P = (typed_data *) &DUMMY[4];
static const hashtable *const DUMMY_HASHTABLE_P = (hashtable *) &DUMMY[5];
static const size_t DUMMY_SIZE_STRUCT_HASHTABLE = 1;
static const symtab *const DUMMY_SYMTAB_P = (symtab *) &DUMMY[6];

static list collected_ptr_to_be_freed = NULL;



//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


void * mock_malloc(size_t size) {
    check_expected(size);
    return mock_type(void *);
}

static void *fake_malloc_returned_value_for_a_symtab;

void mock_free(void *ptr) {
    check_expected_ptr(ptr);
}

void ast_destroy_typed_data_wrapper(ast *ast_data_wrapper) {
    check_expected(ast_data_wrapper);
}

void ast_destroy_non_typed_data_wrapper(ast *non_typed_data_wrapper) {
    check_expected(non_typed_data_wrapper);
}

hashtable *hashtable_create(size_t size, hashtable_destroy_value_fn_t destroy_value_fn) {
    check_expected(size);
    check_expected(destroy_value_fn);
    return mock_type(hashtable *);
}

static hashtable *fake_hashtable_create_returned_value;



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
// symtab_destroy_symbol TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_symbol_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int destroy_symbol_teardown(void **state) {
    set_allocators(NULL, NULL);
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: symbol = NULL
// Expected: do nothing
static void destroy_symbol_do_nothing_when_symbol_null(void **state) {
    symtab_destroy_symbol(NULL);
}

// Given: symbol != NULL, symbol->image = NULL
// Expected:
//  - frees symbol->name
//  - frees symbol
static void destroy_symbol_frees_name_and_symbol_when_image_null(void **state) {
    symbol *s;
    alloc_and_save_address_to_be_freed((void **)&s, sizeof(symbol));
    s->name = (char *) DUMMY_STRING;
    s->image = NULL;

    symtab_destroy_symbol((void *) s);
}

// Given:
//  - symbol != NULL
//  - symbol->image != NULL
//  - symbol->image->type = AST_TYPE_DATA_WRAPPER
// Expected:
//  - calls ast_destroy_typed_data_wrapper with symbol->image
//  - symbol->name IS NOT FREED because hashtable owns its keys memory
static void destroy_symbol_calls_ast_destroy_typed_data_wrapper_when_image_is_data_wrapper(void **state) {
    symbol *s;
    alloc_and_save_address_to_be_freed((void **)&s, sizeof(symbol));
    s->name = (char *) DUMMY_STRING;
    alloc_and_save_address_to_be_freed((void **)&(s->image), sizeof(ast));
    s->image->type = AST_TYPE_DATA_WRAPPER;
    s->image->data = (typed_data *) DUMMY_TYPED_DATA_P;

    expect_value(ast_destroy_typed_data_wrapper, ast_data_wrapper, s->image);

    symtab_destroy_symbol((void *) s);
}

// Given:
//  - symbol != NULL
//  - symbol->image != NULL
//  - symbol->image->type != AST_TYPE_DATA_WRAPPER
// Expected:
//  - calls ast_destroy_non_typed_data_wrapper with symbol->image
//  - symbol->name IS NOT FREED because hashtable owns its keys memory
static void destroy_symbol_calls_ast_destroy_non_typed_data_wrapper_when_image_is_not_data_wrapper(void **state) {
    symbol *s;
    alloc_and_save_address_to_be_freed((void **)&s, sizeof(symbol));
    s->name = (char *) DUMMY_STRING;
    alloc_and_save_address_to_be_freed((void **)&(s->image), sizeof(ast));
    s->image->type = AST_TYPE_ADDITION;
    s->image->children = (ast_children_t *) DUMMY_CHILDREN_INFO_P;

    expect_value(ast_destroy_non_typed_data_wrapper, non_typed_data_wrapper, s->image);

    symtab_destroy_symbol((void *) s);
}



//-----------------------------------------------------------------------------
// symtab_wind_scope TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int wind_scope_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    return 0;
}

static int wind_scope_teardown(void **state) {
    set_allocators(NULL, NULL);
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: any
// Expected: calls malloc with sizeof(symtab)
static void wind_scope_calls_malloc_for_a_symtab(void **state) {
    expect_value(mock_malloc, size, sizeof(symtab));
    will_return(mock_malloc, MALLOC_ERROR_CODE); // to avoid more mock call

    symtab_wind_scope((symtab *) DUMMY_SYMTAB_P);
}


// Given: malloc fails
// Expected: return NULL
static void wind_scope_returns_null_when_malloc_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(symtab));
    will_return(mock_malloc, MALLOC_ERROR_CODE);
    assert_null(symtab_wind_scope((symtab *) DUMMY_SYMTAB_P));
}

// Given: malloc succeeds
// Expected:
//  - calls hashtable_create with:
//     - size: SYMTAB_SIZE
//     - destroy_value_fn: symtab_destroy_symbol
static void wind_scope_calls_hashtable_create_when_malloc_succeds(void **state) {
    expect_value(mock_malloc, size, sizeof(symtab));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(hashtable_create, size, SYMTAB_SIZE);
    expect_value(hashtable_create, destroy_value_fn, symtab_destroy_symbol);

    // finish a scenario tested further to avoid segmentation fault
    will_return(hashtable_create, NULL);
    expect_value(mock_free, ptr, DUMMY_MALLOC_RETURNED_VALUE);

    symtab_wind_scope((symtab *) DUMMY_SYMTAB_P);
}

// Given: hashtable_create fails
// Expected:
//  - frees malloc'ed symtab
//  - returns NULL
static void wind_scope_returns_null_when_hashtable_create_fails(void **state) {
    expect_value(mock_malloc, size, sizeof(symtab));
    will_return(mock_malloc, DUMMY_MALLOC_RETURNED_VALUE);
    expect_value(hashtable_create, size, SYMTAB_SIZE);
    expect_value(hashtable_create, destroy_value_fn, symtab_destroy_symbol);
    will_return(hashtable_create, NULL);
    expect_value(mock_free, ptr, DUMMY_MALLOC_RETURNED_VALUE);

    assert_null(symtab_wind_scope((symtab *) DUMMY_SYMTAB_P));
}

// Given: hashtable_create succeeds
// Expected: malloc'ed symtab is initialized and returned
static void wind_scope_calls_initializes_and_returns_malloced_symtab_when_hashtable_create_succeeds(void **state) {
    alloc_and_save_address_to_be_freed((void **)&fake_malloc_returned_value_for_a_symtab, sizeof(struct symtab));
    alloc_and_save_address_to_be_freed((void **)&fake_hashtable_create_returned_value, DUMMY_SIZE_STRUCT_HASHTABLE);

    expect_value(mock_malloc, size, sizeof(symtab));
    will_return(mock_malloc, fake_malloc_returned_value_for_a_symtab);
    expect_value(hashtable_create, size, SYMTAB_SIZE);
    expect_value(hashtable_create, destroy_value_fn, symtab_destroy_symbol);
    will_return(hashtable_create, fake_hashtable_create_returned_value);

    symtab *ret = symtab_wind_scope((symtab *) DUMMY_SYMTAB_P);

    assert_ptr_equal(ret, fake_malloc_returned_value_for_a_symtab);
    assert_ptr_equal(ret->symbols, fake_hashtable_create_returned_value);
    assert_ptr_equal(ret->parent, DUMMY_SYMTAB_P);
}




//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest destroy_symbol_tests[] = {
        cmocka_unit_test_setup_teardown(
            destroy_symbol_do_nothing_when_symbol_null,
            destroy_symbol_setup, destroy_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            destroy_symbol_frees_name_and_symbol_when_image_null,
            destroy_symbol_setup, destroy_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            destroy_symbol_calls_ast_destroy_non_typed_data_wrapper_when_image_is_not_data_wrapper,
            destroy_symbol_setup, destroy_symbol_teardown),
    };

    const struct CMUnitTest wind_scope_tests[] = {
        cmocka_unit_test_setup_teardown(
            wind_scope_calls_malloc_for_a_symtab,
            wind_scope_setup, wind_scope_teardown),
        cmocka_unit_test_setup_teardown(
            wind_scope_returns_null_when_malloc_fails,
            wind_scope_setup, wind_scope_teardown),
        cmocka_unit_test_setup_teardown(
            wind_scope_calls_hashtable_create_when_malloc_succeds,
            wind_scope_setup, wind_scope_teardown),
        cmocka_unit_test_setup_teardown(
            wind_scope_returns_null_when_hashtable_create_fails,
            wind_scope_setup, wind_scope_teardown),
        cmocka_unit_test_setup_teardown(
            wind_scope_calls_initializes_and_returns_malloced_symtab_when_hashtable_create_succeeds,
            wind_scope_setup, wind_scope_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(destroy_symbol_tests, NULL, NULL);
    failed += cmocka_run_group_tests(wind_scope_tests, NULL, NULL);

    return failed;
}

