// src/core/resolver/tests/unit/test_resolver.c

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

#include "memory_allocator.h"
#include "string_utils.h"
#include "resolver.h"
#include "internal/resolver_test_utils.h"
#include "symtab.h"
#include "ast.h"
#include "lexleo/test/fake_memory.h"



//-----------------------------------------------------------------------------
// GLOBALS NOT DOUBLES
//-----------------------------------------------------------------------------


static ast *ast_to_promote = NULL;
resolver_ctx ctx;



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------

// mocks
int mock_intern_symbol(symtab *st, const char *name) {
    check_expected(st);
    check_expected(name);
    return mock_type(int);
}
symbol *mock_get(symtab *st, const char *name) {
    check_expected(st);
    check_expected(name);
    return mock_type(symbol *);
}

// stubs
static symtab STUB_SYMTAB_INSTANCE = { .symbols = NULL, .parent = NULL };
static const symbol STUB_SYMBOL;

// fakes
#define FAKABLE_MALLOC(n) (get_current_malloc()(n))
#define FAKABLE_FREE(p) (get_current_free()(p))
#define FAKABLE_STRDUP(s) (get_current_string_duplicate()(s))



//-----------------------------------------------------------------------------
// HELPERS
//-----------------------------------------------------------------------------


int initialize_ast_to_promote_with_data_wrapper_ast_not_yet_resolved(void) {
    if (!(ast_to_promote = FAKABLE_MALLOC(sizeof(ast))))
        return 1;

    ast_to_promote->type = AST_TYPE_DATA_WRAPPER;

    if (!(ast_to_promote->data = FAKABLE_MALLOC(sizeof(typed_data)))) {
        FAKABLE_FREE(ast_to_promote);
        ast_to_promote = NULL;
        return 1;
    }

    return 0;
}

int initialize_ast_to_promote_with_string_data_wrapper_ast(const char *str) {
    if (initialize_ast_to_promote_with_data_wrapper_ast_not_yet_resolved() == 1)
        return 1;

    if (!(ast_to_promote->data->data.string_value = FAKABLE_STRDUP(str))) {
        FAKABLE_FREE(ast_to_promote->data);
        FAKABLE_FREE(ast_to_promote);
        ast_to_promote = NULL;
        return 1;
    }

    ast_to_promote->data->type = TYPE_STRING;
    return 0;
}

int initialize_ast_to_promote_with_symbol_name_data_wrapper_ast(const char *symbol_name) {
    if (initialize_ast_to_promote_with_data_wrapper_ast_not_yet_resolved() == 1)
        return 1;

    if (!(ast_to_promote->data->data.string_value = FAKABLE_STRDUP(symbol_name))) {
        FAKABLE_FREE(ast_to_promote->data);
        FAKABLE_FREE(ast_to_promote);
        ast_to_promote = NULL;
        return 1;
    }

    ast_to_promote->data->type = TYPE_SYMBOL_NAME;
    return 0;
}

int initialize_ast_to_promote_with_binding_node(const char *lhs, const char *rhs) {
    if (initialize_ast_to_promote_with_symbol_name_data_wrapper_ast(lhs) != 0) return 1;
    ast *symbol_name_data_wrapper_ast = ast_to_promote;
    if (initialize_ast_to_promote_with_string_data_wrapper_ast(rhs) != 0) {
        ast_destroy(symbol_name_data_wrapper_ast);
        return 1;
    }
    ast *string_data_wrapper_ast = ast_to_promote;
    ast_to_promote = ast_create_children_node_var(
        AST_TYPE_BINDING,
        2,
        symbol_name_data_wrapper_ast,
        string_data_wrapper_ast );
    if (!ast_to_promote) {
        ast_destroy(symbol_name_data_wrapper_ast);
        ast_destroy(string_data_wrapper_ast);
        return 1;
    }
    return 0;
}

void assert_is_sentinel_error(ast **a) {
    assert_non_null(a);
    assert_non_null(*a);
    assert_ptr_equal(*a, ast_error_sentinel());
    assert_int_equal((*a)->type, AST_TYPE_ERROR);
    assert_int_equal(
        (*a)->error->code,
        AST_UNRETRIEVABLE_ERROR_CODE );
    assert_true((*a)->error->is_sentinel);
    assert_string_equal(
        (*a)->error->message,
        "AST error sentinel: original cause lost due to allocation failure while constructing error node" );
}

