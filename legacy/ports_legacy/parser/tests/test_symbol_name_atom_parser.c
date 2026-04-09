// src/ports/parser/tests/test_symbol_name_atom_parser.c

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
#include "parser_cfg.h"
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
    {SYMBOL_NAME, {.string_value = "symbol_name"}},
    {0, {0}}
};

static void dup_semantic(STYPE *dst, const STYPE *src, int tok) {
    *dst = *src; // copy POD fields first
    if (tok == SYMBOL_NAME) {
        // strdup ONLY because the grammar will %destructor free($$) STRING
        dst->string_value = strdup(src->string_value);
    }
}

static char *g_spy_create_symbol_name_node_arg_str = NULL;
ast *mock_create_symbol_name_node(const char *str) {
    g_spy_create_symbol_name_node_arg_str = (str ? strdup(str) : NULL);
    return mock_type(ast *);
}

ast *mock_create_error_node_or_sentinel(ast_error_type code, const char *message) {
    check_expected(code);
    check_expected(message);
    return mock_type(ast *);
}

parser_cfg mock_ctx;


//-----------------------------------------------------------------------------
// symbol_name_atom_parse TESTS
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


static int symbol_name_atom_parse_setup(void **state) {
    (void) state;
    parsed_ast = NULL;
    mock_ctx.ops.create_symbol_name_node = mock_create_symbol_name_node;
    g_spy_create_symbol_name_node_arg_str = NULL;
    mock_ctx.ops.create_error_node_or_sentinel = mock_create_error_node_or_sentinel;
    mock_lex_reset();
    mock_lex_set_dupper(dup_semantic);
    mock_lex_set(seq, 2);
    return 0;
}

static int symbol_name_atom_parse_teardown(void **state) {
    (void) state;
    mock_lex_reset();
    parsed_ast = NULL;
    free(g_spy_create_symbol_name_node_arg_str);
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// At every tests
// Given: lexer returns SYMBOL_NAME("symbol_name")
// Expected: calls ast_create_symbol_name_node("symbol_name")


// Given:
//  - create_symbol_name_node fails
// Expected:
//  - calls create_error_node_or_sentinel
//  - gives create_error_node_or_sentinel returned value for the semantic value of the symbol_name_atom lexeme
static void
symbol_name_atom_parse_calls_create_error_node_or_sentinel_and_returns_its_returned_value_when_create_symbol_name_node_fails(
    void **state) {
    will_return(mock_create_symbol_name_node, NULL);
    expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_SYMBOL_NAME_NODE_CREATION_FAILED);
    expect_string(mock_create_error_node_or_sentinel, message, "ast creation for a symbol name failed");
    will_return(mock_create_error_node_or_sentinel, DUMMY_AST_ERROR_P);

    symbol_name_atom_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_P);
    assert_string_equal(g_spy_create_symbol_name_node_arg_str, "symbol_name");
}

// Given:
//  - create_symbol_name_node succeeds
// Expected:
//  - gives create_symbol_name_node returned value for the semantic value of the symbol_name_atom lexeme
static void
symbol_name_atom_parse_calls_create_symbol_name_node_and_returns_its_returned_value_when_create_symbol_name_node_succeeds(
    void **state) {
    will_return(mock_create_symbol_name_node, DUMMY_AST_NOT_ERROR_P);

    symbol_name_atom_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_NOT_ERROR_P);
    assert_string_equal(g_spy_create_symbol_name_node_arg_str, "symbol_name");
}


//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest symbol_name_atom_parse_tests[] = {
        cmocka_unit_test_setup_teardown(
            symbol_name_atom_parse_calls_create_error_node_or_sentinel_and_returns_its_returned_value_when_create_symbol_name_node_fails,
            symbol_name_atom_parse_setup, symbol_name_atom_parse_teardown),
        cmocka_unit_test_setup_teardown(
            symbol_name_atom_parse_calls_create_symbol_name_node_and_returns_its_returned_value_when_create_symbol_name_node_succeeds,
            symbol_name_atom_parse_setup, symbol_name_atom_parse_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(symbol_name_atom_parse_tests, NULL, NULL);

    return failed;
}