// src/core/runtime_session/tests/test_runtime_session

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "runtime_session_internal.h"
#include "lexleo/test/fake_memory.h"
#include "string_utils.h"
#include "symtab.h"
#include "ast.h"



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


// fakes

#define FAKABLE_MALLOC(n) (get_current_malloc()(n))
#define FAKABLE_FREE(p) (get_current_free()(p))
#define FAKABLE_STRDUP(s) (get_current_string_duplicate()(s))


// dummies

static yyscan_t dummy_scanner = (yyscan_t) 0xDEADBEEF;
static symbol *dummy_symbol_function = (symbol *) 0xDEADC0DE;
static symbol *dummy_symbol_parameter = (symbol *) 0xBAADF00D;


// spies

typedef struct {
	bool init_fn_has_been_called;
	bool destroy_fn_has_been_called;
	yyscan_t* init_arg_scanner;
	yyscan_t destroy_arg_scanner;
} runtime_session_lexer_spy_t;
static runtime_session_lexer_spy_t *g_lexer_spy = NULL;
static int spy_lexer_init_fn(yyscan_t *scanner) {
	assert_non_null(g_lexer_spy);
	g_lexer_spy->init_fn_has_been_called = true;
	g_lexer_spy->init_arg_scanner = scanner;
	*scanner = dummy_scanner;
	return 0;
}
static int spy_lexer_destroy_fn(yyscan_t scanner) {
	assert_non_null(g_lexer_spy);
	g_lexer_spy->destroy_fn_has_been_called = true;
	g_lexer_spy->destroy_arg_scanner = scanner;
	return 0;
}



//-----------------------------------------------------------------------------
// GENERAL HELPERS
//-----------------------------------------------------------------------------


// f(x){x="a_string";}
ast *a_function_node(void) {
	return ast_create_children_node_var(
		AST_TYPE_FUNCTION,
		3,
		ast_create_symbol_node(dummy_symbol_function),
		ast_create_children_node_var(
			AST_TYPE_LIST_OF_PARAMETERS,
			1,
			ast_create_children_node_var(
				AST_TYPE_PARAMETERS,
				1,
				ast_create_symbol_node(dummy_symbol_parameter)
			)
		),
		ast_create_children_node_var(
			AST_TYPE_BLOCK,
			1,
			ast_create_children_node_var(
				AST_TYPE_BLOCK_ITEMS,
				1,
				ast_create_children_node_var(
					AST_TYPE_BINDING,
					2,
					ast_create_symbol_node(dummy_symbol_parameter),
					ast_create_string_node("a_string")
				)
			)
		)
	);
}



//-----------------------------------------------------------------------------
// TESTS
// struct runtime_session *runtime_session_create(void);
// void runtime_session_destroy(struct runtime_session *session);
//-----------------------------------------------------------------------------


/*
  - struct runtime_session *runtime_session_create(void);
  - void runtime_session_destroy(struct runtime_session *session);
  - symtab *symtab_wind_scope(symtab *st);
  - symtab *symtab_unwind_scope(symtab *st);
  - runtime_env *runtime_env_wind(runtime_env *parent);
  - runtime_env *runtime_env_unwind(runtime_env *e);
*/



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


/*
  - fake:
	- malloc, free, realloc
  - dummy:
    - arg yyscan_t* of yylex_init
    - arg yyscan_t of yylex_destroy
  -spy:
    - functions of the module lexer which are used:
      - yylex_init
      - yylex_destroy
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int create_setup(void **state) {
	(void)state;

	// fakes
	set_allocators(fake_malloc, fake_free);
	set_reallocator(fake_realloc);
	fake_memory_reset();

	// spies
	g_lexer_spy = fake_malloc(sizeof(runtime_session_lexer_spy_t));
	g_lexer_spy->init_fn_has_been_called = false;
	g_lexer_spy->destroy_fn_has_been_called = false;
	g_lexer_spy->init_arg_scanner = NULL;
	g_lexer_spy->destroy_arg_scanner = NULL;
	runtime_session_set_lexer_ops(&(lexer_legacy_ops_t){
		.lexer_init_fn = spy_lexer_init_fn,
		.lexer_destroy_fn = spy_lexer_destroy_fn
	});

	return 0;
}

static int create_teardown(void **state) {
	fake_free(g_lexer_spy);
	g_lexer_spy = NULL;
	runtime_session_reset_lexer_ops();
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
	assert_true(fake_memory_no_leak());
	set_allocators(NULL, NULL);
	set_reallocator(NULL);
	fake_memory_reset();
	return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - oom
// Expected:
//  - ret == NULL
static void create_ret_null_when_oom(void **state) {
	(void)state;
	fake_memory_fail_on_all_call();
	runtime_session *ret = runtime_session_create();
	fake_memory_fail_on_calls(0, NULL);
	assert_null(ret);
}

// Given:
//  - no oom
// Expected:
//  - ret != NULL
static void create_ret_not_null_when_no_oom(void **state) {
	(void)state;
	runtime_session *ret = runtime_session_create();
	assert_non_null(ret);

	runtime_session_destroy(ret);
}

// Given:
//  - no oom
// Expected after runtime_session_create call:
//  - ret != NULL
//  - ret->in == NULL
//  - yylex_init has been called with:
//    - scanner == &ret->scanner
//  - ret->scanner != NULL
//  - ret->st != NULL
//  - ret->env != NULL
//  - ret->ast_pool == NULL
//  - ret->symbol_pool == NULL
// Expected after runtime_session_destroy call:
//  - yylex_destroy has been called with:
//    - scanner == <value of ret->scanner before destruction of ret>
static void create_initialize_ctx_when_no_oom(void **state) {
	(void)state;
	runtime_session *ret = runtime_session_create();

	assert_non_null(ret);
	assert_null(ret->in);
	assert_non_null(ret->scanner);
	assert_non_null(ret->st);
	assert_non_null(ret->env);
	assert_null(ret->ast_pool);
	assert_null(ret->symbol_pool);
	assert_true(g_lexer_spy->init_fn_has_been_called);
	assert_ptr_equal(g_lexer_spy->init_arg_scanner, &ret->scanner);
	yyscan_t scanner_field_before_destruction = ret->scanner;

	runtime_session_destroy(ret);

	assert_true(g_lexer_spy->destroy_fn_has_been_called);
	assert_ptr_equal(g_lexer_spy->destroy_arg_scanner, scanner_field_before_destruction);
}



//-----------------------------------------------------------------------------
// TESTS
// bool runtime_session_store_symbol(struct symbol *sym, runtime_session *session);
// bool runtime_session_store_ast(struct ast *root, runtime_session *session);
// void runtime_session_destroy(struct runtime_session *session);
//-----------------------------------------------------------------------------


/*
  - bool runtime_session_store_symbol(struct symbol *sym, runtime_session *session)
  - bool list_contains(list l, void *item);
  - int symtab_intern_symbol(symtab *st, char *name);
  - symbol *symtab_get(symtab *st, const char *name);
  - list list_push(list l, void * e);
  - struct runtime_session *runtime_session_create(void);
  - void runtime_session_destroy(struct runtime_session *session);
  - symtab *symtab_wind_scope(symtab *st);
  - symtab *symtab_unwind_scope(symtab *st);
  - runtime_env *runtime_env_wind(runtime_env *parent);
  - runtime_env *runtime_env_unwind(runtime_env *e);
  - void list_free_list(list l, void (*destroy_fn_t)(void *item, void *user_data), void *user_data);
*/



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


