// src/parser/tests/test_parser.tab.c

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


// Given:
// Expected:
static void yyparse__when_(void **state) { // ICI
    ast **ret;
    context ctx = malloc(sizeof(struct context));
    ctx->st = NULL;
    yyscan_t scanner;
    yylex_init(&scanner);
    yyset_extra(ctx, scanner);
    const char *src = "a + b * c";
    YY_BUFFER_STATE buf = yy_scan_string(src, scanner);

    int result = yyparse(scanner, ret);

    yy_delete_buffer(buf, scanner);
    yylex_destroy(scanner);
    //yyparse(NULL);
}

// Given:
// Expected:
static void yyparse__when_2(void **state) { // ICI
    ast *parsed_ast = NULL;
    ast **ret = &parsed_ast;
    context ctx = malloc(sizeof(struct context));
    ctx->st = NULL;
    yyscan_t scanner;
    yylex_init(&scanner);
    yyset_extra(ctx, scanner);
    YY_BUFFER_STATE buf = yy_scan_string("5", scanner);
    int result = yyparse(scanner, ret);
    yy_delete_buffer(buf, scanner);
    yylex_destroy(scanner);
    printf("ret->type: %i\n", (*(ast **)ret)->type);
    printf("AST_TYPE_DATA_WRAPPER: %i\n", AST_TYPE_DATA_WRAPPER);
    printf("ret->data->type: %i\n", (*(ast **)ret)->data->type);
    printf("TYPE_INT: %i\n", TYPE_INT);
    //printf("ret->data->data.int_value: %i\n", (*(ast **)ret)->data->data.int_value);
}






//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest yyparse_tests[] = {
/*
        cmocka_unit_test_setup_teardown(
            yyparse__when_,
            yyparse_setup, yyparse_teardown),
*/
        cmocka_unit_test_setup_teardown(
            yyparse__when_2,
            yyparse_setup, yyparse_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(yyparse_tests, NULL, NULL);

    return failed;
}
