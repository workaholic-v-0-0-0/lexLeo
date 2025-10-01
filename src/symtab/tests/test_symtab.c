// src/symtab/tests/test_symtab.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "memory_allocator.h"
#include "string_utils.h"
#include "logger.h"
#include "list.h"
#include "ast.h"

#include "internal/symtab_internal.h"
#include "internal/symtab_test_utils.h"
#include "internal/hashtable_test_utils.h"
#include "internal/list_test_utils.h"



//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static const char DUMMY[12];
static const void *const DUMMY_MALLOC_RETURNED_VALUE = (void *) &DUMMY[0];
static const char *const DUMMY_STRING = (char *) &DUMMY[1];
#define MALLOC_ERROR_CODE NULL
static ast *const DUMMY_AST_P = (ast *) &DUMMY[2];
static const ast_children_t *const DUMMY_CHILDREN_INFO_P = (ast_children_t *) &DUMMY[3];
static const typed_data *const DUMMY_TYPED_DATA_P = (typed_data *) &DUMMY[4];
static const hashtable *const DUMMY_HASHTABLE_P = (hashtable *) &DUMMY[5];
static const size_t DUMMY_SIZE_STRUCT_HASHTABLE = 1;
static const symtab *const DUMMY_SYMTAB_P = (symtab *) &DUMMY[6];
static const symbol *const DUMMY_SYMBOL_P = (symbol *) &DUMMY[7];
static const int DUMMY_INT = 0;
static const ast *const DUMMY_IMAGE = (ast *) &DUMMY[8];
static const void *const DUMMY_VOID_POINTER = (void *) &DUMMY[9];
static char too_long_symbol_name[MAXIMUM_SYMBOL_NAME_LENGTH +2];
static char *valid_symbol_name = "valid_symbol_name";
static const void *DUMMY_STRDUP_RETURNED_VALUE = (void *) &DUMMY[10];
#define STRDUP_ERROR_CODE NULL
static const list *DUMMY_SYMBOL_POOL = (void *) &DUMMY[11];

static list collected_ptr_to_be_freed = NULL;

symtab_destroy_value_fn_t symtab_destroy_symbol;



//-----------------------------------------------------------------------------
// MOCKS, STUBS, SPIES AND FAKES
//-----------------------------------------------------------------------------


void * mock_malloc(size_t size) {
    check_expected(size);
    return mock_type(void *);
}

static void *fake_malloc_returned_value_for_a_symtab;
static void *fake_malloc_returned_value_for_a_symbol;

void mock_free(void *ptr) {
    check_expected_ptr(ptr);
}

void ast_destroy_typed_data_wrapper(ast *ast_data_wrapper) {
    check_expected(ast_data_wrapper);
}

void ast_destroy_children_node(ast *children_node) {
    check_expected(children_node);
}

hashtable *mock_hashtable_create(size_t size, hashtable_key_type key_type, hashtable_destroy_value_fn_t destroy_value_fn) {
    check_expected(size);
    check_expected(key_type);
    check_expected(destroy_value_fn);
    return mock_type(hashtable *);
}

static hashtable *fake_hashtable_create_returned_value;

void mock_hashtable_destroy(hashtable *ht) {
    check_expected(ht);
}

int mock_hashtable_add(hashtable *ht, const void *key, void *value) {
    check_expected(ht);
    check_expected(key);
    check_expected(value);
    return mock_type(int);
}

void *mock_hashtable_get(const hashtable *ht, const void *key) {
    check_expected(ht);
    check_expected(key);
    return mock_type(void *);
}

int mock_hashtable_remove(hashtable *ht, const void *key) {
    check_expected(ht);
    check_expected(key);
    return mock_type(int);
}

int mock_hashtable_key_is_in_use(hashtable *ht, const void *key) {
    check_expected(ht);
    check_expected(key);
    return mock_type(int);
}

symbol *mock_symtab_get_local(symtab *st, const char *name) {
    check_expected(st);
    check_expected(name);
    return mock_type(symbol *);
}

