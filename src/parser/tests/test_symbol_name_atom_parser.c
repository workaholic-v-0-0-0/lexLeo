// src/parser/tests/test_symbol_name_atom_parser.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "symbol_name_atom_parser.tab.h"
#include "parser_ctx.h"



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


typedef struct {
    int tok;
    SYMBOL_NAME_ATOM_STYPE yv;
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
    { SYMBOL_NAME, { .string_value = "symbol_name" } },
    { 0,       { 0 } }
};

int yylex(SYMBOL_NAME_ATOM_STYPE *yylval, void *yyscanner /* yyscan_t */) {
    (void)yyscanner;
    if (!g_seq || g_idx >= g_len) return 0;
    int tok = g_seq[g_idx].tok;
    if (tok != 0 && yylval) *yylval = g_seq[g_idx].yv;
    g_idx++;
    return tok;
}

ast *mock_create_symbol_name_node(char *str) {
    check_expected(str);
    return mock_type(ast *);
}

ast *mock_create_error_node_or_sentinel(error_type code, char *message) {
    check_expected(code);
    check_expected(message);
    return mock_type(ast *);
}

parser_ctx mock_ctx;



//-----------------------------------------------------------------------------
// yyparse TESTS with SYMBOL_NAME lexeme input
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// "symbol_name_atom : SYMBOL_NAME" action via injected ops

// mocked:
//  - functions of the ast module which are used:
//    - ast_create_symbol_name_node
//    - ast_create_error_node_or_sentinel
//  - function of the lexer module which are used:
//    - yylex



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int yyparse_with_SYMBOL_NAME_input_setup(void **state) {
    (void)state;
    parsed_ast = NULL;
    mock_ctx.ops.create_symbol_name_node = mock_create_symbol_name_node;
    mock_ctx.ops.create_error_node_or_sentinel = mock_create_error_node_or_sentinel;
    mock_lex_reset();
    mock_lex_set(seq, 2);
    return 0;
}

static int yyparse_with_SYMBOL_NAME_input_teardown(void **state) {
    (void)state;
    mock_lex_reset();
    parsed_ast = NULL;
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// At every test
// Given: lexer returns SYMBOL_NAME("symbol_name")
// Expected: calls ast_create_symbol_name_node("symbol_name")


// Given:
//  - create_symbol_name_node fails
// Expected:
//  - calls create_error_node_or_sentinel
//  - gives create_error_node_or_sentinel returned value for the semantic value of the symbol_name_atom lexeme
static void yyparse_calls_create_error_node_or_sentinel_and_returns_its_returned_value_when_create_symbol_name_node_fails(void **state) {
    expect_string(mock_create_symbol_name_node, str, "symbol_name");
    will_return(mock_create_symbol_name_node, NULL);
    expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_SYMBOL_NAME_NODE_CREATION_FAILED);
    expect_string(mock_create_error_node_or_sentinel, message, "ast creation for a symbol name failed");
    will_return(mock_create_error_node_or_sentinel, DUMMY_AST_ERROR_P);

    symbol_name_atom_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_P);
}

// Given:
//  - create_symbol_name_node succeeds
// Expected:
//  - gives create_symbol_name_node returned value for the semantic value of the symbol_name_atom lexeme
static void yyparse_calls_create_symbol_name_node_and_returns_its_returned_value_when_create_symbol_name_node_succeeds(void **state) {
    expect_string(mock_create_symbol_name_node, str, "symbol_name");
    will_return(mock_create_symbol_name_node, DUMMY_AST_NOT_ERROR_P);

    symbol_name_atom_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_NOT_ERROR_P);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest yyparse_tests[] = {
        cmocka_unit_test_setup_teardown(
            yyparse_calls_create_error_node_or_sentinel_and_returns_its_returned_value_when_create_symbol_name_node_fails,
            yyparse_with_SYMBOL_NAME_input_setup, yyparse_with_SYMBOL_NAME_input_teardown),
        cmocka_unit_test_setup_teardown(
            yyparse_calls_create_symbol_name_node_and_returns_its_returned_value_when_create_symbol_name_node_succeeds,
            yyparse_with_SYMBOL_NAME_input_setup, yyparse_with_SYMBOL_NAME_input_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(yyparse_tests, NULL, NULL);

    return failed;
}
