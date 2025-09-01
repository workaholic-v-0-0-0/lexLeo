// src/parser/tests/test_number_parser.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "number_parser.tab.h"
#include "parser_ctx.h"



//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static ast *parsed_ast;

static const char DUMMY[4];
static ast *const DUMMY_AST_P = (ast *) &DUMMY[0];
static ast *const DUMMY_ERROR_SENTINEL = (ast *) &DUMMY[1];
static ast *const DUMMY_AST_ERROR_NOT_SENTINEL_P = (ast *) &DUMMY[2];
static ast *const DUMMY_AST_NOT_ERROR_P = (ast *) &DUMMY[3];



//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


typedef struct {
	int tok;
	NUMBER_ATOM_STYPE yv;
} MockTok;

static const MockTok *g_seq = NULL;
static size_t g_len = 0;
static size_t g_idx = 0;

void mock_lex_set(const MockTok *seq, size_t len) {
	g_seq = seq;
	g_len = len;
	g_idx = 0;
}

void mock_lex_reset() {
	g_seq = NULL;
	g_len = 0;
	g_idx = 0;
}

static MockTok seq[] = {
	{ INTEGER, { .int_value = 5 } },
	{ 0,       { 0 } }
};

int yylex(NUMBER_ATOM_STYPE *yylval, void *yyscanner /* yyscan_t */) {
	(void)yyscanner;
	if (!g_seq || g_idx >= g_len) return 0;
	int tok = g_seq[g_idx].tok;
	if (tok != 0 && yylval) *yylval = g_seq[g_idx].yv;
	g_idx++;
	return tok;
}

ast *mock_create_int_node(int i) {
	check_expected(i);
    return mock_type(ast *);
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
// yyparse TESTS with INTEGER lexem inputs
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// "number_atom : INTEGER" action via injected ops

// mocked:
//  - functions of the ast module which are used:
//    - ast_create_int_node
//    - ast_create_error_node
//    - ast_error_sentinel
//  - function of the lexer module which are used:
//    - yylex



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int yyparse_with_INTEGER_input_setup(void **state) {
    (void)state;
    parsed_ast = NULL;
    mock_ctx.ops.create_int_node = mock_create_int_node;
    mock_ctx.ops.create_error_node = mock_create_error_node;
	mock_ctx.ops.error_sentinel = mock_error_sentinel;
	mock_lex_reset();
	mock_lex_set(seq, 2);
    return 0;
}

static int yyparse_with_INTEGER_input_teardown(void **state) {
    (void)state;
	mock_lex_reset();
    parsed_ast = NULL;
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// At every test
// Given: lexer returns INTEGER(5)
// Expected: calls ast_create_int_node(5)


// Given:
//  - create_int_node fails
//  - ast_create_error_node fails
// Expected:
//  - calls ast_error_sentinel
//  - gives sentinel error ast for the semantic value number_atom lexeme
static void yyparse_calls_ast_error_sentinel_and_returns_its_returned_value_when_ast_create_error_node_fails(void **state) {
	expect_value(mock_create_int_node, i, 5);
    will_return(mock_create_int_node, NULL);
	expect_any(mock_create_error_node, code);
	expect_any(mock_create_error_node, message);
    will_return(mock_create_error_node, NULL);
	will_return(mock_error_sentinel, DUMMY_ERROR_SENTINEL);

	number_atom_parse(NULL, &parsed_ast, &mock_ctx);

	assert_ptr_equal(parsed_ast, DUMMY_ERROR_SENTINEL);
}

// Given:
//  - create_int_node fails
//  - ast_create_error_node succeeds
// Expected:
//  - calls ast_create_error_node with:
//    - code = AST_ERROR_CODE_INT_NODE_CREATION_FAILED
//    - message = "ast creation for a number failed"
//  - gives the returned value of ast_create_error_node for the semantic value number_atom lexeme
static void yyparse_calls_ast_create_error_node_and_returns_its_returned_value_when_ast_create_error_node_succeeds(void **state) {
	expect_value(mock_create_int_node, i, 5);
	will_return(mock_create_int_node, NULL);
	expect_value(mock_create_error_node, code, AST_ERROR_CODE_INT_NODE_CREATION_FAILED);
	expect_string(mock_create_error_node, message, "ast creation for a number failed");
    will_return(mock_create_error_node, DUMMY_AST_ERROR_NOT_SENTINEL_P);

	number_atom_parse(NULL, &parsed_ast, &mock_ctx);

	assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_NOT_SENTINEL_P);
}

// Given:
//  - create_int_node succeeds
// Expected:
//  - gives the returned value of create_int_node for the semantic value number_atom lexeme
static void yyparse_returns_value_from_create_int_node_when_successful(void **state) {
	expect_value(mock_create_int_node, i, 5);
	will_return(mock_create_int_node, DUMMY_AST_P);

	number_atom_parse(NULL, &parsed_ast, &mock_ctx);

	assert_ptr_equal(parsed_ast, DUMMY_AST_P);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest yyparse_tests[] = {
        cmocka_unit_test_setup_teardown(
            yyparse_calls_ast_error_sentinel_and_returns_its_returned_value_when_ast_create_error_node_fails,
            yyparse_with_INTEGER_input_setup, yyparse_with_INTEGER_input_teardown),
        cmocka_unit_test_setup_teardown(
            yyparse_calls_ast_create_error_node_and_returns_its_returned_value_when_ast_create_error_node_succeeds,
            yyparse_with_INTEGER_input_setup, yyparse_with_INTEGER_input_teardown),
    	cmocka_unit_test_setup_teardown(
			yyparse_returns_value_from_create_int_node_when_successful,
			yyparse_with_INTEGER_input_setup, yyparse_with_INTEGER_input_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(yyparse_tests, NULL, NULL);

    return failed;
}