/*
  - fake:
	- malloc, free, realloc
  - dummy:
	- arg yyscan_t* of yylex_init
	- arg yyscan_t of yylex_destroy
    - symbols inside ast created by the general helper a_function_node (hence not registered in symbol_pool)
  - spy:
	- functions of the module lexer which are used:
	  - yylex_init
	  - yylex_destroy
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int destroy_setup(void **state) {
	(void)state;

	// fakes
	set_allocators(fake_malloc, fake_free);
	set_reallocator(fake_realloc);
	set_string_duplicate(fake_strdup);
	fake_memory_reset();

	// spies
	g_lexer_spy = fake_malloc(sizeof(runtime_session_lexer_spy_t));
	g_lexer_spy->init_fn_has_been_called = false;
	g_lexer_spy->destroy_fn_has_been_called = false;
	g_lexer_spy->init_arg_scanner = NULL;
	g_lexer_spy->destroy_arg_scanner = NULL;
	runtime_session_set_lexer_ops(&(lexer_legacy_ops_t){
		.lexer_init_fn = spy_lexer_init_fn,
		.lexer_destroy_fn = spy_lexer_destroy_fn
	});

	return 0;
}

static int destroy_teardown(void **state) {
	fake_free(g_lexer_spy);
	g_lexer_spy = NULL;
	runtime_session_reset_lexer_ops();
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
	assert_true(fake_memory_no_leak());
	set_allocators(NULL, NULL);
	set_reallocator(NULL);
	set_string_duplicate(NULL);
	fake_memory_reset();
	return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - session is properly initialized
//  - session->symbol_pool and session->st contain a symbol
// Expected:
//  - session is properly cleaned up
static void destroy_success_when_symbol_pool_not_empty(void **state) {
	(void)state;
	runtime_session *session = runtime_session_create();
	assert_int_equal(0, symtab_intern_symbol(session->st, "symbol_name"));
	symbol *sym = symtab_get(session->st, "symbol_name");
	assert_non_null(sym);
	assert_true(runtime_session_store_symbol(sym, session));
	assert_non_null(session->symbol_pool);

	runtime_session_destroy(session);
}

// Given:
//  - session is properly initialized
//  - session->ast_pool an ast
// Expected:
//  - session is properly cleaned up
static void destroy_success_when_ast_pool_not_empty(void **state) {
	(void)state;
	runtime_session *session = runtime_session_create();
	assert_non_null(session);
	ast *root = a_function_node();
	assert_non_null(root);
	assert_true(runtime_session_store_ast(root, session));
	assert_non_null(session->ast_pool);

	runtime_session_destroy(session);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
	const struct CMUnitTest create_tests[] = {
		cmocka_unit_test_setup_teardown(
			create_ret_null_when_oom,
			create_setup, create_teardown),
		cmocka_unit_test_setup_teardown(
			create_ret_not_null_when_no_oom,
			create_setup, create_teardown),
		cmocka_unit_test_setup_teardown(
			create_initialize_ctx_when_no_oom,
			create_setup, create_teardown),
	};

	const struct CMUnitTest destroy_tests[] = {
		cmocka_unit_test_setup_teardown(
			destroy_success_when_symbol_pool_not_empty,
			destroy_setup, destroy_teardown),
		cmocka_unit_test_setup_teardown(
			destroy_success_when_ast_pool_not_empty,
			destroy_setup, destroy_teardown),
	};

	int failed = 0;
	failed += cmocka_run_group_tests(create_tests, NULL, NULL);
	failed += cmocka_run_group_tests(destroy_tests, NULL, NULL);

	return failed;
}
