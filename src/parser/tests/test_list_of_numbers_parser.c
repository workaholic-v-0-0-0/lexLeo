// src/parser/tests/test_list_of_numbers_parser.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>

#include "list_of_numbers_parser.tab.h"
#include "parser_ctx.h"
#include "mock_lexer.h"
#include "list.h"



//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static ast *parsed_ast;

static const char DUMMY[3];
static ast *const DUMMY_AST_ERROR_OR_NUMBERS_NODE = (ast *) &DUMMY[0];
static ast *const DUMMY_AST_ERROR = (ast *) &DUMMY[1];
static ast *const DUMMY_AST_LIST_OF_NUMBERS_NODE = (ast *) &DUMMY[2];



//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


mock_token seq_list_of_numbers[] = {
    {LPAREN, {0}},
    {INTEGER, {.int_value = 7}},
    {INTEGER, {.int_value = 66}},
    {RPAREN, {0}},
    {0, {0}}
};

ast *stub_numbers_action(void) {
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

ast *mock_create_error_node_or_sentinel(ast_error_type code, char *message) {
    check_expected(code);
    check_expected(message);
    return mock_type(ast *);
}

void mock_destroy(ast *root) {
    check_expected(root);
}

parser_ctx mock_ctx;



//-----------------------------------------------------------------------------
// list_of_numbers_parse TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// Action under test via injected stubs:
// list_of_numbers: LPAREN numbers RPAREN

// mocked:
//  - actions of grammar rules that the rule under test depends on:
//    - numbers: | numbers number_atom
//    - number_atom: INTEGER
//  - functions of the ast module which are used:
//    - ast_create_children_node_var
//    - ast_create_error_node_or_sentinel
//    - ast_destroy
//  - function of the lexer module which are used:
//    - yylex



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int list_of_numbers_parse_setup(void **state) {
    (void)state;
    parsed_ast = NULL;
    mock_lex_reset();
    mock_lex_set(seq_list_of_numbers, 5);
    mock_ctx.ops.create_children_node_var = mock_create_children_node_var;
    mock_ctx.ops.create_error_node_or_sentinel = mock_create_error_node_or_sentinel;
    mock_ctx.ops.destroy = mock_destroy;
    return 0;
}

static int list_of_numbers_parse_teardown(void **state) {
    (void)state;
    mock_lex_reset();
    parsed_ast = NULL;
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// At any test:
// Given:
//  - lexer returns:
//    - LPAREN
//    - INTEGER(7)
//    - INTEGER(66)
//    - RPAREN
// Expected:
//  - action of the rule "numbers: | numbers number_atom" is executed,
//    this yields a numbers (semantic value provided here by a stub)
//  - calls ast_create_children_node_var with:
//    - type: AST_TYPE_LIST_OF_NUMBERS
//    - children_nb: 1
//    - first optionnal arg: $2 (numbers's semantic value)
//    - no more arg


// Given:
//  - ast_create_children_node_var will fail
// Expected:
//  - calls ast_destroy with:
//    - root: $2 (numbers's semantic value)
//  - calls create_error_node_or_sentinel with:
//    - code: AST_ERROR_CODE_LIST_OF_NUMBERS_NODE_CREATION_FAILED
//    - message: "ast creation for a node for a list of numbers failed"
//  - gives create_error_node_or_sentinel returned value for the LHS semantic value
static void list_of_numbers_parse_cleans_up_and_create_error_node_for_LHS_when_create_children_node_var_fails(void **state) {
    will_return(stub_numbers_action, DUMMY_AST_ERROR_OR_NUMBERS_NODE);
    expect_value(mock_create_children_node_var, type, AST_TYPE_LIST_OF_NUMBERS);
    expect_value(mock_create_children_node_var, children_nb, 1);
    expect_value(mock_create_children_node_var, child, DUMMY_AST_ERROR_OR_NUMBERS_NODE);
    will_return(mock_create_children_node_var, NULL);
    expect_value(mock_destroy, root, DUMMY_AST_ERROR_OR_NUMBERS_NODE);
    expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_LIST_OF_NUMBERS_NODE_CREATION_FAILED);
    expect_string(mock_create_error_node_or_sentinel, message, "ast creation for a node for a list of numbers failed");
    will_return(mock_create_error_node_or_sentinel, DUMMY_AST_ERROR);

    list_of_numbers_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR);
}

// Given:
//  - ast_create_children_node_var will succeed
// Expected:
//  - gives create_children_node_var returned value for the LHS semantic value
static void list_of_numbers_parse_creates_a_list_of_numbers_node_when_create_children_node_var_succeeds(void **state) {
    will_return(stub_numbers_action, DUMMY_AST_ERROR_OR_NUMBERS_NODE);
    expect_value(mock_create_children_node_var, type, AST_TYPE_LIST_OF_NUMBERS);
    expect_value(mock_create_children_node_var, children_nb, 1);
    expect_value(mock_create_children_node_var, child, DUMMY_AST_ERROR_OR_NUMBERS_NODE);
    will_return(mock_create_children_node_var, DUMMY_AST_LIST_OF_NUMBERS_NODE);

    list_of_numbers_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_LIST_OF_NUMBERS_NODE);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
    const struct CMUnitTest list_of_numbers_parse_tests[] = {
        cmocka_unit_test_setup_teardown(
            list_of_numbers_parse_cleans_up_and_create_error_node_for_LHS_when_create_children_node_var_fails,
            list_of_numbers_parse_setup, list_of_numbers_parse_teardown),
        cmocka_unit_test_setup_teardown(
            list_of_numbers_parse_creates_a_list_of_numbers_node_when_create_children_node_var_succeeds,
            list_of_numbers_parse_setup, list_of_numbers_parse_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(list_of_numbers_parse_tests, NULL, NULL);

    return failed;
}
