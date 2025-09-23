// src/parser/tests/test_statement_parser.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>

#include "statement_parser.tab.h"
#include "parser_ctx.h"
#include "mock_lexer.h"


//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static ast *parsed_ast;

static const char DUMMY[4];
static ast *const DUMMY_AST_P = (ast *) &DUMMY[0];
static ast *const DUMMY_AST_BINDING_P = (ast *) &DUMMY[1];
static ast *const DUMMY_AST_WRITING_P = (ast *) &DUMMY[2];
static ast *const DUMMY_AST_READING_P = (ast *) &DUMMY[3];



//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


mock_token seq_binding_statement[] = {
    {SYMBOL_NAME, {.symbol_name_value = "symbol_name"}},
    {EQUAL, {0}},
    {STRING, {.string_value = "chaine"}},
    {SEMICOLON, {0}},
    {0, {0}}
};

mock_token seq_writing_statement[] = {
    {WRITE, { 0 }},
    {SYMBOL_NAME, {.symbol_name_value = "symbol_name"}},
    {SEMICOLON, {0}},
    {0, {0}}
};

mock_token seq_reading_statement[] = {
    {READ, { 0 }},
    {SYMBOL_NAME, {.symbol_name_value = "symbol_name"}},
    {SEMICOLON, {0}},
    {0, {0}}
};

ast *stub_binding_action(void) {
    return mock_type(ast *);
}

ast *stub_writing_action(void) {
    return mock_type(ast *);
}

ast *stub_reading_action(void) {
    return mock_type(ast *);
}

ast *stub_function_definition_action(void) {
    return mock_type(ast *);
}

ast *stub_function_call_action(void) {
    return mock_type(ast *);
}



//-----------------------------------------------------------------------------
// statement_parse TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// Action under test via injected stubs:
// statement: binding | writing | reading

// mocked:
//  - actions of grammar rules that the rule under test depends on:
//    - binding: symbol_name_atom EQUAL atom SEMICOLON
//    - writing: WRITE symbol_name_atom SEMICOLON
//    - reading: READ symbol_name_atom SEMICOLON
//  - function of the lexer module which are used:
//    - yylex



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int statement_parse_setup(void **state) {
    (void)state;
    parsed_ast = NULL;
    mock_lex_reset();
    return 0;
}

static int statement_parse_teardown(void **state) {
    (void)state;
    mock_lex_reset();
    parsed_ast = NULL;
    return 0;
}



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------

// Given:
//  - lexer returns:
//    - SYMBOL_NAME("symbol_name")
//    - EQUAL
//    - STRING("chaine")
//    - SEMICOLON
// Expected:
//  - the action of "binding: symbol_name_atom EQUAL atom SEMICOLON" is executed
//  - the semantic value of binding is propagated to statement's semantic value
static void statement_parse_executes_binding_action_and_propagates_value_when_lexer_returns_binding_sequence(void **state) {
    mock_lex_set(seq_binding_statement, 5);
    will_return(stub_binding_action, DUMMY_AST_BINDING_P);

    statement_parse(NULL, &parsed_ast, NULL);

    assert_ptr_equal(parsed_ast, DUMMY_AST_BINDING_P);
}

// Given:
//  - lexer returns:
//    - WRITE
//    - SYMBOL_NAME("symbol_name")
//    - SEMICOLON
// Expected:
//  - the action of "writing: symbol_name_atom SEMICOLON" is executed
//  - the semantic value of writing is propagated to statement's semantic value
static void statement_parse_executes_writing_action_and_propagates_value_when_lexer_returns_writing_sequence(void **state) {
    mock_lex_set(seq_writing_statement, 4);
    will_return(stub_writing_action, DUMMY_AST_WRITING_P);

    statement_parse(NULL, &parsed_ast, NULL);

    assert_ptr_equal(parsed_ast, DUMMY_AST_WRITING_P);
}

// Given:
//  - lexer returns:
//    - READ
//    - SYMBOL_NAME("symbol_name")
// Expected:
//  - the action of "reading : SYMBOL_NAME" is executed
//  - the semantic value of reading is propagated to statement's semantic value
static void statement_parse_executes_reading_action_and_propagates_value_when_lexer_returns_reading_sequence(void **state) {
    mock_lex_set(seq_reading_statement, 4);
    will_return(stub_reading_action, DUMMY_AST_READING_P);

    statement_parse(NULL, &parsed_ast, NULL);

    assert_ptr_equal(parsed_ast, DUMMY_AST_READING_P);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
    const struct CMUnitTest statement_parse_tests[] = {
        cmocka_unit_test_setup_teardown(
            statement_parse_executes_binding_action_and_propagates_value_when_lexer_returns_binding_sequence,
            statement_parse_setup, statement_parse_teardown),
        cmocka_unit_test_setup_teardown(
            statement_parse_executes_writing_action_and_propagates_value_when_lexer_returns_writing_sequence,
            statement_parse_setup, statement_parse_teardown),
        cmocka_unit_test_setup_teardown(
            statement_parse_executes_reading_action_and_propagates_value_when_lexer_returns_reading_sequence,
            statement_parse_setup, statement_parse_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(statement_parse_tests, NULL, NULL);

    return failed;
}
