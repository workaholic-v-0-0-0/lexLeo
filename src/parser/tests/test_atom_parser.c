// src/parser/tests/test_atom_parser.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>

#include "atom_parser.tab.h"
#include "mock_lexer.h"


//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static ast *parsed_ast;

static const char DUMMY[1];
static ast *const DUMMY_AST_P = (ast *) &DUMMY[0];


//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


static mock_token seq_integer[] = {
    {INTEGER, {.int_value = 5}},
    {0, {0}}
};

mock_token seq_string[] = {
    {STRING, {.string_value = "chaine"}},
    {0, {0}}
};

static mock_token seq_symbol_name[] = {
    {SYMBOL_NAME, {.string_value = "symbol_name"}},
    {0, {0}}
};

ast *stub_number_atom_action(int i) {
    check_expected(i);
    return mock_type(ast *);
}

ast *stub_string_atom_action(char *str) {
    check_expected(str);
    return mock_type(ast *);
}

ast *stub_symbol_name_atom_action(char *symbol_name) {
    check_expected(symbol_name);
    return mock_type(ast *);
}


//-----------------------------------------------------------------------------
// atom_parse TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// Action under tests via injected stubs:
// atom : number_atom | string_atom | symbol_name_atom

// mocked:
//  - actions of grammar rules that the rule under tests depends on:
//    - number_atom: INTEGER
//    - string_atom: STRING
//    - symbol_name_atom: SYMBOL_NAME
//  - function of the lexer module which are used:
//    - yylex


//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int atom_parse_setup(void **state) {
    (void) state;
    parsed_ast = NULL;
    mock_lex_reset();
    return 0;
}

static int atom_parse_teardown(void **state) {
    (void) state;
    mock_lex_reset();
    parsed_ast = NULL;
    return 0;
}


//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - lexer returns INTEGER(5)
// Expected:
//  - the action of "number_atom : INTEGER" is executed
//  - the semantic value of number_atom is propagated to atom's semantic value
static void atom_parse_executes_number_atom_action_and_propagates_value_when_lexer_returns_INTEGER(void **state) {
    mock_lex_set(seq_integer, 2);
    expect_value(stub_number_atom_action, i, 5);
    will_return(stub_number_atom_action, DUMMY_AST_P);

    atom_parse(NULL, &parsed_ast, NULL);

    assert_ptr_equal(parsed_ast, DUMMY_AST_P);
}

// Given:
//  - lexer returns STRING("chaine")
// Expected:
//  - the action of "string_atom : STRING" is executed
//  - the semantic value of string_atom is propagated to atom's semantic value
static void atom_parse_executes_string_atom_action_and_propagates_value_when_lexer_returns_STRING(void **state) {
    mock_lex_set(seq_string, 2);
    expect_string(stub_string_atom_action, str, "chaine");
    will_return(stub_string_atom_action, DUMMY_AST_P);

    atom_parse(NULL, &parsed_ast, NULL);

    assert_ptr_equal(parsed_ast, DUMMY_AST_P);
}

// Given:
//  - lexer returns SYMBOL_NAME("symbol_name")
// Expected:
//  - the action of "symbol_name_atom : SYMBOL_NAME" is executed
//  - the semantic value of symbol_name_atom is propagated to atom's semantic value
static void atom_parse_executes_symbol_name_atom_action_and_propagates_value_when_lexer_returns_SYMBOL_NAME(
    void **state) {
    mock_lex_set(seq_symbol_name, 2);
    expect_string(stub_symbol_name_atom_action, symbol_name, "symbol_name");
    will_return(stub_symbol_name_atom_action, DUMMY_AST_P);

    atom_parse(NULL, &parsed_ast, NULL);

    assert_ptr_equal(parsed_ast, DUMMY_AST_P);
}


//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
    const struct CMUnitTest atom_parse_tests[] = {
        cmocka_unit_test_setup_teardown(
            atom_parse_executes_number_atom_action_and_propagates_value_when_lexer_returns_INTEGER,
            atom_parse_setup, atom_parse_teardown),
        cmocka_unit_test_setup_teardown(
            atom_parse_executes_string_atom_action_and_propagates_value_when_lexer_returns_STRING,
            atom_parse_setup, atom_parse_teardown),
        cmocka_unit_test_setup_teardown(
            atom_parse_executes_symbol_name_atom_action_and_propagates_value_when_lexer_returns_SYMBOL_NAME,
            atom_parse_setup, atom_parse_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(atom_parse_tests, NULL, NULL);

    return failed;
}