void assert_is_error_not_sentinel(ast *a, ast_error_type code, const char *message) {
    assert_non_null(a);
    assert_int_equal(a->type, AST_TYPE_ERROR);
    assert_non_null(a->error);
    assert_int_equal(a->error->code, code);
    assert_false(a->error->is_sentinel);
    assert_non_null(a->error->message);
    assert_string_equal(a->error->message, message);
}



//-----------------------------------------------------------------------------
// TESTS    int resolver_resolve_ast(symtab *st, ast **a);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// resolver_resolve_ast
// ast * param a
// ast_destroy
// ast_create_error_node_or_sentinel
// list_push
// list_pop

// mock:
//  - functions of the symtab module which are used:
//    - symtab_intern_symbol
//    - symtab_get
// stub:
//  - symtab* param
// fake:
//  - functions of standard library which are used:
//    - malloc, free, strdup



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int resolve_ast_setup(void **state) {
    (void)state;

    // real
    ast_to_promote = NULL;
    ctx.ops.push = list_push;
    ctx.ops.pop = list_pop;

    // mock
    ctx.ops.intern_symbol = mock_intern_symbol;
    ctx.ops.get = mock_get;

    // stub
    ctx.st = &STUB_SYMTAB_INSTANCE;

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    return 0;
}

static int resolve_ast_teardown(void **state) {
    (void)state;
    ast_to_promote = NULL;
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
    fake_memory_reset();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - a == NULL
// Expected:
//  - do nothing
//  - returns false
static void resolve_ast_returns_false_when_a_null(void **state) {
    (void)state;
    assert_int_equal(
        resolver_resolve_ast(NULL, &ctx),
        false );
}

// Given:
//  - a != NULL && *a == NULL
//  - first memory allocation fails
//    i.e:
//      - allocation inside ast_create_error_node_or_sentinel will fail
// Expected:
//  - *a == ast_error_sentinel() ie:
//    - (*a)->type == AST_TYPE_ERROR
//    - (*a)->error->code == AST_UNRETRIEVABLE_ERROR_CODE
//    - (*a)->error->message same string as "AST error sentinel: original cause lost due to allocation failure while constructing error node"
//    - (*a)->error->is_sentinel == true
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns false
static void resolve_ast_make_error_node_sentinel_and_returns_false_when_root_pointer_is_null_and_first_alloc_fails(void **state) {
    (void)state;
    ast_to_promote = NULL;
    fake_memory_fail_only_on_call(1);

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        false );

    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_is_sentinel_error(&ast_to_promote);
    assert_true(fake_memory_no_leak());
}

// Given:
//  - a != NULL && *a == NULL
//  - memory allocation never fails
//    i.e:
//      - allocations inside ast_create_error_node_or_sentinel will succeed (malloc and strdup)
// Expected:
//  - (*a)->type == AST_TYPE_ERROR
//  - (*a)->error->code == RESOLVER_ERROR_CODE_NULL_ROOT
//  - (*a)->error->message same string as "resolver: null root AST pointer (nothing to resolve)"
//  - (*a)->error->is_sentinel == false
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns false
static void resolve_ast_make_error_node_not_sentinel_and_returns_false_when_root_pointer_is_null_and_malloc_never_fails(void **state) {
    (void)state;
    ast_to_promote = NULL;

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        false );
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_is_error_not_sentinel(
        ast_to_promote,
        RESOLVER_ERROR_CODE_NULL_ROOT,
        "resolver: null root AST pointer (nothing to resolve)" );
    ast_destroy(ast_to_promote);
    assert_true(fake_memory_no_leak());
}

// Given:
//  - *a == <a string data wrapper ast>
//  - allocation always fails
//    i.e:
//      - allocation of the root frame will fail
// Expected:
//  - *a == NULL
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns false
static void resolve_ast_fatal_error_oom_and_returns_false_when_string_data_wrapper_and_malloc_always_fails(void **state) {
    (void)state;
    assert_int_equal(initialize_ast_to_promote_with_string_data_wrapper_ast("string"), 0);
    fake_memory_fail_on_all_call();

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        false );
    assert_null(ast_to_promote);
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
}

