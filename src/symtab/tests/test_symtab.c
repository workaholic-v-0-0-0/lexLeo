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
#include "internal/hashtable_test_utils.h"


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
static const symbol *const DUMMY_SYMBOL_P = (symbol *) &DUMMY[7];
static const int DUMMY_INT = 0;
static const ast *const DUMMY_IMAGE = (ast *) &DUMMY[8];
static const void *const DUMMY_VOID_POINTER = (void *) &DUMMY[9];

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

hashtable *mock_hashtable_create(size_t size, hashtable_destroy_value_fn_t destroy_value_fn) {
    check_expected(size);
    check_expected(destroy_value_fn);
    return mock_type(hashtable *);
}

static hashtable *fake_hashtable_create_returned_value;

void mock_hashtable_destroy(hashtable *ht) {
    check_expected(ht);
}

int mock_hashtable_add(hashtable *ht, const char *key, void *value) {
    check_expected(ht);
    check_expected(key);
    check_expected(value);
    return mock_type(int);
}

void *mock_hashtable_get(const hashtable *ht, const char *key) {
    check_expected(ht);
    check_expected(key);
    return mock_type(void *);
}

int mock_hashtable_reset_value(hashtable *ht, const char *key, void *value) {
    check_expected(ht);
    check_expected(key);
    check_expected(value);
    return mock_type(int);
}

int mock_hashtable_remove(hashtable *ht, const char *key) {
    check_expected(ht);
    check_expected(key);
    return mock_type(int);
}

int mock_hashtable_key_is_in_use(hashtable *ht, const char *key) {
    check_expected(ht);
    check_expected(key);
    return mock_type(int);
}



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
    set_hashtable_create(mock_hashtable_create);
    return 0;
}

static int wind_scope_teardown(void **state) {
    set_allocators(NULL, NULL);
    set_hashtable_create(NULL);
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
    expect_value(mock_hashtable_create, size, SYMTAB_SIZE);
    expect_value(mock_hashtable_create, destroy_value_fn, symtab_destroy_symbol);

    // finish a scenario tested further to avoid segmentation fault
    will_return(mock_hashtable_create, NULL);
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
    expect_value(mock_hashtable_create, size, SYMTAB_SIZE);
    expect_value(mock_hashtable_create, destroy_value_fn, symtab_destroy_symbol);
    will_return(mock_hashtable_create, NULL);
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
    expect_value(mock_hashtable_create, size, SYMTAB_SIZE);
    expect_value(mock_hashtable_create, destroy_value_fn, symtab_destroy_symbol);
    will_return(mock_hashtable_create, fake_hashtable_create_returned_value);

    symtab *ret = symtab_wind_scope((symtab *) DUMMY_SYMTAB_P);

    assert_ptr_equal(ret, fake_malloc_returned_value_for_a_symtab);
    assert_ptr_equal(ret->symbols, fake_hashtable_create_returned_value);
    assert_ptr_equal(ret->parent, DUMMY_SYMTAB_P);
}



//-----------------------------------------------------------------------------
// symtab_unwind_scope TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int unwind_scope_setup(void **state) {
    set_allocators(mock_malloc, mock_free);
    set_hashtable_destroy(mock_hashtable_destroy);
    return 0;
}

static int unwind_scope_teardown(void **state) {
    set_allocators(NULL, NULL);
    set_hashtable_destroy(NULL);
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: st = NULL
// Expected: do nothing and returns NULL
static void unwind_scope_do_nothing_and_returns_null_when_st_null(void **state) {
    assert_null(symtab_unwind_scope(NULL));
}

// Given: st != NULL
// Expected:
//  - calls hashtable_destroy with st->symbols
//  - frees st
//  - returns st->parent
static void unwind_scope_calls_hashtable_destroy_frees_st_and_returns_parent_when_st_not_null(void **state) {
    symtab *st = NULL;
    alloc_and_save_address_to_be_freed((void **)&st, sizeof(symtab));
    st->symbols = (hashtable *) DUMMY_HASHTABLE_P;
    st->parent = (symtab *) DUMMY_SYMTAB_P;

    expect_value(mock_hashtable_destroy, ht, st->symbols);
    expect_value(mock_free, ptr, st);

    symtab *ret = symtab_unwind_scope(st);

    assert_ptr_equal(ret, st->parent);
}



//-----------------------------------------------------------------------------
// symtab_add TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int add_setup(void **state) {
    set_hashtable_add(mock_hashtable_add);
    return 0;
}

