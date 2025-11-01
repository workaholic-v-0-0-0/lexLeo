// src/parser/tests/test_reading_parser.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>

#include "reading_parser.tab.h"
#include "parser_ctx.h"
#include "mock_lexer.h"


//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static ast *parsed_ast;

static const char DUMMY[3];
static ast *const DUMMY_AST_P = (ast *) &DUMMY[0];
static ast *const DUMMY_AST_ERROR_P = (ast *) &DUMMY[1];
static ast *const DUMMY_AST_ERROR_OR_SYMBOL_NAME_DATA_WRAPPER_P = (ast *) &DUMMY[2];


//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


mock_token seq[] = {
    {READ, {0}},
    {SYMBOL_NAME, {.symbol_name_value = "symbol_name"}},
    {SEMICOLON, {0}},
    {0, {0}}
};

ast *stub_symbol_name_atom_action(char *symbol_name) {
    check_expected(symbol_name);
    return mock_type(ast *);
}

ast *mock_create_children_node_var(ast_type type, size_t children_nb, ...) {
    check_expected(type);
    check_expected(children_nb);

    va_list ap;
    va_start(ap, children_nb);

    for (size_t i = 0; i < children_nb; i++) {
        ast *child = va_arg(ap, ast *);
        check_expected_ptr(child);
    }

    va_end(ap);

    return mock_type(ast *);
}

void mock_destroy(ast *root) {
    check_expected(root);
}

ast *mock_create_error_node_or_sentinel(ast_error_type code, const char *message) {
    check_expected(code);
    check_expected(message);
    return mock_type(ast *);
}

parser_ctx mock_ctx;


//-----------------------------------------------------------------------------
// reading_parse TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// Action under tests via injected stubs:
// reading: READ symbol_name_atom SEMICOLON

// mocked:
//  - action of grammar rule that the rule under tests depends on:
//    - symbol_name_atom: SYMBOL_NAME
// mocked:
//  - functions of the ast module which are used:
//    - ast_create_children_node_var
//    - ast_create_error_node_or_sentinel
//  - function of the lexer module which are used:
//    - yylex


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int reading_parse_setup(void **state) {
    (void) state;
    parsed_ast = NULL;
    mock_ctx.ops.create_children_node_var = mock_create_children_node_var;
    mock_ctx.ops.destroy = mock_destroy;
    mock_ctx.ops.create_error_node_or_sentinel = mock_create_error_node_or_sentinel;
    mock_lex_reset();
    mock_lex_set(seq, 4);
    return 0;
}

static int reading_parse_teardown(void **state) {
    (void) state;
    mock_lex_reset();
    parsed_ast = NULL;
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// At every tests
// Given:
//  - lexer returns:
//    - READ
//    - SYMBOL_NAME("symbol_name")
//    - SEMICOLON
// Expected:
//  - calls _create_children_node_var with:
//    - type: AST_TYPE_READING
//    - children_nb: 1
//    - first optionnal arg: $2 (symbol_name_atom's semantic value)
//    - no more arg

// Given:
//  - ast_create_children_node_var will fail
// Expected:
//  - calls ast_destroy with:
//    - root: $2 (symbol_name_atom's semantic value)
//  - calls create_error_node_or_sentinel with:
//    - code: AST_ERROR_CODE_READING_NODE_CREATION_FAILED
//    - message: "ast creation for a reading node failed"
//  - gives create_error_node_or_sentinel returned value for the LHS semantic value
static void reading_parse_cleans_up_and_create_error_node_for_LHS_semantic_value_when_create_children_node_var_fails(
    void **state) {
    expect_string(stub_symbol_name_atom_action, symbol_name, "symbol_name");
    will_return(stub_symbol_name_atom_action, DUMMY_AST_ERROR_OR_SYMBOL_NAME_DATA_WRAPPER_P);
    expect_value(mock_create_children_node_var, type, AST_TYPE_READING);
    expect_value(mock_create_children_node_var, children_nb, 1);
    expect_value(mock_create_children_node_var, child, DUMMY_AST_ERROR_OR_SYMBOL_NAME_DATA_WRAPPER_P);
    will_return(mock_create_children_node_var, NULL);
    expect_value(mock_destroy, root, DUMMY_AST_ERROR_OR_SYMBOL_NAME_DATA_WRAPPER_P);
    expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_READING_NODE_CREATION_FAILED);
    expect_string(mock_create_error_node_or_sentinel, message, "ast creation for a reading node failed");
    will_return(mock_create_error_node_or_sentinel, DUMMY_AST_ERROR_P);

    reading_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_P);
}

// Given:
//  - ast_create_children_node_var will succeed
// Expected:
//  - gives ast_create_children_node_var returned value for the LHS semantic value
static void reading_parse_create_reading_node_for_LHS_semantic_value_when_create_children_node_var_succeeds(
    void **state) {
    expect_string(stub_symbol_name_atom_action, symbol_name, "symbol_name");
    will_return(stub_symbol_name_atom_action, DUMMY_AST_ERROR_OR_SYMBOL_NAME_DATA_WRAPPER_P);
    expect_value(mock_create_children_node_var, type, AST_TYPE_READING);
    expect_value(mock_create_children_node_var, children_nb, 1);
    expect_value(mock_create_children_node_var, child, DUMMY_AST_ERROR_OR_SYMBOL_NAME_DATA_WRAPPER_P);
    will_return(mock_create_children_node_var, DUMMY_AST_P);

    reading_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_P);
}


//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest reading_parse_tests[] = {
        cmocka_unit_test_setup_teardown(
            reading_parse_cleans_up_and_create_error_node_for_LHS_semantic_value_when_create_children_node_var_fails,
            reading_parse_setup, reading_parse_teardown),
        cmocka_unit_test_setup_teardown(
            reading_parse_create_reading_node_for_LHS_semantic_value_when_create_children_node_var_succeeds,
            reading_parse_setup, reading_parse_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(reading_parse_tests, NULL, NULL);

    return failed;
}