// Given:
//  - *a == <a string data wrapper ast>
//  - the first allocation will fail
//    i.e:
//      - allocation of the root frame will fail
// Expected:
//  - *a == NULL
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns false
static void resolve_ast_fatal_error_oom_and_returns_false_when_string_data_wrapper_and_only_first_malloc_fails(void **state) {
    (void)state;
    assert_int_equal(initialize_ast_to_promote_with_string_data_wrapper_ast("string"), 0);
    fake_memory_fail_only_on_call(1);

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        false );
    assert_null(ast_to_promote);
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
}

// Given:
//  - *a == <a string data wrapper ast>
//  - the second allocation is the only one that fails
//    i.e.:
//      - allocation of the root frame will succeed
//      - allocation inside list_push (for the traversal stack) will fail
// Expected:
//  - *a == NULL
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns false
static void resolve_ast_fatal_error_oom_and_returns_false_when_string_data_wrapper_and_only_second_malloc_fails(void **state) {
    (void)state;
    assert_int_equal(initialize_ast_to_promote_with_string_data_wrapper_ast("string"), 0);
    fake_memory_fail_only_on_call(2);

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        false );
    assert_null(ast_to_promote);
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
}

// Given:
//  - *a == <a string data wrapper ast>
//  - memory allocation never fails
//    i.e.:
//      - allocation of the root frame will succeed
//      - allocation inside list_push (for the traversal stack) will succeed
// Expected:
//  - no side effect
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns true
static void resolve_ast_success_with_no_side_effect_when_string_data_wrapper_and_string_data_wrapper_ast(void **state) {
    (void)state;
    assert_int_equal(initialize_ast_to_promote_with_string_data_wrapper_ast("string"), 0);
    ast *old_ast = ast_to_promote;
    ast_type old_type = ast_to_promote->type;
    typed_data *old_data = ast_to_promote->data;
    data_type old_data_type = old_data->type;
    char *old_data_data = old_data->data.string_value;
    int old_string_value_length = strlen(old_data_data);

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        true );
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_ptr_equal(ast_to_promote, old_ast);
    assert_int_equal(ast_to_promote->type, old_type);
    assert_ptr_equal(ast_to_promote->data, old_data);
    assert_int_equal(ast_to_promote->data->type, old_data_type);
    assert_ptr_equal(ast_to_promote->data->data.string_value, old_data_data);
    assert_string_equal(ast_to_promote->data->data.string_value, old_data_data);
    ast_destroy(ast_to_promote);
    assert_true(fake_memory_no_leak());
}

// Given:
//  - *a == <a symbol data wrapper ast>
//  - the first allocation is the only one that fails
//    i.e:
//      - allocation of the root frame will fail
// Expected:
//  - *a == NULL
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns false
static void resolve_ast_fatal_error_oom_and_returns_false_when_symbol_name_data_wrapper_and_only_first_malloc_fails(void **state) {
    (void)state;
    assert_int_equal(initialize_ast_to_promote_with_symbol_name_data_wrapper_ast("symbol_name"), 0);
    fake_memory_fail_only_on_call(1);

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        false );
    assert_null(ast_to_promote);
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
}

// Given:
//  - *a == <a symbol data wrapper ast>
//  - the second allocation is the only one that fails
//    i.e.:
//      - allocation of the root frame will succeed
//      - allocation inside list_push (for the traversal stack) will fail
// Expected:
//  - *a == NULL
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns false
static void resolve_ast_fatal_error_oom_and_returns_false_when_symbol_name_data_wrapper_and_only_second_malloc_fails(void **state) {
    (void)state;
    assert_int_equal(initialize_ast_to_promote_with_symbol_name_data_wrapper_ast("symbol_name"), 0);
    fake_memory_fail_only_on_call(2);

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        false );
    assert_null(ast_to_promote);
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
}