static int add_teardown(void **state) {
    set_hashtable_add(NULL);
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: st = NULL
// Expected: returns 1
static void add_returns_1_when_st_null(void **state) {
    assert_int_equal(1, symtab_add(NULL, (symbol *) DUMMY_SYMBOL_P));
}

// Given: st != NULL, sym = NULL
// Expected: returns 1
static void add_returns_1_when_st_not_null_sym_null(void **state) {
    assert_int_equal(1, symtab_add((symtab *) DUMMY_SYMTAB_P, NULL));
}

// Given: st != NULL, sym != NULL
// Expected:
//  - calls hashtable_add(st->symbols, sym->name, (void *) sym)
//  - returns hashtable_add returned value
static void add_calls_hashtable_add_and_returns_its_returned_value_when_st_not_null_sym_not_null(void **state) {
    symtab *st = NULL;
    symbol *sym = NULL;
    alloc_and_save_address_to_be_freed((void **)&st, sizeof(symtab));
    alloc_and_save_address_to_be_freed((void **)&sym, sizeof(symbol));
    st->symbols = (hashtable *) DUMMY_HASHTABLE_P;
    st->parent = (symtab *) DUMMY_SYMTAB_P;
    sym->name = (char *) DUMMY_STRING;
    sym->image = (ast *) DUMMY_IMAGE;

    expect_value(mock_hashtable_add, ht, st->symbols);
    expect_value(mock_hashtable_add, key, sym->name);
    expect_value(mock_hashtable_add, value, (void *) sym);
    will_return(mock_hashtable_add, DUMMY_INT);

    assert_int_equal(symtab_add(st, sym), DUMMY_INT);
}



//-----------------------------------------------------------------------------
// symtab_get_local TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int get_setup(void **state) {
    set_hashtable_get(mock_hashtable_get);
    return 0;
}

static int get_teardown(void **state) {
    set_hashtable_get(NULL);
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: st = NULL
// Expected: returns 1
static void get_returns_null_when_st_null(void **state) {
    assert_null(symtab_get_local(NULL, (char *) DUMMY_STRING));
}

// Given: st != NULL
// Expected:
//  - calls hashtable_get(st->symbols, name)
//  - returns hashtable_get returned value
static void get_calls_hashtable_get_and_returns_its_returned_value_when_st_not_null(void **state) {
    symtab *st = NULL;
    alloc_and_save_address_to_be_freed((void **)&st, sizeof(symtab));
    st->symbols = (hashtable *) DUMMY_HASHTABLE_P;
    st->parent = (symtab *) DUMMY_SYMTAB_P;

    expect_value(mock_hashtable_get, ht, st->symbols);
    expect_value(mock_hashtable_get, key, DUMMY_STRING);
    will_return(mock_hashtable_get, DUMMY_VOID_POINTER);

    assert_ptr_equal(symtab_get_local(st, (char *) DUMMY_STRING), DUMMY_VOID_POINTER);
}



//-----------------------------------------------------------------------------
// symtab_reset_local TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int reset_setup(void **state) {
    set_hashtable_reset_value(mock_hashtable_reset_value);
    return 0;
}

static int reset_teardown(void **state) {
    set_hashtable_reset_value(NULL);
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: st = NULL
// Expected: returns 1
static void reset_returns_1_when_st_null(void **state) {
    assert_int_equal(
        symtab_reset_local(NULL, (char *) DUMMY_STRING, (ast *) DUMMY_IMAGE),
        1 );
}

// Given: st != NULL, image != NULL
// Expected:
//   - calls hashtable_reset_value(st->symbols, name, (void *) image)
//   - returns hashtable_reset_value returned value
static void reset_calls_hashtable_reset_value_and_returns_its_returned_value_when_st_not_null_image_not_null(void **state) {
    symtab *st = NULL;
    alloc_and_save_address_to_be_freed((void **)&st, sizeof(symtab));
    st->symbols = (hashtable *) DUMMY_HASHTABLE_P;
    st->parent = (symtab *) DUMMY_SYMTAB_P;

    expect_value(mock_hashtable_reset_value, ht, st->symbols);
    expect_value(mock_hashtable_reset_value, key, (char *) DUMMY_STRING);
    expect_value(mock_hashtable_reset_value, value, (void *) DUMMY_IMAGE);
    will_return(mock_hashtable_reset_value, DUMMY_INT);

    assert_int_equal(
        symtab_reset_local((symtab *) st, (char *) DUMMY_STRING, (ast *) DUMMY_IMAGE),
        DUMMY_INT );
}



//-----------------------------------------------------------------------------
// symtab_remove TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int remove_setup(void **state) {
    set_hashtable_remove(mock_hashtable_remove);
    return 0;
}

