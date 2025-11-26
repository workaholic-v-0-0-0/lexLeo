// src/symtab/tests/test_symtab_external_doubling.c

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
static symtab *local_level = NULL;
static symtab *current_level = NULL;


//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


// fakes
#define FAKABLE_MALLOC(n) (get_current_malloc()(n))
#define FAKABLE_FREE(p) (get_current_free()(p))
#define FAKABLE_STRDUP(s) (get_current_string_duplicate()(s))



//-----------------------------------------------------------------------------
// symtab_intern_symbol TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// int symtab_intern_symbol(symtab *st, char *name)
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
    (void) state;
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();
    return 0;
}

static int intern_symbol_teardown(void **state) {
    (void) state;
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
//  - a new symbol with name "symbol_name" has been registred into st at the local scope
static void intern_symbol_register_new_symbol_into_symbol_pool(void **state) {
    (void) state;
    top_level = symtab_wind_scope(NULL);
    assert_non_null(top_level);

    assert_int_equal(
        symtab_intern_symbol(top_level, "symbol_name"),
        0);
    assert_true(symtab_contains_local(top_level, "symbol_name"));

    symbol *sym = symtab_get_local(top_level, "symbol_name");
    assert_non_null(sym);

    fake_free(sym->name);
    fake_free(sym);
    hashtable_destroy(top_level->symbols);
    fake_free(top_level);
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
}


//-----------------------------------------------------------------------------
// symtab_unwind_scope TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// symtab *symtab_unwind_scope(symtab *st);
// symtab *symtab_wind_scope(symtab *st);
// int symtab_intern_symbol(symtab *st, char *name);
// int symtab_contains_local(symtab *st, const char *name);
// symbol *symtab_get_local(symtab *st, const char *name)
// symtab *symtab_unwind_scope(symtab *st);

// fake:
//  - functions of standard libray which are used:
//    - malloc, free, strdup


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int unwind_scope_setup(void **state) {
    (void) state;
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();
    return 0;
}

static int unwind_scope_teardown(void **state) {
    (void) state;
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
    fake_memory_reset();
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - a two-level symtab with a symbol interned in the local level
//  - st == the local scope
// Expected:
//  - the local scope is cleaned up but not the interned symbol
static void unwind_scope_destroy_local_scope_but_not_destroy_symbol(void **state) {
    (void) state;
    top_level = symtab_wind_scope(NULL);
    assert_non_null(top_level);
    local_level = symtab_wind_scope(top_level);
    assert_non_null(local_level);
    assert_ptr_equal(local_level->parent, top_level);
    current_level = local_level;
    assert_int_equal(
        symtab_intern_symbol(current_level, "symbol_name"),
        0);
    assert_true(symtab_contains_local(local_level, "symbol_name"));
    assert_false(symtab_contains_local(top_level, "symbol_name"));
    symbol *sym = symtab_get_local(local_level, "symbol_name");
    assert_non_null(sym);

    current_level = symtab_unwind_scope(current_level);

    fake_free(sym->name);
    fake_free(sym);
    assert_non_null(top_level);
    current_level = symtab_unwind_scope(current_level);
    assert_null(current_level);

    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
}


//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
    const struct CMUnitTest intern_symbol_tests[] = {
        cmocka_unit_test_setup_teardown(
            intern_symbol_register_new_symbol_into_symbol_pool,
            intern_symbol_setup, intern_symbol_teardown),
    };

    const struct CMUnitTest unwind_scope_tests[] = {
        cmocka_unit_test_setup_teardown(
            unwind_scope_destroy_local_scope_but_not_destroy_symbol,
            unwind_scope_setup, unwind_scope_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(intern_symbol_tests, NULL, NULL);
    failed += cmocka_run_group_tests(unwind_scope_tests, NULL, NULL);

    return failed;
}