// Given:
//  - *a == <a symbol data wrapper ast>
//  - the failing allocation indexes: {3}
//    i.e.:
//      - allocation of the root frame will succeed
//      - allocation inside list_push (for the traversal stack) will succeed
//      - allocation inside ast_create_error_node_or_sentinel will fail
//  - intern_symbol will fail
// Expected:
//  - calls intern_symbol with:
//    - st: ctx.st
//    - name: "symbol_name"
//    - returned value: 0
//  - *a == ast_error_sentinel() ie:
//    - (*a)->type == AST_TYPE_ERROR
//    - (*a)->error->code == AST_UNRETRIEVABLE_ERROR_CODE
//    - (*a)->error->message same string as "AST error sentinel: original cause lost due to allocation failure while constructing error node"
//    - (*a)->error->is_sentinel == true
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns false
static void resolve_ast_make_error_node_sentinel_and_returns_false_when_symbol_name_data_wrapper_and_intern_symbol_fails_and_only_third_malloc_fails(void **state) {
    (void)state;
    assert_int_equal(initialize_ast_to_promote_with_symbol_name_data_wrapper_ast("symbol_name"), 0);
    expect_value(mock_intern_symbol, st, &STUB_SYMTAB_INSTANCE);
    expect_string(mock_intern_symbol, name, "symbol_name");
    will_return(mock_intern_symbol, 1);
    fake_memory_fail_only_on_call(3);

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        false );
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_is_sentinel_error(&ast_to_promote);
    assert_true(fake_memory_no_leak());
}

// Given:
//  - *a == <a symbol data wrapper ast>
//  - the failing allocation indexes: {3}
//    i.e.:
//      - allocation of the root frame will succeed
//      - allocation inside list_push (for the traversal stack) will succeed
//      - allocation inside ast_create_error_node_or_sentinel will fail
//  - intern_symbol will succeed
//  - get will fail
// Expected:
//  - calls intern_symbol with:
//    - st: ctx.st
//    - name: "symbol_name"
//    - returned value: 0
//  - calls get with:
//    - st: ctx.st
//    - name: "symbol_name"
//    - returned value: NULL
//  - *a == ast_error_sentinel() ie:
//    - (*a)->type == AST_TYPE_ERROR
//    - (*a)->error->code == AST_UNRETRIEVABLE_ERROR_CODE
//    - (*a)->error->message same string as "AST error sentinel: original cause lost due to allocation failure while constructing error node"
//    - (*a)->error->is_sentinel == true
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns false
static void resolve_ast_make_error_node_sentinel_and_returns_false_when_symbol_name_data_wrapper_and_get_fails_and_allocations_3_fail(void **state) {
    (void)state;
    assert_int_equal(initialize_ast_to_promote_with_symbol_name_data_wrapper_ast("symbol_name"), 0);
    expect_value(mock_intern_symbol, st, &STUB_SYMTAB_INSTANCE);
    expect_string(mock_intern_symbol, name, "symbol_name");
    will_return(mock_intern_symbol, 0);
    expect_value(mock_get, st, &STUB_SYMTAB_INSTANCE);
    expect_string(mock_get, name, "symbol_name");
    will_return(mock_get, NULL);
    fake_memory_fail_only_on_call(3);

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        false );
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_is_sentinel_error(&ast_to_promote);
    assert_true(fake_memory_no_leak());
}

// Given:
//  - *a == <a symbol data wrapper ast>
//  - memory allocation never fails
//    i.e.:
//      - allocation of the root frame will succeed
//      - allocation inside list_push (for the traversal stack) will succeed
//      - allocation inside ast_create_error_node_or_sentinel will succeed
//  - intern_symbol will succeed
//  - get will fail
// Expected:
//  - calls intern_symbol with:
//    - st: ctx.st
//    - name: "symbol_name"
//    - returned value: 0
//  - calls get with:
//    - st: ctx.st
//    - name: "symbol_name"
//    - returned value: NULL
//  - (*a)->type == AST_TYPE_ERROR
//  - (*a)->error->code == RESOLVER_ERROR_CODE_SYMBOL_LOOKUP_FAILED
//  - (*a)->error->message same string as "resolver: unretrievable interned symbol (inconsistent symtab state)"
//  - (*a)->error->is_sentinel == false
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns false
static void resolve_ast_make_error_node_not_sentinel_and_returns_false_when_symbol_name_data_wrapper_and_get_fails_and_malloc_never_fail(void **state) {
    (void)state;
    assert_int_equal(initialize_ast_to_promote_with_symbol_name_data_wrapper_ast("symbol_name"), 0);
    expect_value(mock_intern_symbol, st, &STUB_SYMTAB_INSTANCE);
    expect_string(mock_intern_symbol, name, "symbol_name");
    will_return(mock_intern_symbol, 0);
    expect_value(mock_get, st, &STUB_SYMTAB_INSTANCE);
    expect_string(mock_get, name, "symbol_name");
    will_return(mock_get, NULL);

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        false );
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_is_error_not_sentinel(
        ast_to_promote,
        RESOLVER_ERROR_CODE_SYMBOL_LOOKUP_FAILED,
        "resolver: unretrievable interned symbol (inconsistent symtab state)" );
    ast_destroy(ast_to_promote);;
    assert_true(fake_memory_no_leak());
}

