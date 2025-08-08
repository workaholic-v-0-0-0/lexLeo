// src/parser/tests/test_atom_parser.tab.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "logger.h"
#include "list.h"
#include "ast.h"
#include "symtab.h"

#include "parser.tab.h"
#include "lexer.yy.h"



//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// yyparse TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int yyparse_setup(void **state) {
    printf("here0\n");
    //set_allocators(mock_malloc, mock_free);
    return 0;
}

static int yyparse_teardown(void **state) {
    //set_allocators(NULL, NULL);
    //free_saved_addresses_to_be_freed();
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given: lexer gives int 5
// Expected: ast_data_wrapper with int 5
static void yyparse_build_right_ast_when_given_an_int(void **state) { // ICI
    symtab *st = symtab_wind_scope(NULL);
    ast *parsed_ast = NULL;
    ast **ret = &parsed_ast;
    context ctx = malloc(sizeof(struct context));
    ctx->st = NULL;
    yyscan_t scanner;
    yylex_init(&scanner);
    yyset_extra(ctx, scanner);
    YY_BUFFER_STATE buf = yy_scan_string("5", scanner);
    printf("here5\n");
    printf("ret: %p\n", ret);
    printf("*ret: %p\n", *ret);
    int result = yyparse(scanner, ret);
    printf("result: %i\n", result);
    printf("ret: %p\n", ret);
    printf("*ret: %p\n", *ret);
    yy_delete_buffer(buf, scanner);
    yylex_destroy(scanner);
    printf("ret->type: %i\n", (*(ast **)ret)->type);
    printf("AST_TYPE_DATA_WRAPPER: %i\n", AST_TYPE_DATA_WRAPPER);
    printf("ret->data->type: %i\n", (*(ast **)ret)->data->type);
    printf("TYPE_INT: %i\n", TYPE_INT);
    printf("ret->data->data.int_value: %i\n", (*(ast **)ret)->data->data.int_value);
    symtab_unwind_scope(st);
}

// Given: lexer gives string "symbol_name"
// Expected: ast_data_wrapper with symbol... and in symbol table...
static void yyparse_build_right_ast_and_update_symbol_table_when_given_a_symbol_name(void **state) { // ICI
    symtab *st = symtab_wind_scope(NULL);
	symbol *s = malloc(sizeof(struct symbol));
	s->name = strdup("symbol_name");
	s->image = NULL;
    assert_null(
        symtab_add(
            st,
			s
             ) );
    ast *parsed_ast = NULL;
    ast **ret = &parsed_ast;
    context ctx = malloc(sizeof(struct context));
    ctx->st = st;
    yyscan_t scanner;
    yylex_init(&scanner);
    yyset_extra(ctx, scanner);
    YY_BUFFER_STATE buf = yy_scan_string("symbol_name", scanner);
    printf("here5\n");
    printf("ret: %p\n", ret);
    printf("*ret: %p\n", *ret);
    int result = yyparse(scanner, ret);
    printf("result: %i\n", result);
    printf("ret: %p\n", ret);
    printf("*ret: %p\n", *ret);
    yy_delete_buffer(buf, scanner);
    yylex_destroy(scanner);
    printf("ret->type: %i\n", (*(ast **)ret)->type);
    printf("AST_TYPE_DATA_WRAPPER: %i\n", AST_TYPE_DATA_WRAPPER);
    printf("ret->data->type: %i\n", (*(ast **)ret)->data->type);
    printf("TYPE_SYMBOL: %i\n", TYPE_SYMBOL);
	printf("s: %p\n", s);
    printf("ret->data->data.symbol_value: %p\n", (*(ast **)ret)->data->data.symbol_value);
    symtab_unwind_scope(st);
}






//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest yyparse_tests[] = {
        cmocka_unit_test_setup_teardown(
            yyparse_build_right_ast_when_given_an_int,
            yyparse_setup, yyparse_teardown),
        cmocka_unit_test_setup_teardown(
            yyparse_build_right_ast_and_update_symbol_table_when_given_a_symbol_name,
            yyparse_setup, yyparse_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(yyparse_tests, NULL, NULL);

    return failed;
}
