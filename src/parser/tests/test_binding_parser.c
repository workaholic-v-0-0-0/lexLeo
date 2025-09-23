// src/parser/tests/test_binding_parser.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>

#include "binding_parser.tab.h"
#include "parser_ctx.h"
#include "mock_lexer.h"


//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static ast *parsed_ast;

static const char DUMMY[4];
static ast *const DUMMY_BINDING_NODE = (ast *) &DUMMY[0];
static ast *const DUMMY_AST_ERROR_P = (ast *) &DUMMY[1];
static ast *const DUMMY_AST_ERROR_OR_EVALUABLE_NODE = (ast *) &DUMMY[2];
static ast *const DUMMY_AST_ERROR_OR_SYMBOL_NAME_NODE = (ast *) &DUMMY[3];


//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


mock_token seq_binding[] = {
    {SYMBOL_NAME, {.symbol_name_value = "symbol_name"}},
    {EQUAL, {0}},
    {INTEGER, {.int_value = 7}},
    {SEMICOLON, {0}},
    {0, {0}}
};

ast *stub_symbol_name_atom_action(void) {
    return mock_type(ast *);
}

ast *stub_evaluable_action(void) {
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

ast *mock_create_error_node_or_sentinel(ast_error_type code, char *message) {
    check_expected(code);
    check_expected(message);
    return mock_type(ast *);
}

parser_ctx mock_ctx;



//-----------------------------------------------------------------------------
// binding_parse TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// Action under test via injected stubs:
// binding: symbol_name_atom EQUAL evaluable SEMICOLON

// mocked:
//  - actions of grammar rules that the rule under test depends on:
//    - symbol_name_atom: SYMBOL_NAME
//    - evaluable: function_call | atom | computable | QUOTE evaluable
//    - function_call: symbol_name_atom list_of_numbers
//    - symbol_name_atom: SYMBOL_NAME
//    - list_of_numbers: LPAREN numbers RPAREN
//    - numbers: | numbers number_atom
//    - number_atom: INTEGER
//    - atom : number_atom | string_atom | symbol_name_atom
//    - string_atom: STRING
// mocked:
//  - functions of the ast module which are used:
//    - ast_create_children_node_var
//    - ast_create_error_node_or_sentinel
//    - ast_destroy
//  - function of the lexer module which are used:
//    - yylex



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int binding_parse_setup(void **state) {
    (void) state;
    parsed_ast = NULL;
    mock_ctx.ops.create_children_node_var = mock_create_children_node_var;
    mock_ctx.ops.destroy = mock_destroy;
    mock_ctx.ops.create_error_node_or_sentinel = mock_create_error_node_or_sentinel;
    mock_lex_reset();
    mock_lex_set(seq_binding, 5);
    return 0;
}

static int binding_parse_teardown(void **state) {
    (void) state;
    mock_lex_reset();
    parsed_ast = NULL;
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// At every test
// Given:
//  - lexer returns:
//    - SYMBOL_NAME("symbol_name")
//    - EQUAL
//    - INTEGER(7)
//    - SEMICOLON
// Expected:
//  - calls ast_create_children_node_var with:
//    - type: AST_TYPE_BINDING
//    - children_nb: 2
//    - first optionnal arg: symbol_name_atom's semantic value
//    - second optionnal arg: evaluable's semantic value
//    - no more arg

// Given:
//  - ast_create_children_node_var will fail
// Expected:
//  - calls ast_destroy with:
//    - root: $1 (symbol_name_atom's semantic value)
//  - calls ast_destroy with:
//    - root: $3 (evaluable's semantic value)
//  - calls create_error_node_or_sentinel with:
//    - code: AST_ERROR_CODE_BINDING_NODE_CREATION_FAILED
//    - message: "ast creation for a binding node failed"
//  - gives create_error_node_or_sentinel returned value for the LHS semantic value
static void binding_parse_cleans_up_and_create_error_node_for_LHS_semantic_value_when_create_children_node_var_fails(
    void **state) {
    will_return(stub_symbol_name_atom_action, DUMMY_AST_ERROR_OR_SYMBOL_NAME_NODE);
    will_return(stub_evaluable_action, DUMMY_AST_ERROR_OR_EVALUABLE_NODE);
    expect_value(mock_create_children_node_var, type, AST_TYPE_BINDING);
    expect_value(mock_create_children_node_var, children_nb, 2);
    expect_value(mock_create_children_node_var, child, DUMMY_AST_ERROR_OR_SYMBOL_NAME_NODE);
    expect_value(mock_create_children_node_var, child, DUMMY_AST_ERROR_OR_EVALUABLE_NODE);
    will_return(mock_create_children_node_var, NULL);
    expect_value(mock_destroy, root, DUMMY_AST_ERROR_OR_SYMBOL_NAME_NODE);
    expect_value(mock_destroy, root, DUMMY_AST_ERROR_OR_EVALUABLE_NODE);
    expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_BINDING_NODE_CREATION_FAILED);
    expect_string(mock_create_error_node_or_sentinel, message, "ast creation for a binding node failed");
    will_return(mock_create_error_node_or_sentinel, DUMMY_AST_ERROR_P);

    binding_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_P);
}

// Given:
//  - ast_create_children_node_var will succeed
// Expected:
//  - gives ast_create_children_node_var returned value for the LHS semantic value
static void binding_parse_create_binding_node_for_LHS_semantic_value_when_create_children_node_var_succeeds(void **state) {
    will_return(stub_symbol_name_atom_action, DUMMY_AST_ERROR_OR_SYMBOL_NAME_NODE);
    will_return(stub_evaluable_action, DUMMY_AST_ERROR_OR_EVALUABLE_NODE);
    expect_value(mock_create_children_node_var, type, AST_TYPE_BINDING);
    expect_value(mock_create_children_node_var, children_nb, 2);
    expect_value(mock_create_children_node_var, child, DUMMY_AST_ERROR_OR_SYMBOL_NAME_NODE);
    expect_value(mock_create_children_node_var, child, DUMMY_AST_ERROR_OR_EVALUABLE_NODE);
    will_return(mock_create_children_node_var, DUMMY_BINDING_NODE);

    binding_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_BINDING_NODE);
}


//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest binding_parse_tests[] = {
        cmocka_unit_test_setup_teardown(
            binding_parse_cleans_up_and_create_error_node_for_LHS_semantic_value_when_create_children_node_var_fails,
            binding_parse_setup, binding_parse_teardown),
        cmocka_unit_test_setup_teardown(
            binding_parse_create_binding_node_for_LHS_semantic_value_when_create_children_node_var_succeeds,
            binding_parse_setup, binding_parse_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(binding_parse_tests, NULL, NULL);

    return failed;
}
