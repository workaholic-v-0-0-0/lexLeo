// src/ports/parser/tests/test_string_atom_parser.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "string_atom_parser.tab.h"
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
    {STRING, {.string_value = "chaine"}},
    {0, {0}}
};

static void dup_semantic(STYPE *dst, const STYPE *src, int tok) {
    *dst = *src; // copy POD fields first
    if (tok == STRING) {
        // strdup ONLY because the grammar will %destructor free($$) STRING
        dst->string_value = strdup(src->string_value);
    }
}

static char *g_spy_create_string_node_arg_str = NULL;
ast *mock_create_string_node(const char *str) {
    g_spy_create_string_node_arg_str = (str ? strdup(str) : NULL);
    return mock_type(ast *);
}

ast *mock_create_error_node_or_sentinel(ast_error_type code, const char *message) {
    check_expected(code);
    check_expected(message);
    return mock_type(ast *);
}

parser_cfg mock_ctx;



//-----------------------------------------------------------------------------
// string_atom_parse TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// "string_atom : STRING" action via injected ops

// mocked:
//  - functions of the ast module which are used:
//    - ast_create_string_node
//    - ast_create_error_node_or_sentinel
//  - function of the lexer module which are used:
//    - yylex


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int string_atom_parse_setup(void **state) {
    (void) state;
    parsed_ast = NULL;
    mock_ctx.ops.create_string_node = mock_create_string_node;
    g_spy_create_string_node_arg_str = NULL;
    mock_ctx.ops.create_error_node_or_sentinel = mock_create_error_node_or_sentinel;
    mock_lex_reset();
    mock_lex_set_dupper(dup_semantic);
    mock_lex_set(seq, 2);
    return 0;
}

static int string_atom_parse_teardown(void **state) {
    (void) state;
    mock_lex_reset();
    parsed_ast = NULL;
    free(g_spy_create_string_node_arg_str);
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// At every tests
// Given: lexer returns STRING("chaine")
// Expected: calls ast_create_string_node("chaine")


// Given:
//  - create_string_node fails
// Expected:
//  - calls create_error_node_or_sentinel
//  - gives create_error_node_or_sentinel returned value for the semantic value string_atom lexeme
static void
string_atom_parse_calls_create_error_node_or_sentinel_and_returns_its_returned_value_when_create_string_node_fails(
    void **state) {
    will_return(mock_create_string_node, NULL);
    expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_STRING_NODE_CREATION_FAILED);
    expect_string(mock_create_error_node_or_sentinel, message, "ast creation for a string failed");
    will_return(mock_create_error_node_or_sentinel, DUMMY_AST_ERROR_P);

    string_atom_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_P);
    assert_string_equal(g_spy_create_string_node_arg_str, "chaine");
}

// Given:
//  - create_string_node succeeds
// Expected:
//  - gives create_string_node returned value for the semantic value string_atom lexeme
static void string_atom_parse_calls_create_string_node_and_returns_its_returned_value_when_create_string_node_succeeds(
    void **state) {
    will_return(mock_create_string_node, DUMMY_AST_NOT_ERROR_P);

    string_atom_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_NOT_ERROR_P);
    assert_string_equal(g_spy_create_string_node_arg_str, "chaine");
}


//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest string_atom_parse_tests[] = {
        cmocka_unit_test_setup_teardown(
            string_atom_parse_calls_create_error_node_or_sentinel_and_returns_its_returned_value_when_create_string_node_fails,
            string_atom_parse_setup, string_atom_parse_teardown),
        cmocka_unit_test_setup_teardown(
            string_atom_parse_calls_create_string_node_and_returns_its_returned_value_when_create_string_node_succeeds,
            string_atom_parse_setup, string_atom_parse_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(string_atom_parse_tests, NULL, NULL);

    return failed;
}