// Given:
//  - *a == <a symbol data wrapper ast>
//  - memory allocation never fails
//    i.e.:
//      - allocation of the root frame will succeed
//      - allocation inside list_push (for the traversal stack) will succeed
//  - intern_symbol will succeed
//  - get will succeed
// Expected:
//  - calls intern_symbol with:
//    - st: ctx.st
//    - name: "symbol_name"
//    - returned value: 0
//  - calls get with:
//    - st: ctx.st
//    - name: "symbol_name"
//    - returned value: a non null symbol pointer pointing to well-formed symbol
//  - *a is deeply modified
//    - *a is unchanged
//    - (*a)->type is unchanged
//    - (*a)->data->type == TYPE_SYMBOL
//    - (*a)->data->symbol_value == symbol pointer returned by get
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns true
static void resolve_ast_success_with_promotion_into_symbol_data_wrapper_when_symbol_name_data_wrapper(void **state) {
    (void)state;
    assert_int_equal(initialize_ast_to_promote_with_symbol_name_data_wrapper_ast("symbol_name"), 0);
    expect_value(mock_intern_symbol, st, &STUB_SYMTAB_INSTANCE);
    expect_string(mock_intern_symbol, name, "symbol_name");
    will_return(mock_intern_symbol, 0);
    expect_value(mock_get, st, &STUB_SYMTAB_INSTANCE);
    expect_string(mock_get, name, "symbol_name");
    will_return(mock_get, &STUB_SYMBOL);
    ast *old_ast = ast_to_promote;
    ast_type old_type = ast_to_promote->type;
    typed_data *old_data = ast_to_promote->data;

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        true );
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_ptr_equal(ast_to_promote, old_ast);
    assert_int_equal(ast_to_promote->type, old_type);
    assert_ptr_equal(ast_to_promote->data, old_data);
    assert_int_equal(ast_to_promote->data->type, TYPE_SYMBOL);
    assert_ptr_equal(ast_to_promote->data->data.symbol_value, &STUB_SYMBOL);
    ast_destroy(ast_to_promote);
    assert_true(fake_memory_no_leak());
}

// Given:
//  - *a == <a binding node>
//  - the first allocation is the only one that fails
//    i.e:
//      - allocation of the root frame will fail
// Expected:
//  - *a == NULL
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns false
static void resolve_ast_fatal_error_oom_and_returns_false_when_binding_node_and_only_first_malloc_fails(void **state) {
    (void)state;
    assert_int_equal(initialize_ast_to_promote_with_binding_node("symbol_name", "string"), 0);
    fake_memory_fail_only_on_call(1);

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        false );
    assert_null(ast_to_promote);
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
}

// Given:
//  - *a == <a binding node>
//  - the second allocation is the only one that fails
//    i.e.:
//      - allocation of the root frame will succeed
//      - allocation inside list_push (for the traversal stack) will fail
// Expected:
//  - *a == NULL
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns false
static void resolve_ast_fatal_error_oom_and_returns_false_when_binding_node_and_only_second_malloc_fails(void **state) {
    (void)state;
    assert_int_equal(initialize_ast_to_promote_with_binding_node("symbol_name", "string"), 0);
    fake_memory_fail_only_on_call(2);

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        false );
    assert_null(ast_to_promote);
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
}

// Given:
//  - *a == <a binding node>
//  - the third allocation is the only one that fails
//    i.e.:
//      - allocation of the root frame will succeed
//      - allocation inside list_push (for the traversal stack) will succeed
//      - allocation of the second child frame will fail
// Expected:
//  - *a == NULL
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns false
static void resolve_ast_turns_second_child_into_error_node_sentinel_and_promote_first_child_into_symbol_data_wrapper_and_returns_false_when_binding_node_and_allocations_3_4_fail(void **state) {
    (void)state;
    assert_int_equal(initialize_ast_to_promote_with_binding_node("symbol_name", "string"), 0);
    fake_memory_fail_only_on_call(3);

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        false );
    assert_null(ast_to_promote);
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
}