int mock_symtab_contains_local(symtab *st, const char *name) {
    check_expected(st);
    check_expected(name);
    return mock_type(int);
}

symbol *mock_symtab_get(symtab *st, const char *name) {
    check_expected(st);
    check_expected(name);
    return mock_type(symbol *);
}

static symtab_get_fn next_symtab_get = real_symtab_get;

// first call is real, second is mock, then repeats
symbol *spy_symtab_get(symtab *st, const char *name) {
    symtab_get_fn current_symtab_get = next_symtab_get;
    next_symtab_get = (next_symtab_get == mock_symtab_get) ? real_symtab_get : mock_symtab_get;
    return current_symtab_get(st, name);
}

int mock_symtab_contains(symtab *st, const char *name) {
    check_expected(st);
    check_expected(name);
    return mock_type(int);
}

static symtab_contains_fn next_symtab_contains = real_symtab_contains;

// first call is real, second is mock, then repeats
int spy_symtab_contains(symtab *st, const char *name) {
    symtab_contains_fn current_symtab_contains = next_symtab_contains;
    next_symtab_contains = (next_symtab_contains == mock_symtab_contains) ? real_symtab_contains : mock_symtab_contains;
    return current_symtab_contains(st, name);
}

char *mock_strdup(const char *s) {
    check_expected_ptr(s);
    return mock_type(char *);
}

static char *fake_strdup_returned_value_for_symbol_name;

static hashtable dummy_ht;
static symtab stub_st;

static void init_symtab_stub(void) {
    memset(&dummy_ht, 0, sizeof dummy_ht);
    memset(&stub_st, 0, sizeof stub_st);
    stub_st.symbols = &dummy_ht;   // ou stub_st.pool = &dummy_ht selon ton design
    stub_st.parent  = NULL;
}

static hashtable stub_ht;
static symtab stub_symtab = {.symbols = &stub_ht, .parent = NULL};
static void *stub_malloc_returned_value_for_symbol = (void *) DUMMY_SYMBOL_P;
static char *malloc_ret_block_for_symbol;
static char *strdup_ret_block;

list mock_list_push(list l, void * e) {
    check_expected(l);
    check_expected(e);
    return mock_type(list);
}

void *mock_list_pop(list *l_p) {
    check_expected(l_p);
    return mock_type(void *);
}



//-----------------------------------------------------------------------------
// GENERAL HELPERS
//-----------------------------------------------------------------------------


static void alloc_and_save_address_to_be_freed(void **ptr, size_t size) {
    *ptr = malloc(size);
    assert_non_null(*ptr);
    set_allocators(NULL, NULL);
    list_push_fn f = get_list_push();
    set_list_push(NULL);
    collected_ptr_to_be_freed = list_push(collected_ptr_to_be_freed, *ptr);
    set_list_push(f);
    set_allocators(mock_malloc, mock_free);
}

static void free_saved_addresses_to_be_freed(void) {
    while (collected_ptr_to_be_freed) {
        list next = collected_ptr_to_be_freed->cdr;
        if (collected_ptr_to_be_freed->car)
            free(collected_ptr_to_be_freed->car);
        free(collected_ptr_to_be_freed);
        collected_ptr_to_be_freed = next;
    }
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
    free_saved_addresses_to_be_freed();
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
    expect_value(mock_hashtable_create, key_type, HASHTABLE_KEY_TYPE_STRING);
    expect_value(mock_hashtable_create, destroy_value_fn, NULL);

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
    expect_value(mock_hashtable_create, key_type, HASHTABLE_KEY_TYPE_STRING);
    expect_value(mock_hashtable_create, destroy_value_fn, NULL);
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
    expect_value(mock_hashtable_create, key_type, HASHTABLE_KEY_TYPE_STRING);
    expect_value(mock_hashtable_create, destroy_value_fn, NULL);
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
    free_saved_addresses_to_be_freed();
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
// symtab_get_local TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int get_local_setup(void **state) {
    set_hashtable_get(mock_hashtable_get);
    return 0;
}

