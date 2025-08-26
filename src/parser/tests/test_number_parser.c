// src/parser/tests/test_number_parser.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>


#include "parser.tab.h"
#include "parser_ctx.h"
#include "lexer.yy.h"



//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static yyscan_t scanner;
static YY_BUFFER_STATE buf;
static ast *parsed_ast;

static const char DUMMY[3];
static const typed_data *const DUMMY_TYPED_DATA_P = (typed_data *) &DUMMY[0];
static ast *const DUMMY_AST_P = (ast *) &DUMMY[1];
static ast *const DUMMY_ERROR_SENTINEL = (ast *) &DUMMY[2];



//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


static typed_data *mock_create_typed_data_int(int i) {
	check_expected(i);
    return mock_type(typed_data *);
}

ast *mock_create_error_node(error_type code, char *message) {
	check_expected(code);
	check_expected(message);
    return mock_type(ast *);
}

ast *mock_error_sentinel(void) {
    return mock_type(ast *);
}

parser_ctx mock_ctx;



//-----------------------------------------------------------------------------
// yyparse TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int yyparse_setup(void **state) {
    (void)state;
    assert_int_equal(yylex_init(&scanner), 0);
	buf = NULL;
    parsed_ast = NULL;
    mock_ctx.ops.create_typed_data_int = mock_create_typed_data_int;
    mock_ctx.ops.create_error_node = mock_create_error_node;
	mock_ctx.ops.error_sentinel = mock_error_sentinel;
    return 0;
}

static int yyparse_teardown(void **state) {
    (void)state;
    if (buf) {
        yy_delete_buffer(buf, scanner);
        buf = NULL;
    }
    yylex_destroy(scanner);
    parsed_ast = NULL;
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// At every test
// Given: lexer returns INTEGER(5)
// Expected: calls ast_create_typed_data_int(5)


// Given:
//  - ast_create_typed_data_int fails
//  - ast_create_error_node fails
// Expected:
//  - calls ast_error_sentinel
//  - give sentinel error ast for the semantic value of the left lexem
static void yyparse_calls_ast_error_sentinel_and_returns_its_returned_value_when_ast_create_error_node_fails(void **state) {

	buf = yy_scan_string("5", scanner);

	expect_value(mock_create_typed_data_int, i, 5);
    will_return(mock_create_typed_data_int, NULL);
	expect_any(mock_create_error_node, code);
	expect_any(mock_create_error_node, message);
    will_return(mock_create_error_node, NULL);

	will_return(mock_error_sentinel, DUMMY_ERROR_SENTINEL);

	yyparse(scanner, &parsed_ast, &mock_ctx);
}







/*
static void yyparse_calls_ast_error_sentinel_and_returns_its_returned_value_when_ast_create_error_node_fails(void **state) {
	buf = yy_scan_string("5", scanner);
	expect_value(mock_create_typed_data_int, i, 5);
    will_return(mock_create_typed_data_int, NULL);
	expect_value(mock_create_error_node, code, MEMORY_ALLOCATION_ERROR_CODE);
	expect_string(mock_create_error_node, message, "Data wrapper creation for a number failed.");
    will_return(mock_create_error_node, DUMMY_AST_P);
	yyparse(scanner, &parsed_ast, &mock_ctx);
}
*/




// Given: lexer gives lexem INTEGER with value 5
// Expected: ast_data_wrapper with int 5
/* should not have been written yet for one wants a TDD approach
static void yyparse_build_right_ast_when_given_an_int(void **state) { // ICI
    printf("here1\n");
    ast *parsed_ast = NULL;
    ast **ret = &parsed_ast;
    printf("here2\n");
    yyscan_t scanner;
    yylex_init(&scanner);
	parser_ctx ctx = make_default_ctx();
    YY_BUFFER_STATE buf = yy_scan_string("5", scanner);
    printf("here3\n");
    int result = yyparse(scanner, ret, &ctx);
    printf("result: %i\n", result);
    printf("ret: %p\n", ret);
    printf("*ret: %p\n", *ret);
    printf("ret->type: %i\n", (*(ast **)ret)->type);
    printf("AST_TYPE_DATA_WRAPPER: %i\n", AST_TYPE_DATA_WRAPPER);
    printf("ret->data->type: %i\n", (*(ast **)ret)->data->type);
    printf("TYPE_INT: %i\n", TYPE_INT);
    printf("ret->data->data.int_value: %i\n", (*(ast **)ret)->data->data.int_value);
    yy_delete_buffer(buf, scanner);
    yylex_destroy(scanner);
}
*/

// Given:
// Expected:
// with a create_typed_data_int double
/* should not have been written yet for one wants a TDD approach
static void yyparse__when_given_an_int_and_create_typed_data_int_fails(void **state) { // ICI
    ast *parsed_ast = NULL;
    ast **ret = &parsed_ast;
    yyscan_t scanner;
    yylex_init(&scanner);
	parser_ctx ctx = { .ops = { .create_typed_data_int = mock_create_typed_data_int } };
	YY_BUFFER_STATE buf = yy_scan_string("5", scanner);

	expect_value(mock_create_typed_data_int, i, 5);
    will_return(mock_create_typed_data_int, NULL);

    int result = yyparse(scanner, ret, &ctx);
    yy_delete_buffer(buf, scanner);
    yylex_destroy(scanner);
}
*/



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest yyparse_tests[] = {
        cmocka_unit_test_setup_teardown(
            yyparse_calls_ast_error_sentinel_and_returns_its_returned_value_when_ast_create_error_node_fails,
            yyparse_setup, yyparse_teardown),
/* should not have been written yet for one wants a TDD approach
        cmocka_unit_test_setup_teardown(
            yyparse_build_right_ast_when_given_an_int,
            yyparse_setup, yyparse_teardown),
        cmocka_unit_test_setup_teardown(
            yyparse__when_given_an_int_and_create_typed_data_int_fails,
            yyparse_setup, yyparse_teardown),
*/
    };
    int failed = 0;
    failed += cmocka_run_group_tests(yyparse_tests, NULL, NULL);

    return failed;
}
