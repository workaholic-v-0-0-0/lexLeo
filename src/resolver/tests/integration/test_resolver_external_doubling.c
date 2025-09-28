// src/resolver/tests/integration/test_resolver_external_doubling.c

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
#include "internal/symtab_test_utils.h"
#include "ast.h"
#include "fake_memory.h"



//-----------------------------------------------------------------------------
// GLOBALS NOT DOUBLES
//-----------------------------------------------------------------------------


static ast *ast_to_promote = NULL;
resolver_ctx ctx;



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

// f(x){x="a_string";}
int initialize_ast_to_promote_with_function_node(void) {
    if (initialize_ast_to_promote_with_binding_node("x", "a_string") != 0)
		return 1;
    ast *binding_node = ast_to_promote;
	ast *block_items_node = ast_create_children_node_var(
        AST_TYPE_BLOCK_ITEMS,
        1,
        binding_node );
    if (!block_items_node) {
        ast_destroy(binding_node);
        return 1;
    }
	ast *block_node = ast_create_children_node_var(
        AST_TYPE_BLOCK,
        1,
        block_items_node );
    if (!block_node) {
        ast_destroy(block_items_node);
        return 1;
    }
	initialize_ast_to_promote_with_symbol_name_data_wrapper_ast("x");
	if (!ast_to_promote) {
		ast_destroy(block_node);
		return 1;
	}
	ast *parameters_node = ast_create_children_node_var(
    	AST_TYPE_PARAMETERS,
	    1,
    	ast_to_promote );
    if (!parameters_node) {
		ast_destroy(block_node);
		ast_destroy(ast_to_promote);
        return 1;
    }
	ast *list_of_parameters_node = ast_create_children_node_var(
    	AST_TYPE_LIST_OF_PARAMETERS,
	    1,
    	parameters_node );
    if (!list_of_parameters_node) {
		ast_destroy(block_node);
		ast_destroy(parameters_node);
        return 1;
    }
    if (initialize_ast_to_promote_with_symbol_name_data_wrapper_ast("f") != 0) {
		ast_destroy(block_node);
		ast_destroy(list_of_parameters_node);
		return 1;
	}
	ast *function_node = ast_create_children_node_var(
    	AST_TYPE_FUNCTION,
	    3,
		ast_to_promote,
    	list_of_parameters_node,
		block_node );
    if (!function_node) {
		ast_destroy(ast_to_promote);
		ast_destroy(block_node);
		ast_destroy(list_of_parameters_node);
        return 1;
    }
	ast_to_promote = function_node;
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS    int resolver_resolve_ast(symtab *st, ast **a);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// resolver_resolve_ast
// ast * param a
// ctx.st (ctx param ; ctx.st type is symtab*)
// symtab_wind_scope
// symtab_unwind_scope
// symtab_intern_symbol
// symtab_get
// ast_destroy
// ast_create_error_node_or_sentinel
// list_push
// list_pop

// fake:
//  - functions of standard libray which are used:
//    - malloc, free, strdup



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int resolve_ast_setup(void **state) {
    (void)state;

    // fake
    set_allocators(fake_malloc, fake_free);
    set_string_duplicate(fake_strdup);
    fake_memory_reset();

    // real
    ctx.ops.push = list_push;
    ctx.ops.pop = list_pop;
    ctx.ops.intern_symbol = symtab_intern_symbol;
    ctx.ops.get = symtab_get;
    ctx.ops.wind_scope = symtab_wind_scope;
    ctx.ops.unwind_scope = symtab_unwind_scope;
    ctx.st = symtab_wind_scope(NULL);
    assert_non_null(ctx.st);
    ast_to_promote = NULL;

    return 0;
}

static int resolve_ast_teardown(void **state) {
    (void)state;
    while (ctx.st) ctx.st = symtab_unwind_scope(ctx.st);
    assert_true(fake_memory_no_invalid_free());
    assert_true(fake_memory_no_double_free());
    assert_true(fake_memory_no_leak());
    set_allocators(NULL, NULL);
    set_string_duplicate(NULL);
    fake_memory_reset();
    memset(&ctx, 0, sizeof ctx);
    ast_to_promote = NULL;
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
        resolver_resolve_ast(NULL, ctx),
        false );
}

// Given:
//  - a == <a function node with one parameter and one statement in its body>
// Expected:
//  - the function symbol ("f") is interned in the current (global) scope
//  - the parameter symbol ("x") is interned in a nested scope
//    and therefore not visible in the global scope after resolution
//  - the global symbol table (ctx.st) contains only "f"
//  - the symbol pool contains exactly two symbols ("f" and "x")
//    because both were interned at some point
//  - after symtab_cleanup_pool(), the symbol pool is empty
//  - no invalid free
//  - no double free
//  - no memory leak
//  - returns true
static void resolve_ast_success_when_function_node(void **state) {
    (void)state;
    initialize_ast_to_promote_with_function_node();

    assert_int_equal(
        resolver_resolve_ast(&ast_to_promote, ctx),
        true );
    ast_destroy(ast_to_promote);
    symbol *symbol_f = symtab_get_local(ctx.st, "f");
    assert_non_null(symbol_f);
    assert_string_equal("f", symbol_f->name);
    assert_ptr_equal(
        get_symbol_pool()->cdr->car,
        symbol_f
    );
    symbol *symbol_x = symtab_get_local(ctx.st, "x");
    assert_null(symbol_x);
    assert_string_equal(
        ((symbol *) (get_symbol_pool()->car))->name,
        "x"
    );
    assert_int_equal(2, list_length(get_symbol_pool()));
    symtab_cleanup_pool();
    assert_int_equal(0, list_length(get_symbol_pool()));
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

        // a == <a function node with one parameter and one statement in its body>
        cmocka_unit_test_setup_teardown(
            resolve_ast_success_when_function_node,
            resolve_ast_setup, resolve_ast_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(resolve_ast_tests, NULL, NULL);

    return failed;
}
