// src/parser/tests/test_evaluable_parser.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>

#include "evaluable_parser.tab.h"
#include "parser_ctx.h"
#include "mock_lexer.h"
#include "list.h"


//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static ast *parsed_ast;

static const char DUMMY[5];
static ast *const DUMMY_AST_ERROR_OR_FUNCTION_CALL_NODE = (ast *) &DUMMY[0];
static ast *const DUMMY_AST_ERROR_OR_ATOM_NODE = (ast *) &DUMMY[1];
static ast *const DUMMY_AST_ERROR_OR_COMPUTABLE_NODE = (ast *) &DUMMY[2];
static ast *const DUMMY_AST_ERROR_OR_QUOTE_NODE = (ast *) &DUMMY[3];
static ast *const DUMMY_AST_ERROR = (ast *) &DUMMY[4];


//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


static mock_token seq_function_call[] = {
    {SYMBOL_NAME, {.symbol_name_value = "function_name"}},
    {LPAREN, {0}},
    {INTEGER, {.int_value = 7}},
    {INTEGER, {.int_value = 66}},
    {RPAREN, {0}},
    {0, {0}}
};

static mock_token seq_atom[] = {
    {SYMBOL_NAME, {.string_value = "symbol_name"}},
    {0, {0}}
};

static mock_token seq_computable[] = {
    {INTEGER, {.int_value = 3}},
    {MULTIPLY, {0}},
    {LPAREN, {0}},
    {INTEGER, {.int_value = 7}},
    {ADD, {0}},
    {INTEGER, {.int_value = 66}},
    {RPAREN, {0}},
    {0, {0}}
};

static mock_token seq_quote[] = {
    {QUOTE, {0}},
    {INTEGER, {.int_value = 3}},
    {MULTIPLY, {0}},
    {LPAREN, {0}},
    {INTEGER, {.int_value = 7}},
    {ADD, {0}},
    {INTEGER, {.int_value = 66}},
    {RPAREN, {0}},
    {0, {0}}
};

ast *stub_function_call_action(void) {
    return mock_type(ast *);
}

ast *stub_atom_action(void) {
    return mock_type(ast *);
}

ast *stub_computable_action(void) {
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
// evaluable_parse TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// Action under tests via injected stubs:
// evaluable: function_call | atom | computable | QUOTE evaluable

// mocked:
//  - actions of grammar rules that the rule under tests depends on:
//    - function_call: symbol_name_atom list_of_numbers
//    - symbol_name_atom: SYMBOL_NAME
//    - list_of_numbers: LPAREN numbers RPAREN
//    - numbers: | numbers number_atom
//    - number_atom: INTEGER
//    - atom : number_atom | string_atom | symbol_name_atom
//    - string_atom: STRING
//    - computable
//        : SUBTRACT computable
//        | computable ADD computable
//        | computable SUBTRACT computable
//        | computable MULTIPLY computable
//        | computable DIVIDE computable
//        | LPAREN computable RPAREN
//        | number_atom
//  - functions of the ast module which are used:
//    - ast_create_children_node_var
//    - ast_create_error_node_or_sentinel
//    - ast_destroy
//  - function of the lexer module which are used:
//    - yylex


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int evaluable_parse_setup(void **state) {
    (void) state;
    parsed_ast = NULL;
    mock_lex_reset();
    mock_ctx.ops.create_children_node_var = mock_create_children_node_var;
    mock_ctx.ops.create_error_node_or_sentinel = mock_create_error_node_or_sentinel;
    mock_ctx.ops.destroy = mock_destroy;
    return 0;
}

static int evaluable_parse_teardown(void **state) {
    (void) state;
    mock_lex_reset();
    parsed_ast = NULL;
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - lexer returns:
//    - SYMBOL_NAME("function_name")
//    - LPAREN
//    - INTEGER(7)
//    - INTEGER(66)
//    - RPAREN
// Expected:
//  - the action of "function_call: symbol_name_atom list_of_numbers" is executed
//  - the semantic value of function_call is propagated to evaluable's semantic value
static void evaluable_parse_executes_function_call_action_and_propagates_value_when_lexer_returns_function_call(
    void **state) {
    mock_lex_set(seq_function_call, 6);
    will_return(stub_function_call_action, DUMMY_AST_ERROR_OR_FUNCTION_CALL_NODE);

    evaluable_parse(NULL, &parsed_ast, NULL);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_OR_FUNCTION_CALL_NODE);
}

// Given:
//  - lexer returns:
//    - SYMBOL_NAME("symbol_name")
// Expected:
//  - the action of "atom : number_atom | string_atom | symbol_name_atom" is executed
//  - the semantic value of atom is propagated to evaluable's semantic value
static void evaluable_parse_executes_atom_action_and_propagates_value_when_lexer_returns_atom(void **state) {
    mock_lex_set(seq_atom, 2);
    will_return(stub_atom_action, DUMMY_AST_ERROR_OR_ATOM_NODE);

    evaluable_parse(NULL, &parsed_ast, NULL);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_OR_ATOM_NODE);
}

