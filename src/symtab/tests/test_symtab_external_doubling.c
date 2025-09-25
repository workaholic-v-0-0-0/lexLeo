// src/symtab/test/test_symtab_external_doubling.c

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "string_utils.h"
#include "internal/symtab_internal.h"
#include "internal/symtab_test_utils.h"
#include "symtab.h"
#include "list.h"
#include "ast.h"
#include "fake_memory.h"
#include "memory_allocator.h"



//-----------------------------------------------------------------------------
// GLOBALS NOT DOUBLES
//-----------------------------------------------------------------------------


static symtab *top_level = NULL;



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


// fakes
#define FAKABLE_MALLOC(n) (get_current_malloc()(n))
#define FAKABLE_FREE(p) (get_current_free()(p))
#define FAKABLE_STRDUP(s) (get_current_string_duplicate()(s))



//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------





//-----------------------------------------------------------------------------
// symtab_cleanup_pool TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// void symtab_cleanup_pool(void);
// static list *symbol_pool;
// list list_push(list l, void * e);
// void list_free_list(list l, void (*destroy_fn)(void *item, void *user_data), void *user_data)

// fake:
//  - functions of standard libray which are used:
//    - malloc, free, strdup



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int cleanup_pool_setup(void **state) {
    (void)state;

    // real
    set_symbol_pool(NULL);

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    return 0;
}

static int cleanup_pool_teardown(void **state) {
    (void)state;
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
    fake_memory_reset();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - symbol_pool == NULL
// Expected:
//  - do nothing
static void cleanup_pool_do_nothing_when_symbol_pool_is_null(void **state) {
    (void)state;
    assert_null(get_symbol_pool());

    symtab_cleanup_pool();

    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
}

// Given:
//  - symbol_pool == <a one-element list>
// Expected:
//  - the global symbol_pool is cleaned up
//  - no invalid free
//  - no double free
//  - no memory leak
static void cleanup_pool_successful_when_symbol_pool_has_one_element(void **state) {
    (void)state;
    symbol *s = FAKABLE_MALLOC(sizeof(symbol));
    s->name = FAKABLE_STRDUP("symbol_name");
    set_symbol_pool(list_push(NULL, s));

    symtab_cleanup_pool();

    assert_null(get_symbol_pool());
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
}

// Given:
//  - symbol_pool == <two-elements list>
// Expected:
//  - the global symbol_pool is cleaned up
//  - no invalid free
//  - no double free
//  - no memory leak
static void cleanup_pool_successful_when_symbol_pool_has_two_elements(void **state) {
    (void)state;
    list l = NULL;
    symbol *s1 = FAKABLE_MALLOC(sizeof(symbol));
    s1->name = FAKABLE_STRDUP("symbol_name_1");
    symbol *s2 = FAKABLE_MALLOC(sizeof(symbol));
    s2->name = FAKABLE_STRDUP("symbol_name_2");
    l = list_push(l, s2);
    l = list_push(l, s1);
    set_symbol_pool(l);

    symtab_cleanup_pool();

    assert_null(get_symbol_pool());
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
}



//-----------------------------------------------------------------------------
// symtab_intern_symbol TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// int symtab_intern_symbol(symtab *st, char *name)
// static list *symbol_pool;
// list list_push(list l, void * e);
// symtab *symtab_wind_scope(symtab *st);
// int symtab_contains_local(symtab *st, const char *name);
// symbol *symtab_get_local(symtab *st, const char *name)

// fake:
//  - functions of standard libray which are used:
//    - malloc, free, strdup



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int intern_symbol_setup(void **state) {
    (void)state;

    // real
    set_symbol_pool(NULL);
    assert_null(get_symbol_pool());

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    return 0;
}

static int intern_symbol_teardown(void **state) {
    (void)state;
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
    fake_memory_reset();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - st a non null well-formed symtab*
//  - name == "symbol_name"
// Expected:
//  - a new symbol with name "symbol_name" has been registred into symbol_pool
static void intern_symbol_register_new_symbol_into_symbol_pool(void **state) {
    (void)state;
    top_level = symtab_wind_scope(NULL);
    assert_non_null(top_level);

    assert_int_equal(
        symtab_intern_symbol(top_level, "symbol_name"),
        0 );
    assert_true(symtab_contains_local(top_level, "symbol_name"));

    symbol *sym = symtab_get_local(top_level, "symbol_name");
    assert_non_null(sym);
    list symbol_pool = get_symbol_pool();
    assert_non_null(symbol_pool);
    assert_non_null(((symbol *) symbol_pool->car));
    assert_ptr_equal(symbol_pool->car, sym);
    assert_string_equal(
        ((symbol *) symbol_pool->car)->name,
        "symbol_name" );

    fake_free(sym->name);
    fake_free(sym);
    fake_free(symbol_pool);
    hashtable_destroy(top_level->symbols);
    fake_free(top_level);
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
}






//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
    const struct CMUnitTest cleanup_pool_tests[] = {
        cmocka_unit_test_setup_teardown(
            cleanup_pool_do_nothing_when_symbol_pool_is_null,
            cleanup_pool_setup, cleanup_pool_teardown),
        cmocka_unit_test_setup_teardown(
            cleanup_pool_successful_when_symbol_pool_has_one_element,
            cleanup_pool_setup, cleanup_pool_teardown),
        cmocka_unit_test_setup_teardown(
            cleanup_pool_successful_when_symbol_pool_has_two_elements,
            cleanup_pool_setup, cleanup_pool_teardown),
    };

    const struct CMUnitTest intern_symbol_tests[] = {
        cmocka_unit_test_setup_teardown(
            intern_symbol_register_new_symbol_into_symbol_pool,
            intern_symbol_setup, intern_symbol_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(cleanup_pool_tests, NULL, NULL);
    failed += cmocka_run_group_tests(intern_symbol_tests, NULL, NULL);

    return failed;
}