// Given:
//  - *a == <a binding node>
//  - memory allocation never fails
//    i.e.:
//      - allocation of the root frame will succeed
//      - allocation inside list_push (for the traversal stack) will succeed
//      - allocation of the second child frame will succeed
//      - allocation of the first child frame will succeed
//  - intern_symbol will succeed
//  - get will succeed
// Expected:
//  - calls intern_symbol with:
//    - st: ctx.st
//    - name: "symbol_name"
//    - returned value: 0
//  - calls get with:
//    - st: ctx.st
//    - name: "symbol_name"
//    - returned value: a non null symbol pointer pointing to well-formed symbol
//  - are unchanged:
//    - *a
//    - (*a)->type
//    - (*a)->children
//    - (*a)->children->children_nb
//    - (*a)->children->capacity
//    - (*a)->children->children
//    - (*a)->children->children[0]
//    - (*a)->children->children[0]->type
//    - (*a)->children->children[0]->data
//  - are deeply unchanged:
//    - (*a)->children->children[1]
//  - are modified:
//    - (*a)->children->children[0] is promoted into a symbol data wrapper:
//      - (*a)->children->children[0]->data->type == TYPE_SYMBOL
//      - (*a)->children->children[0]->data->symbol_value == symbol pointer returned by get
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns true
static void resolve_ast_success_with_a_symbol_promotion_when_binding_node_and_allocation_never_fails(void **state) {
    (void)state;
    assert_int_equal(initialize_ast_to_promote_with_binding_node("symbol_name", "string"), 0);
    ast *old_ast = ast_to_promote;
    ast_type old_type = ast_to_promote->type;
    ast_children_t *old_children_info = ast_to_promote->children;
    size_t old_children_nb = ast_to_promote->children->children_nb;
    size_t old_capacity = ast_to_promote->children->capacity;
    ast **old_children = ast_to_promote->children->children;
    ast *old_child_0 = ast_to_promote->children->children[0];
    ast_type old_child_0_type = old_child_0->type;
    typed_data *old_child_0_data = old_child_0->data;
    ast *old_child_1 = ast_to_promote->children->children[1];
    ast_type old_child_1_type = old_child_1->type;
    typed_data *old_child_1_data = old_child_1->data;
    size_t old_child_1_data_type =  old_child_1->data->type;
    char *old_child_1_data_string_value = old_child_1->data->data.string_value;
    expect_value(mock_intern_symbol, st, &STUB_SYMTAB_INSTANCE);
    expect_string(mock_intern_symbol, name, "symbol_name");
    will_return(mock_intern_symbol, 0);
    expect_value(mock_get, st, &STUB_SYMTAB_INSTANCE);
    expect_string(mock_get, name, "symbol_name");
    will_return(mock_get, &STUB_SYMBOL);

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, &ctx),
        true );
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_ptr_equal(ast_to_promote, old_ast);
    assert_int_equal(ast_to_promote->type, old_type);
    assert_ptr_equal(ast_to_promote->children, old_children_info);
    assert_int_equal(ast_to_promote->children->children_nb, old_children_nb);
    assert_int_equal(ast_to_promote->children->capacity, old_capacity);
    assert_ptr_equal(ast_to_promote->children->children, old_children);
    ast *new_child_0 = ast_to_promote->children->children[0];
    assert_ptr_equal(new_child_0, old_child_0);
    assert_int_equal(new_child_0->type, old_child_0_type);
    assert_ptr_equal(new_child_0->data, old_child_0_data);
    assert_int_equal(new_child_0->data->type, TYPE_SYMBOL);
    assert_ptr_equal(new_child_0->data->data.symbol_value, &STUB_SYMBOL);
    ast *new_child_1 = ast_to_promote->children->children[1];
    assert_ptr_equal(new_child_1, old_child_1);
    assert_int_equal(new_child_1->type, old_child_1_type);
    assert_ptr_equal(new_child_1->data, old_child_1_data);
    assert_int_equal(new_child_1->data->type, old_child_1_data_type);
    assert_string_equal(new_child_1->data->data.string_value, old_child_1_data_string_value);
    ast_destroy(ast_to_promote);
    assert_true(fake_memory_no_leak());
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
    const struct CMUnitTest resolve_ast_tests[] = {
        // a == NULL
        cmocka_unit_test_setup_teardown(
            resolve_ast_returns_false_when_a_null,
            resolve_ast_setup, resolve_ast_teardown),

        // a != NULL && *a == NULL
        cmocka_unit_test_setup_teardown(
            resolve_ast_make_error_node_sentinel_and_returns_false_when_root_pointer_is_null_and_first_alloc_fails,
            resolve_ast_setup, resolve_ast_teardown),
        cmocka_unit_test_setup_teardown(
            resolve_ast_make_error_node_not_sentinel_and_returns_false_when_root_pointer_is_null_and_malloc_never_fails,
            resolve_ast_setup, resolve_ast_teardown),

        // string data wrapper
        cmocka_unit_test_setup_teardown(
            resolve_ast_fatal_error_oom_and_returns_false_when_string_data_wrapper_and_malloc_always_fails,
            resolve_ast_setup, resolve_ast_teardown),
        cmocka_unit_test_setup_teardown(
            resolve_ast_fatal_error_oom_and_returns_false_when_string_data_wrapper_and_only_first_malloc_fails,
            resolve_ast_setup, resolve_ast_teardown),
        cmocka_unit_test_setup_teardown(
            resolve_ast_fatal_error_oom_and_returns_false_when_string_data_wrapper_and_only_second_malloc_fails,
            resolve_ast_setup, resolve_ast_teardown),
        cmocka_unit_test_setup_teardown(
            resolve_ast_success_with_no_side_effect_when_string_data_wrapper_and_string_data_wrapper_ast,
            resolve_ast_setup, resolve_ast_teardown),

        // symbol name data wrapper
        cmocka_unit_test_setup_teardown(
            resolve_ast_fatal_error_oom_and_returns_false_when_symbol_name_data_wrapper_and_only_first_malloc_fails,
            resolve_ast_setup, resolve_ast_teardown),
        cmocka_unit_test_setup_teardown(
            resolve_ast_fatal_error_oom_and_returns_false_when_symbol_name_data_wrapper_and_only_second_malloc_fails,
            resolve_ast_setup, resolve_ast_teardown),
        cmocka_unit_test_setup_teardown(
            resolve_ast_make_error_node_sentinel_and_returns_false_when_symbol_name_data_wrapper_and_intern_symbol_fails_and_only_third_malloc_fails,
            resolve_ast_setup, resolve_ast_teardown),
        cmocka_unit_test_setup_teardown(
            resolve_ast_make_error_node_sentinel_and_returns_false_when_symbol_name_data_wrapper_and_get_fails_and_allocations_3_fail,
            resolve_ast_setup, resolve_ast_teardown),
        cmocka_unit_test_setup_teardown(
            resolve_ast_make_error_node_not_sentinel_and_returns_false_when_symbol_name_data_wrapper_and_get_fails_and_malloc_never_fail,
            resolve_ast_setup, resolve_ast_teardown),
        cmocka_unit_test_setup_teardown(
            resolve_ast_success_with_promotion_into_symbol_data_wrapper_when_symbol_name_data_wrapper,
            resolve_ast_setup, resolve_ast_teardown),

        // binding node
        cmocka_unit_test_setup_teardown(
            resolve_ast_fatal_error_oom_and_returns_false_when_binding_node_and_only_first_malloc_fails,
            resolve_ast_setup, resolve_ast_teardown),
        cmocka_unit_test_setup_teardown(
            resolve_ast_fatal_error_oom_and_returns_false_when_binding_node_and_only_second_malloc_fails,
            resolve_ast_setup, resolve_ast_teardown),
        cmocka_unit_test_setup_teardown(
            resolve_ast_turns_second_child_into_error_node_sentinel_and_promote_first_child_into_symbol_data_wrapper_and_returns_false_when_binding_node_and_allocations_3_4_fail,
            resolve_ast_setup, resolve_ast_teardown),
        cmocka_unit_test_setup_teardown(
            resolve_ast_success_with_a_symbol_promotion_when_binding_node_and_allocation_never_fails,
            resolve_ast_setup, resolve_ast_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(resolve_ast_tests, NULL, NULL);

    return failed;
}
