// src/parser/tests/test_number_atom_parser.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "number_atom_parser.tab.h"
#include "parser_ctx.h"
#include "mock_lexer.h"



//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static ast *parsed_ast;

static const char DUMMY[2];
static ast *const DUMMY_AST_ERROR_P = (ast *) &DUMMY[0];
static ast *const DUMMY_AST_NOT_ERROR_P = (ast *) &DUMMY[1];



//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------

static mock_token seq[] = {
	{ INTEGER, { .int_value = 5 } },
	{ 0,       { 0 } }
};

ast *mock_create_int_node(int i) {
	check_expected(i);
    return mock_type(ast *);
}

ast *mock_create_error_node_or_sentinel(error_type code, char *message) {
	check_expected(code);
	check_expected(message);
    return mock_type(ast *);
}

parser_ctx mock_ctx;



//-----------------------------------------------------------------------------
// number_atom_parse TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// "number_atom : INTEGER" action via injected ops

// mocked:
//  - functions of the ast module which are used:
//    - ast_create_int_node
//    - ast_create_error_node_or_sentinel
//  - function of the lexer module which are used:
//    - yylex



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int number_atom_parse_setup(void **state) {
    (void)state;
    parsed_ast = NULL;
    mock_ctx.ops.create_int_node = mock_create_int_node;
    mock_ctx.ops.create_error_node_or_sentinel = mock_create_error_node_or_sentinel;
		mock_lex_reset();
		mock_lex_set(seq, 2);
    return 0;
}

static int number_atom_parse_teardown(void **state) {
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
// Expected:
//  - calls create_error_node_or_sentinel
//  - gives create_error_node_or_sentinel returned value for the semantic value number_atom lexeme
static void number_atom_parse_calls_create_error_node_or_sentinel_and_returns_its_returned_value_when_create_int_node_fails(void **state) {
	expect_value(mock_create_int_node, i, 5);
  will_return(mock_create_int_node, NULL);
	expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_INT_NODE_CREATION_FAILED);
	expect_string(mock_create_error_node_or_sentinel, message, "ast creation for a number failed");
	will_return(mock_create_error_node_or_sentinel, DUMMY_AST_ERROR_P);

	number_atom_parse(NULL, &parsed_ast, &mock_ctx);

	assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_P);
}

// Given:
//  - create_int_node succeeds
// Expected:
//  - gives create_int_node returned value for the semantic value number_atom lexeme
static void number_atom_parse_calls_create_int_node_and_returns_its_returned_value_when_create_int_node_succeeds(void **state) {
	expect_value(mock_create_int_node, i, 5);
	will_return(mock_create_int_node, DUMMY_AST_NOT_ERROR_P);

	number_atom_parse(NULL, &parsed_ast, &mock_ctx);

	assert_ptr_equal(parsed_ast, DUMMY_AST_NOT_ERROR_P);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest number_atom_parse_tests[] = {
        cmocka_unit_test_setup_teardown(
            number_atom_parse_calls_create_error_node_or_sentinel_and_returns_its_returned_value_when_create_int_node_fails,
            number_atom_parse_setup, number_atom_parse_teardown),
        cmocka_unit_test_setup_teardown(
            number_atom_parse_calls_create_int_node_and_returns_its_returned_value_when_create_int_node_succeeds,
            number_atom_parse_setup, number_atom_parse_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(number_atom_parse_tests, NULL, NULL);

    return failed;
}