static int remove_teardown(void **state) {
    set_hashtable_remove(NULL);
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: st = NULL
// Expected: returns 1
static void remove_returns_1_when_st_null(void **state) {
    assert_int_equal(
        symtab_remove(NULL, (char *) DUMMY_STRING),
        1 );
}

// Given: st != NULL
// Expected:
//  - calls hashtable_remove(st->symbols, name)
//  - returns hashtable_remove returned value
static void remove_calls_hashtable_remove_and_returns_its_returned_value_when_st_not_null_name_not_null(void **state) {
    symtab *st = NULL;
    alloc_and_save_address_to_be_freed((void **)&st, sizeof(symtab));
    st->symbols = (hashtable *) DUMMY_HASHTABLE_P;
    st->parent = (symtab *) DUMMY_SYMTAB_P;

    expect_value(mock_hashtable_remove, ht, st->symbols);
    expect_value(mock_hashtable_remove, key, (char *) DUMMY_STRING);
    will_return(mock_hashtable_remove, DUMMY_INT);

    assert_int_equal(
        symtab_remove(st, (char *) DUMMY_STRING),
        DUMMY_INT );
}



//-----------------------------------------------------------------------------
// symtab_contains_local TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int contains_setup(void **state) {
    set_hashtable_key_is_in_use(mock_hashtable_key_is_in_use);
    return 0;
}

static int contains_teardown(void **state) {
    set_hashtable_key_is_in_use(NULL);
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: st = NULL
// Expected: returns 0
static void contains_returns_0_when_st_null(void **state) {
    assert_int_equal(
        symtab_contains_local(NULL, (char *) DUMMY_STRING),
        0 );
}

// Given: st != NULL
// Expected:
//  - calls hashtable_key_is_in_use(st->symbols, name)
//  - returns hashtable_key_is_in_use
static void contains_calls_hashtable_key_is_in_use_when_st_not_null(void **state) {
    symtab *st = NULL;
    alloc_and_save_address_to_be_freed((void **)&st, sizeof(symtab));
    st->symbols = (hashtable *) DUMMY_HASHTABLE_P;
    st->parent = (symtab *) DUMMY_SYMTAB_P;

    expect_value(mock_hashtable_key_is_in_use, ht, st->symbols);
    expect_value(mock_hashtable_key_is_in_use, key, (char *) DUMMY_STRING);
    will_return(mock_hashtable_key_is_in_use, DUMMY_INT);

    assert_int_equal(
        symtab_contains_local(st, (char *) DUMMY_STRING),
        DUMMY_INT );
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

    const struct CMUnitTest unwind_scope_tests[] = {
        cmocka_unit_test_setup_teardown(
            unwind_scope_do_nothing_and_returns_null_when_st_null,
            unwind_scope_setup, unwind_scope_teardown),
        cmocka_unit_test_setup_teardown(
            unwind_scope_calls_hashtable_destroy_frees_st_and_returns_parent_when_st_not_null,
            unwind_scope_setup, unwind_scope_teardown),
    };

    const struct CMUnitTest add_tests[] = {
        cmocka_unit_test_setup_teardown(
            add_returns_1_when_st_null,
            add_setup, add_teardown),
        cmocka_unit_test_setup_teardown(
            add_returns_1_when_st_not_null_sym_null,
            add_setup, add_teardown),
        cmocka_unit_test_setup_teardown(
            add_calls_hashtable_add_and_returns_its_returned_value_when_st_not_null_sym_not_null,
            add_setup, add_teardown),
    };

    const struct CMUnitTest get_tests[] = {
        cmocka_unit_test_setup_teardown(
            get_returns_null_when_st_null,
            get_setup, get_teardown),
        cmocka_unit_test_setup_teardown(
            get_calls_hashtable_get_and_returns_its_returned_value_when_st_not_null,
            get_setup, get_teardown),
    };

    const struct CMUnitTest reset_tests[] = {
        cmocka_unit_test_setup_teardown(
            reset_returns_1_when_st_null,
            reset_setup, reset_teardown),
        cmocka_unit_test_setup_teardown(
            reset_calls_hashtable_reset_value_and_returns_its_returned_value_when_st_not_null_image_not_null,
            reset_setup, reset_teardown),
    };

    const struct CMUnitTest remove_tests[] = {
        cmocka_unit_test_setup_teardown(
            remove_returns_1_when_st_null,
            remove_setup, remove_teardown),
        cmocka_unit_test_setup_teardown(
            remove_calls_hashtable_remove_and_returns_its_returned_value_when_st_not_null_name_not_null,
            remove_setup, remove_teardown),
    };

    const struct CMUnitTest contains_tests[] = {
        cmocka_unit_test_setup_teardown(
            contains_returns_0_when_st_null,
            contains_setup, contains_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(destroy_symbol_tests, NULL, NULL);
    failed += cmocka_run_group_tests(wind_scope_tests, NULL, NULL);
    failed += cmocka_run_group_tests(unwind_scope_tests, NULL, NULL);
    failed += cmocka_run_group_tests(add_tests, NULL, NULL);
    failed += cmocka_run_group_tests(get_tests, NULL, NULL);
    failed += cmocka_run_group_tests(reset_tests, NULL, NULL);
    failed += cmocka_run_group_tests(remove_tests, NULL, NULL);
    failed += cmocka_run_group_tests(contains_tests, NULL, NULL);

    return failed;
}