// Given:
//  - lexer returns:
//    - INTEGER(3)
//    - MULTIPLY
//    - LPAREN
//    - INTEGER(7)
//    - ADD
//    - INTEGER(66)
//    - RPAREN
// Expected:
//  - the action of
//        "computable
//        : SUBTRACT computable
//        | computable ADD computable
//        | computable SUBTRACT computable
//        | computable MULTIPLY computable
//        | computable DIVIDE computable
//        | LPAREN computable RPAREN
//        | number_atom"
//    is executed
//  - the semantic value of computable is propagated to evaluable's semantic value
static void
evaluable_parse_executes_computable_action_and_propagates_value_when_lexer_returns_computable(void **state) {
    mock_lex_set(seq_computable, 8);
    will_return(stub_computable_action, DUMMY_AST_ERROR_OR_COMPUTABLE_NODE);

    evaluable_parse(NULL, &parsed_ast, NULL);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_OR_COMPUTABLE_NODE);
}

// Given:
//  - lexer returns:
//    - QUOTE
//    - INTEGER(3)
//    - MULTIPLY
//    - LPAREN
//    - INTEGER(7)
//    - ADD
//    - INTEGER(66)
//    - RPAREN
//  - ast_create_children_node_var will fail
// Expected:
//  - the action of
//        "computable
//        : SUBTRACT computable
//        | computable ADD computable
//        | computable SUBTRACT computable
//        | computable MULTIPLY computable
//        | computable DIVIDE computable
//        | LPAREN computable RPAREN
//        | number_atom"
//    is executed
//  - calls ast_create_children_node_var with:
//    - type: AST_TYPE_QUOTE
//    - children_nb: 1
//    - first optionnal arg: evaluable's semantic value
//    - no more arg
//  - calls ast_destroy with:
//    - root: $2 (evaluable's semantic value)
//  - calls create_error_node_or_sentinel with:
//    - code: AST_ERROR_CODE_QUOTE_NODE_CREATION_FAILED
//    - message: "ast creation for a quote node failed"
//  - gives create_error_node_or_sentinel returned value for the LHS semantic value
static void evaluable_parse_cleans_up_and_create_error_node_for_LHS_semantic_value_when_create_children_node_var_fails(
    void **state) {
    mock_lex_set(seq_quote, 9);
    will_return(stub_computable_action, DUMMY_AST_ERROR_OR_COMPUTABLE_NODE);
    expect_value(mock_create_children_node_var, type, AST_TYPE_QUOTE);
    expect_value(mock_create_children_node_var, children_nb, 1);
    expect_value(mock_create_children_node_var, child, DUMMY_AST_ERROR_OR_COMPUTABLE_NODE);
    will_return(mock_create_children_node_var, NULL);
    expect_value(mock_destroy, root, DUMMY_AST_ERROR_OR_COMPUTABLE_NODE);
    expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_QUOTE_NODE_CREATION_FAILED);
    expect_string(mock_create_error_node_or_sentinel, message, "ast creation for a quote node failed");
    will_return(mock_create_error_node_or_sentinel, DUMMY_AST_ERROR);

    evaluable_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR);
}

// Given:
//  - lexer returns:
//    - QUOTE
//    - INTEGER(3)
//    - MULTIPLY
//    - LPAREN
//    - INTEGER(7)
//    - ADD
//    - INTEGER(66)
//    - RPAREN
//  - ast_create_children_node_var will succeed
// Expected:
//  - the action of
//        "computable
//        : SUBTRACT computable
//        | computable ADD computable
//        | computable SUBTRACT computable
//        | computable MULTIPLY computable
//        | computable DIVIDE computable
//        | LPAREN computable RPAREN
//        | number_atom"
//    is executed
//  - calls ast_create_children_node_var with:
//    - type: AST_TYPE_QUOTE
//    - children_nb: 1
//    - first optionnal arg: evaluable's semantic value
//    - no more arg
//  - gives create_children_node_var returned value for the LHS semantic value
static void evaluable_parse_creates_a_quote_node_when_create_children_node_var_succeeds(void **state) {
    mock_lex_set(seq_quote, 9);
    will_return(stub_computable_action, DUMMY_AST_ERROR_OR_COMPUTABLE_NODE);
    expect_value(mock_create_children_node_var, type, AST_TYPE_QUOTE);
    expect_value(mock_create_children_node_var, children_nb, 1);
    expect_value(mock_create_children_node_var, child, DUMMY_AST_ERROR_OR_COMPUTABLE_NODE);
    will_return(mock_create_children_node_var, DUMMY_AST_ERROR_OR_QUOTE_NODE);

    evaluable_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_OR_QUOTE_NODE);
}


//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
    const struct CMUnitTest evaluable_parse_tests[] = {
        cmocka_unit_test_setup_teardown(
            evaluable_parse_executes_function_call_action_and_propagates_value_when_lexer_returns_function_call,
            evaluable_parse_setup, evaluable_parse_teardown),
        cmocka_unit_test_setup_teardown(
            evaluable_parse_executes_atom_action_and_propagates_value_when_lexer_returns_atom,
            evaluable_parse_setup, evaluable_parse_teardown),
        cmocka_unit_test_setup_teardown(
            evaluable_parse_executes_computable_action_and_propagates_value_when_lexer_returns_computable,
            evaluable_parse_setup, evaluable_parse_teardown),
        cmocka_unit_test_setup_teardown(
            evaluable_parse_cleans_up_and_create_error_node_for_LHS_semantic_value_when_create_children_node_var_fails,
            evaluable_parse_setup, evaluable_parse_teardown),
        cmocka_unit_test_setup_teardown(
            evaluable_parse_creates_a_quote_node_when_create_children_node_var_succeeds,
            evaluable_parse_setup, evaluable_parse_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(evaluable_parse_tests, NULL, NULL);

    return failed;
}