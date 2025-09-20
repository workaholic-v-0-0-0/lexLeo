// src/parser/tests/test_block_parser.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>

#include "block_parser.tab.h"
#include "parser_ctx.h"
#include "mock_lexer.h"
#include "list.h"



//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static ast *parsed_ast;

static const char DUMMY[3];
static ast *const DUMMY_AST_ERROR_OR_BLOCK_ITEMS_NODE = (ast *) &DUMMY[0];
static ast *const DUMMY_AST_ERROR = (ast *) &DUMMY[1];
static ast *const DUMMY_AST_BLOCK_NODE = (ast *) &DUMMY[2];



//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


mock_token seq_block[] = {
    {LBRACE, {0}},
    {SYMBOL_NAME, {.symbol_name_value = "symbol_name"}},
    {EQUAL, {0}},
    {STRING, {.string_value = "chaine"}},
    {SEMICOLON, {0}},
    {READ, { 0 }},
    {SYMBOL_NAME, {.symbol_name_value = "symbol_name"}},
    {SEMICOLON, {0}},
    {WRITE, { 0 }},
    {SYMBOL_NAME, {.symbol_name_value = "symbol_name"}},
    {SEMICOLON, {0}},
    {RBRACE, {0}},
    {0, {0}}
};

ast *stub_block_items_action(void) {
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
// block_parse TESTS
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// Action under test via injected stubs:
// block: LBRACE block_items RBRACE

// mocked:
//  - actions of grammar rules that the rule under test depends on:
//    - block_items: | block_items statement
//    - statement: binding | writing | reading
//    - binding: symbol_name_atom EQUAL atom SEMICOLON
//    - writing: WRITE symbol_name_atom SEMICOLON
//    - reading: READ symbol_name_atom SEMICOLON
//    - atom: number_atom | string_atom | symbol_name_atom
//    - number_atom: INTEGER
//    - string_atom: STRING
//    - symbol_name_atom: SYMBOL_NAME
//  - functions of the ast module which are used:
//    - ast_create_children_node_var
//    - ast_create_error_node_or_sentinel
//    - ast_destroy
//  - function of the lexer module which are used:
//    - yylex



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int block_parse_setup(void **state) {
    (void)state;
    parsed_ast = NULL;
    mock_lex_reset();
    mock_ctx.ops.create_children_node_var = mock_create_children_node_var;
    mock_ctx.ops.create_error_node_or_sentinel = mock_create_error_node_or_sentinel;
    mock_ctx.ops.destroy = mock_destroy;
    return 0;
}

static int block_parse_teardown(void **state) {
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
//    - LBRACE
//    - SYMBOL_NAME("symbol_name")
//    - EQUAL
//    - STRING("chaine")
//    - SEMICOLON
//    - READ
//    - SYMBOL_NAME("symbol_name"),
//    - SEMICOLON
//    - WRITE
//    - SYMBOL_NAME("symbol_name")
//    - SEMICOLON
//    - RBRACE
// Expected:
//  - action of the rule "block_items: block_items statement" is executed,
//    this yields a block_items (semantic value provided here by a stub)
//  - calls ast_create_children_node_var with:
//    - type: AST_TYPE_BLOCK
//    - children_nb: 1
//    - first optionnal arg: $2 (block_items's semantic value)
//    - no more arg


// Given:
//  - ast_create_children_node_var will fail
// Expected:
//  - calls ast_destroy with:
//    - root: $2 (block_items's semantic value)
//  - calls create_error_node_or_sentinel with:
//    - code: AST_ERROR_CODE_BLOCK_NODE_CREATION_FAILED
//    - message: "ast creation for a block node failed"
//  - gives create_error_node_or_sentinel returned value for the LHS semantic value
static void block_parse_cleans_up_and_create_error_node_for_LHS_when_create_children_node_var_fails(void **state) {
    mock_lex_set(seq_block, 13);
    will_return(stub_block_items_action, DUMMY_AST_ERROR_OR_BLOCK_ITEMS_NODE);
    expect_value(mock_create_children_node_var, type, AST_TYPE_BLOCK);
    expect_value(mock_create_children_node_var, children_nb, 1);
    expect_value(mock_create_children_node_var, child, DUMMY_AST_ERROR_OR_BLOCK_ITEMS_NODE);
    will_return(mock_create_children_node_var, NULL);
    expect_value(mock_destroy, root, DUMMY_AST_ERROR_OR_BLOCK_ITEMS_NODE);
    expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_BLOCK_NODE_CREATION_FAILED);
    expect_string(mock_create_error_node_or_sentinel, message, "ast creation for a block node failed");
    will_return(mock_create_error_node_or_sentinel, DUMMY_AST_ERROR);

    block_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR);
}

// Given:
//  - ast_create_children_node_var will succeed
// Expected:
//  - gives create_children_node_var returned value for the LHS semantic value
static void block_parse_creates_a_block_node_when_create_children_node_var_succeeds(void **state) {
    mock_lex_set(seq_block, 13);
    will_return(stub_block_items_action, DUMMY_AST_ERROR_OR_BLOCK_ITEMS_NODE);
    expect_value(mock_create_children_node_var, type, AST_TYPE_BLOCK);
    expect_value(mock_create_children_node_var, children_nb, 1);
    expect_value(mock_create_children_node_var, child, DUMMY_AST_ERROR_OR_BLOCK_ITEMS_NODE);
    will_return(mock_create_children_node_var, DUMMY_AST_BLOCK_NODE);

    block_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_BLOCK_NODE);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
    const struct CMUnitTest block_parse_tests[] = {
        cmocka_unit_test_setup_teardown(
            block_parse_cleans_up_and_create_error_node_for_LHS_when_create_children_node_var_fails,
            block_parse_setup, block_parse_teardown),
        cmocka_unit_test_setup_teardown(
            block_parse_creates_a_block_node_when_create_children_node_var_succeeds,
            block_parse_setup, block_parse_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(block_parse_tests, NULL, NULL);

    return failed;
}