static int get_local_teardown(void **state) {
    set_hashtable_get(NULL);
    free_saved_addresses_to_be_freed();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: st = NULL
// Expected: returns NULL
static void get_local_returns_null_when_st_null(void **state) {
    assert_null(symtab_get_local(NULL, (char *) DUMMY_STRING));
}

// Given: st != NULL
// Expected:
//  - calls hashtable_get(st->symbols, name)
//  - returns hashtable_get returned value
static void get_local_calls_hashtable_get_and_returns_its_returned_value_when_st_not_null(void **state) {
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
    free_saved_addresses_to_be_freed();
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


static int contains_local_setup(void **state) {
    set_hashtable_key_is_in_use(mock_hashtable_key_is_in_use);
    return 0;
}

static int contains_local_teardown(void **state) {
    set_hashtable_key_is_in_use(NULL);
    free_saved_addresses_to_be_freed();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: st = NULL
// Expected: returns 0
static void contains_local_returns_0_when_st_null(void **state) {
    assert_int_equal(
        symtab_contains_local(NULL, (char *) DUMMY_STRING),
        0 );
}

// Given: st != NULL
// Given: st != NULL
// Expected:
//  - calls hashtable_key_is_in_use(st->symbols, name)
//  - returns hashtable_key_is_in_use
static void contains_local_calls_hashtable_key_is_in_use_when_st_not_null(void **state) {
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
// symtab_get TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int get_setup(void **state) {
    set_symtab_contains_local(mock_symtab_contains_local);
    set_symtab_get_local(mock_symtab_get_local);
    set_symtab_get(spy_symtab_get);
    next_symtab_get = real_symtab_get;
    return 0;
}

static int get_teardown(void **state) {
    set_symtab_get_local(NULL);
    set_symtab_contains_local(NULL);
    set_symtab_get(NULL);
    free_saved_addresses_to_be_freed();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: st = NULL
// Expected: return NULL
static void get_returns_null_when_st_null(void **state) {
    assert_null(symtab_get(NULL, DUMMY_STRING));
}

// Given: st != NULL
// Expected: calls symtab_contains_local(st, name);
static void get_calls_symtab_contains_local_when_st_not_null(void **state) {
    expect_value(mock_symtab_contains_local, st, DUMMY_SYMTAB_P);
    expect_value(mock_symtab_contains_local, name, DUMMY_STRING);
    will_return(mock_symtab_contains_local, true); // see next tests
    expect_value(mock_symtab_get_local, st, DUMMY_SYMTAB_P); // see next tests
    expect_value(mock_symtab_get_local, name, DUMMY_STRING); // see next tests
    will_return(mock_symtab_get_local, DUMMY_SYMBOL_P); // see next tests
    symtab_get((symtab *) DUMMY_SYMTAB_P, DUMMY_STRING);
}

// Given: symtab_contains_local(st, name) returns true
// Expected:
//  - calls symtab_get_local(st, name);
//  - returns symtab_get_local returned value
static void get_calls_symtab_get_local_and_returns_its_returned_value_when_symtab_contains_local_returns_true(void **state) {
    expect_value(mock_symtab_contains_local, st, DUMMY_SYMTAB_P);
    expect_value(mock_symtab_contains_local, name, DUMMY_STRING);
    will_return(mock_symtab_contains_local, true);
    expect_value(mock_symtab_get_local, st, DUMMY_SYMTAB_P);
    expect_value(mock_symtab_get_local, name, DUMMY_STRING);
    will_return(mock_symtab_get_local, DUMMY_SYMBOL_P);
    assert_ptr_equal(
        symtab_get((symtab *) DUMMY_SYMTAB_P, DUMMY_STRING),
        DUMMY_SYMBOL_P
    );
}

// Given: symtab_contains_local(st, name) returns false
// Expected:
//  - calls symtab_get(st->parent, name);
//  - returns symtab_get returned value
static void get_calls_itself_and_returns_value_when_symtab_contains_local_returns_false(void **state) {
    symtab *st = NULL;
    alloc_and_save_address_to_be_freed((void **)&st, sizeof(symtab));
    st->symbols = (hashtable *) DUMMY_HASHTABLE_P;
    st->parent = (symtab *) DUMMY_SYMTAB_P;
    expect_value(mock_symtab_contains_local, st, st);
    expect_value(mock_symtab_contains_local, name, DUMMY_STRING);
    will_return(mock_symtab_contains_local, false);
    expect_value(mock_symtab_get, st, st->parent);
    expect_value(mock_symtab_get, name, DUMMY_STRING);
    will_return(mock_symtab_get, DUMMY_SYMBOL_P);
    assert_ptr_equal(
        symtab_get(st, DUMMY_STRING),
        DUMMY_SYMBOL_P
    );
}



//-----------------------------------------------------------------------------
// symtab_contains TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int contains_setup(void **state) {
    set_symtab_contains_local(mock_symtab_contains_local);
    set_symtab_contains(spy_symtab_contains);
    next_symtab_contains = real_symtab_contains;
    return 0;
}

static int contains_teardown(void **state) {
    set_symtab_contains_local(NULL);
    set_symtab_contains(NULL);
    free_saved_addresses_to_be_freed();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: st = NULL
// Expected: returns 0
static void contains_returns_0_when_st_null(void **state) {
    assert_int_equal(0, symtab_contains(NULL, DUMMY_STRING));
}

// Given: st != NULL
// Expected: calls symtab_contains_local(st, name)
static void contains_calls_symtab_contains_local_when_st_not_null(void **state) {
    expect_value(mock_symtab_contains_local, st, DUMMY_SYMTAB_P);
    expect_value(mock_symtab_contains_local, name, DUMMY_STRING);
    will_return(mock_symtab_contains_local, true); // to avoid some mock calls
    symtab_contains((symtab *) DUMMY_SYMTAB_P, DUMMY_STRING);
}

// Given: symtab_contains_local(st, name) returns 1
// Expected: returns 1
static void contains_returns_1_when_symtab_contains_local_returns_1(void **state) {
    expect_value(mock_symtab_contains_local, st, DUMMY_SYMTAB_P);
    expect_value(mock_symtab_contains_local, name, DUMMY_STRING);
    will_return(mock_symtab_contains_local, true);
    assert_int_equal(1, symtab_contains((symtab *) DUMMY_SYMTAB_P, DUMMY_STRING));
}

// Given: symtab_contains_local(st, name) returns 0
// Expected:
//  - calls symtab_contains(st->parent, name);
//  - returns symtab_contains returned value
static void contains_calls_symtab_contains_and_returns_value_when_symtab_contains_local_returns_0(void **state) {
    symtab *st = NULL;
    alloc_and_save_address_to_be_freed((void **)&st, sizeof(symtab));
    st->symbols = (hashtable *) DUMMY_HASHTABLE_P;
    st->parent = (symtab *) DUMMY_SYMTAB_P;
    expect_value(mock_symtab_contains_local, st, st);
    expect_value(mock_symtab_contains_local, name, DUMMY_STRING);
    will_return(mock_symtab_contains_local, false);
    expect_value(mock_symtab_contains, st, st->parent);
    expect_value(mock_symtab_contains, name, DUMMY_STRING);
    will_return(mock_symtab_contains, DUMMY_INT);
    assert_int_equal(
        symtab_contains(st, DUMMY_STRING),
        DUMMY_INT );
}



//-----------------------------------------------------------------------------
// symtab_intern_symbol TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// symtab_intern_symbol

// doubled:{

//  - hashtable_add (mock)
//  - hashtable_key_is_in_use (mock)
//  - malloc, free, strdup  (mock)
//  - list_push (mock)
//  - list_pop (mock)
//  - symtab *st param (stub)



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int intern_symbol_setup(void **state) {
	memset(too_long_symbol_name, 'a', MAXIMUM_SYMBOL_NAME_LENGTH + 1);
	too_long_symbol_name[256] = '\0';
    memset(&stub_ht, 0, sizeof stub_ht);
    memset(&stub_symtab, 0, sizeof stub_symtab);
    stub_symtab.symbols = &stub_ht;
    stub_symtab.parent  = NULL;
    set_allocators(mock_malloc, mock_free);
	set_string_duplicate(mock_strdup);
    set_hashtable_key_is_in_use(mock_hashtable_key_is_in_use);
    set_hashtable_add(mock_hashtable_add);
    set_list_push(mock_list_push);
    set_list_pop(mock_list_pop);
    return 0;
}

static int intern_symbol_teardown(void **state) {
    set_allocators(NULL, NULL);
	set_string_duplicate(NULL);
    set_hashtable_key_is_in_use(NULL);
    set_list_push(NULL);
	set_list_pop(NULL);
    set_hashtable_add(NULL);
    free_saved_addresses_to_be_freed();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - st == NULL
// Expected:
//  - returns 1
static void intern_symbol_error_when_st_null(void **state) {
    assert_int_equal(
        symtab_intern_symbol(NULL, valid_symbol_name),
        1 );
}

// Given:
//  - name == NULL
// Expected:
//  - returns 1
static void intern_symbol_error_when_name_null(void **state) {
    assert_int_equal(
        symtab_intern_symbol(&stub_symtab, NULL),
        1 );
}

// Given:
//  - name length exceeds MAXIMUM_SYMBOL_NAME_LENGTH
// Expected:
//  - returns 1
static void intern_symbol_error_when_name_too_long(void **state) {
    assert_int_equal(
        symtab_intern_symbol(&stub_symtab, too_long_symbol_name),
        1 );
}

// Given:
//  - st->symbols == NULL
// Expected:
//  - returns 1
static void intern_symbol_error_when_symbols_field_null(void **state) {
    (&stub_symtab)->symbols = NULL;
    assert_int_equal(
        symtab_intern_symbol(&stub_symtab, NULL),
        1 );
}

// Given:
//  - arguments are valid
//  - hashtable_key_is_in_use will return true
// Expected:
//  - calls hashtable_key_is_in_use with:
//    - ht: st->symbols
//    - key: name
//  - returns 0
static void intern_symbol_do_nothing_and_returns_0_when_symbol_already_interned(void **state) {
    expect_value(mock_hashtable_key_is_in_use, ht, (&stub_symtab)->symbols);
    expect_value(mock_hashtable_key_is_in_use, key, valid_symbol_name);
    will_return(mock_hashtable_key_is_in_use, true);

    assert_int_equal(
        symtab_intern_symbol(&stub_symtab, valid_symbol_name),
        0 );
}

// Given:
//  - arguments are valid
//  - hashtable_key_is_in_use will return false (which means the key is not in use because arguments are valid!)
//  - allocation for symbol will fail
// Expected:
//  - calls hashtable_key_is_in_use with:
//    - ht: st->symbols
//    - key: name
//  - calls malloc with:
//    - size: sizeof(symbol)
//  - returns 1
static void intern_symbol_error_when_malloc_for_symbol_fails(void **state) {
    expect_value(mock_hashtable_key_is_in_use, ht, (&stub_symtab)->symbols);
    expect_value(mock_hashtable_key_is_in_use, key, valid_symbol_name);
    will_return(mock_hashtable_key_is_in_use, false);
    expect_value(mock_malloc, size, sizeof(symbol));
    will_return(mock_malloc, NULL);

    assert_int_equal(
        symtab_intern_symbol(&stub_symtab, valid_symbol_name),
        1 );
}

// Given:
//  - arguments are valid
//  - hashtable_key_is_in_use will return false
//  - allocation for symbol will succeed
//  - duplication of name will fail
// Expected:
//  - calls hashtable_key_is_in_use with:
//    - ht: st->symbols
//    - key: name
//  - calls malloc with:
//    - size: sizeof(symbol)
//  - calls strdup with:
//    - s: name
//  - calls free with:
//    - ptr: malloc returned value for symbol
//  - returns 1
static void intern_symbol_cleanup_and_error_when_strdup_fails(void **state) {
    expect_value(mock_hashtable_key_is_in_use, ht, (&stub_symtab)->symbols);
    expect_string(mock_hashtable_key_is_in_use, key, valid_symbol_name);
    will_return(mock_hashtable_key_is_in_use, false);
    alloc_and_save_address_to_be_freed((void **)&malloc_ret_block_for_symbol, sizeof(symbol));
    expect_value(mock_malloc, size, sizeof(symbol));
    will_return(mock_malloc, malloc_ret_block_for_symbol);
    expect_value(mock_strdup, s, valid_symbol_name);
    will_return(mock_strdup, NULL);
    expect_value(mock_free, ptr, malloc_ret_block_for_symbol);

    assert_int_equal(
        symtab_intern_symbol(&stub_symtab, valid_symbol_name),
        1 );
}

// Given:
//  - arguments are valid
//  - hashtable_key_is_in_use will return false
//  - allocation for symbol will succeed
//  - duplication of name will succeed
//  - list_push for symbol registration in symbol_pool will fail
// Expected:
//  - calls hashtable_key_is_in_use with:
//    - ht: st->symbols
//    - key: name
//  - calls malloc with:
//    - size: sizeof(symbol)
//  - calls strdup with:
//    - s: name
//  - call list_push with:
//    - l: symbol_pool
//    - e: malloc returned value for symbol
//  - calls free with:
//    - ptr: strdup returned value
//  - calls free with:
//    - ptr: malloc returned value for symbol
//  - returns 1
static void intern_symbol_cleanup_and_error_when_list_push_fails(void **state) {
    expect_value(mock_hashtable_key_is_in_use, ht, stub_symtab.symbols);
    expect_string(mock_hashtable_key_is_in_use, key, valid_symbol_name);
    will_return(mock_hashtable_key_is_in_use, false);
    alloc_and_save_address_to_be_freed((void **)&malloc_ret_block_for_symbol, sizeof(symbol));
    expect_value(mock_malloc, size, sizeof(symbol));
    will_return(mock_malloc, malloc_ret_block_for_symbol);
    alloc_and_save_address_to_be_freed((void **)&strdup_ret_block, sizeof(char) * (strlen(valid_symbol_name)+1));
    expect_value(mock_strdup, s, valid_symbol_name);
    will_return(mock_strdup, strdup_ret_block);
    expect_value(mock_list_push, l, get_symbol_pool());
    expect_value(mock_list_push, e, malloc_ret_block_for_symbol);
    will_return(mock_list_push, NULL);
    expect_value(mock_free, ptr, strdup_ret_block);
    expect_value(mock_free, ptr, malloc_ret_block_for_symbol);

    assert_int_equal(
        symtab_intern_symbol(&stub_symtab, valid_symbol_name),
        1 );
}

// Given:
//  - arguments are valid
//  - hashtable_key_is_in_use will return false
//  - allocation for symbol will succeed
//  - duplication of name will succeed
//  - list_push will succeed
//  - hashtable_add will fail
// Expected:
//  - calls hashtable_key_is_in_use with:
//    - ht: st->symbols
//    - key: name
//  - calls malloc with:
//    - size: sizeof(symbol)
//  - calls strdup with:
//    - s: name
//  - call list_push with:
//    - l: symbol_pool
//    - e: malloc returned value for symbol
//  - calls hashtable_add:
//    - ht: st->symbols
//    - key: name
//    - value: malloc returned value for symbol
//  - calls free with:
//    - ptr: malloc returned value for the duplicated string
//  - calls free with:
//    - ptr: malloc returned value for symbol
//  - calls list_pop with:
//    - &symbol_pool
//  - returns 1
static void intern_symbol_cleanup_and_error_when_hashtable_add_fails(void **state) {
    expect_value(mock_hashtable_key_is_in_use, ht, stub_symtab.symbols);
    expect_string(mock_hashtable_key_is_in_use, key, valid_symbol_name);
    will_return(mock_hashtable_key_is_in_use, false);
    alloc_and_save_address_to_be_freed((void **)&malloc_ret_block_for_symbol, sizeof(symbol));
    expect_value(mock_malloc, size, sizeof(symbol));
    will_return(mock_malloc, malloc_ret_block_for_symbol);
    alloc_and_save_address_to_be_freed((void **)&strdup_ret_block, sizeof(char) * (strlen(valid_symbol_name)+1));
    expect_value(mock_strdup, s, valid_symbol_name);
    will_return(mock_strdup, strdup_ret_block);
    expect_value(mock_list_push, l, get_symbol_pool());
    expect_value(mock_list_push, e, malloc_ret_block_for_symbol);
    will_return(mock_list_push, DUMMY_SYMBOL_POOL);
    expect_value(mock_hashtable_add, ht, stub_symtab.symbols);
    expect_value(mock_hashtable_add, key, valid_symbol_name);
    expect_value(mock_hashtable_add, value, malloc_ret_block_for_symbol);
    will_return(mock_hashtable_add, 1);
	expect_value(mock_list_pop, l_p, get_symbol_pool_address());
	will_return(mock_list_pop, malloc_ret_block_for_symbol);
    expect_value(mock_free, ptr, strdup_ret_block);
    expect_value(mock_free, ptr, malloc_ret_block_for_symbol);

    assert_int_equal(
        symtab_intern_symbol(&stub_symtab, valid_symbol_name),
        1 );
}

// Given:
//  - arguments are valid
//  - hashtable_key_is_in_use will return false
//  - allocation for symbol will succeed
//  - duplication of name will succeed
//  - list_push will succeed
//  - hashtable_add will succeed
// Expected:
//  - calls hashtable_key_is_in_use with:
//    - ht: st->symbols
//    - key: name
//  - calls malloc with:
//    - size: sizeof(symbol)
//  - calls strdup with:
//    - s: name
//  - call list_push with:
//    - l: symbol_pool
//    - e: malloc returned value for symbol
//  - calls hashtable_add:
//    - ht: st->symbols
//    - key: name
//    - value: malloc returned value for symbol
//  - returns 0
static void intern_symbol_interns_new_symbol_when_hashtable_add_succeeds(void **state) {
    expect_value(mock_hashtable_key_is_in_use, ht, stub_symtab.symbols);
    expect_string(mock_hashtable_key_is_in_use, key, valid_symbol_name);
    will_return(mock_hashtable_key_is_in_use, false);
    alloc_and_save_address_to_be_freed((void **)&malloc_ret_block_for_symbol, sizeof(symbol));
    expect_value(mock_malloc, size, sizeof(symbol));
    will_return(mock_malloc, malloc_ret_block_for_symbol);
    alloc_and_save_address_to_be_freed((void **)&strdup_ret_block, sizeof(char) * (strlen(valid_symbol_name)+1));
    expect_string(mock_strdup, s, valid_symbol_name);
    will_return(mock_strdup, strdup_ret_block);
    expect_value(mock_list_push, l, get_symbol_pool());
    expect_value(mock_list_push, e, malloc_ret_block_for_symbol);
    will_return(mock_list_push, DUMMY_SYMBOL_POOL);
    expect_value(mock_hashtable_add, ht, stub_symtab.symbols);
    expect_value(mock_hashtable_add, key, valid_symbol_name);
    expect_value(mock_hashtable_add, value, malloc_ret_block_for_symbol);
    will_return(mock_hashtable_add, 0);

    assert_int_equal(
        symtab_intern_symbol(&stub_symtab, valid_symbol_name),
        0 );
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
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

    const struct CMUnitTest get_local_tests[] = {
        cmocka_unit_test_setup_teardown(
            get_local_returns_null_when_st_null,
            get_local_setup, get_local_teardown),
        cmocka_unit_test_setup_teardown(
            get_local_calls_hashtable_get_and_returns_its_returned_value_when_st_not_null,
            get_local_setup, get_local_teardown),
    };

    const struct CMUnitTest remove_tests[] = {
        cmocka_unit_test_setup_teardown(
            remove_returns_1_when_st_null,
            remove_setup, remove_teardown),
        cmocka_unit_test_setup_teardown(
            remove_calls_hashtable_remove_and_returns_its_returned_value_when_st_not_null_name_not_null,
            remove_setup, remove_teardown),
    };

    const struct CMUnitTest contains_local_tests[] = {
        cmocka_unit_test_setup_teardown(
            contains_local_returns_0_when_st_null,
            contains_local_setup, contains_local_teardown),
        cmocka_unit_test_setup_teardown(
            contains_local_calls_hashtable_key_is_in_use_when_st_not_null,
            contains_local_setup, contains_local_teardown),
    };

    const struct CMUnitTest get_tests[] = {
        cmocka_unit_test_setup_teardown(
            get_returns_null_when_st_null,
            get_setup, get_teardown),
        cmocka_unit_test_setup_teardown(
            get_calls_symtab_contains_local_when_st_not_null,
            get_setup, get_teardown),
        cmocka_unit_test_setup_teardown(
            get_calls_symtab_get_local_and_returns_its_returned_value_when_symtab_contains_local_returns_true,
            get_setup, get_teardown),
        cmocka_unit_test_setup_teardown(
            get_calls_itself_and_returns_value_when_symtab_contains_local_returns_false,
            get_setup, get_teardown),
    };

    const struct CMUnitTest contains_tests[] = {
        cmocka_unit_test_setup_teardown(
            contains_returns_0_when_st_null,
            contains_setup, contains_teardown),
        cmocka_unit_test_setup_teardown(
            contains_calls_symtab_contains_local_when_st_not_null,
            contains_setup, contains_teardown),
        cmocka_unit_test_setup_teardown(
            contains_returns_1_when_symtab_contains_local_returns_1,
            contains_setup, contains_teardown),
        cmocka_unit_test_setup_teardown(
            contains_calls_symtab_contains_and_returns_value_when_symtab_contains_local_returns_0,
            contains_setup, contains_teardown),
    };

    const struct CMUnitTest intern_symbol_tests[] = {
        cmocka_unit_test_setup_teardown(
            intern_symbol_error_when_st_null,
            intern_symbol_setup, intern_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            intern_symbol_error_when_name_null,
            intern_symbol_setup, intern_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            intern_symbol_error_when_name_too_long,
            intern_symbol_setup, intern_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            intern_symbol_error_when_symbols_field_null,
            intern_symbol_setup, intern_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            intern_symbol_do_nothing_and_returns_0_when_symbol_already_interned,
            intern_symbol_setup, intern_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            intern_symbol_error_when_malloc_for_symbol_fails,
            intern_symbol_setup, intern_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            intern_symbol_cleanup_and_error_when_strdup_fails,
            intern_symbol_setup, intern_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            intern_symbol_cleanup_and_error_when_list_push_fails,
            intern_symbol_setup, intern_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            intern_symbol_cleanup_and_error_when_hashtable_add_fails,
            intern_symbol_setup, intern_symbol_teardown),
        cmocka_unit_test_setup_teardown(
            intern_symbol_interns_new_symbol_when_hashtable_add_succeeds,
            intern_symbol_setup, intern_symbol_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(wind_scope_tests, NULL, NULL);
    failed += cmocka_run_group_tests(unwind_scope_tests, NULL, NULL);
    failed += cmocka_run_group_tests(get_local_tests, NULL, NULL);
    failed += cmocka_run_group_tests(remove_tests, NULL, NULL);
    failed += cmocka_run_group_tests(contains_local_tests, NULL, NULL);
    failed += cmocka_run_group_tests(get_tests, NULL, NULL);
    failed += cmocka_run_group_tests(contains_tests, NULL, NULL);
    failed += cmocka_run_group_tests(intern_symbol_tests, NULL, NULL);

    return failed;
}
