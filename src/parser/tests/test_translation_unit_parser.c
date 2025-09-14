// src/parser/tests/test_translation_unit_parser.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>

#include "translation_unit_parser.tab.h"
#include "parser_ctx.h"
#include "mock_lexer.h"


//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static ast *parsed_ast;

static const char DUMMY[7];
static ast *const DUMMY_AST_ERROR_P = (ast *) &DUMMY[0];
static ast *const DUMMY_CHILDREN_NODE_P = (ast *) &DUMMY[1];
static ast *const DUMMY_AST_ERROR_OR_BINDING_NODE_P = (ast *) &DUMMY[2];
static error_info *const DUMMY_ERROR_INFO_P = (error_info *) &DUMMY[3];
static ast FAKE_AST_ERROR = {AST_TYPE_ERROR, .error = DUMMY_ERROR_INFO_P};
static ast *const DUMMY_AST_TRANSLATION_UNIT_NODE_P = (ast *) &DUMMY[4];
static ast *const DUMMY_AST_ERROR_OR_READING_NODE_P = (ast *) &DUMMY[2];
static ast *const DUMMY_AST_ERROR_OR_WRITING_NODE_P = (ast *) &DUMMY[2];



//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


mock_token seq_0_statement[] = {
    {0, {0}}
};

mock_token seq_1_statement[] = {
    {SYMBOL_NAME, {.symbol_name_value = "symbol_name"}},
    {EQUAL, {0}},
    {STRING, {.string_value = "chaine"}},
    {SEMICOLON, {0}},
    {0, {0}}
};

mock_token seq_3_statements[] = {
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
    {0, {0}}
};

static ast *fake_ast_error_p = NULL;

ast *stub_binding_statement_action(void) {
    return mock_type(ast *);
}

ast *stub_reading_statement_action(void) {
    return mock_type(ast *);
}

ast *stub_writing_statement_action(void) {
    return mock_type(ast *);
}

bool mock_children_append_take(ast *parent, ast *child) {
    check_expected(parent);
    check_expected(child);
    return mock_type(bool);
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
// translation_unit_parse TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// Action under test via injected stubs:
// translation_unit: /* empty */ | translation_unit statement

// mocked:
//  - actions of grammar rules that the rule under test depends on:
//    - statement: binding | writing | reading
//    - binding: symbol_name_atom EQUAL atom SEMICOLON
//    - writing: WRITE symbol_name_atom SEMICOLON
//    - reading: READ symbol_name_atom SEMICOLON
//    - atom: number_atom | string_atom | symbol_name_atom
//    - number_atom: INTEGER
//    - string_atom: STRING
//    - symbol_name_atom: SYMBOL_NAME
//  - functions of the ast module which are used:
//    - ast_children_append_take
//    - ast_create_children_node_var
//    - ast_create_error_node_or_sentinel
//    - ast_destroy
//  - function of the lexer module which are used:
//    - yylex



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int translation_unit_parse_setup(void **state) {
    (void)state;
    parsed_ast = NULL;
    mock_lex_reset();
    mock_ctx.ops.children_append_take = mock_children_append_take;
    mock_ctx.ops.create_children_node_var = mock_create_children_node_var;
    mock_ctx.ops.create_error_node_or_sentinel = mock_create_error_node_or_sentinel;
    mock_ctx.ops.destroy = mock_destroy;
    return 0;
}

static int translation_unit_parse_teardown(void **state) {
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
//    - EOF
//  - ast_create_children_node_var will fail
// Expected:
//  - calls ast_create_children_node_var with:
//    - type: AST_TYPE_TRANSLATION_UNIT
//    - children_nb: 0
//    - no more arg
//  - calls create_error_node_or_sentinel with:
//    - code: AST_ERROR_CODE_TRANSLATION_UNIT_NODE_CREATION_FAILED
//    - message: "ast creation for a translation unit node failed"
//  - gives create_error_node_or_sentinel returned value for the translation_unit's semantic value
static void translation_unit_parse_create_error_node_for_LHS_when_0_statement_and_create_children_node_var_fails(void **state) {
    mock_lex_set(seq_0_statement, 1);
    expect_value(mock_create_children_node_var, type, AST_TYPE_TRANSLATION_UNIT);
    expect_value(mock_create_children_node_var, children_nb, 0);
    will_return(mock_create_children_node_var, NULL);
    expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_TRANSLATION_UNIT_NODE_CREATION_FAILED);
    expect_string(mock_create_error_node_or_sentinel, message, "ast creation for a translation unit node failed");
    will_return(mock_create_error_node_or_sentinel, DUMMY_AST_ERROR_P);

    translation_unit_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_P);
}

// Given:
//  - lexer returns:
//    - EOF
//  - ast_create_children_node_var will succeed
// Expected:
//  - calls ast_create_children_node_var with:
//    - type: AST_TYPE_TRANSLATION_UNIT
//    - children_nb: 0
//    - no more arg
//  - gives ast_create_children_node_var returned value for the translation_unit's semantic value
static void translation_unit_parse_create_translation_unit_node_for_LHS_when_0_statement_and_create_children_node_var_succeeds(void **state) {
    mock_lex_set(seq_0_statement, 1);
    expect_value(mock_create_children_node_var, type, AST_TYPE_TRANSLATION_UNIT);
    expect_value(mock_create_children_node_var, children_nb, 0);
    will_return(mock_create_children_node_var, DUMMY_CHILDREN_NODE_P);

    translation_unit_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_CHILDREN_NODE_P);
}

// Given:
//  - lexer returns:
//    - SYMBOL_NAME("symbol_name")
//    - EQUAL
//    - STRING("chaine")
//    - SEMICOLON
//  - ast_create_children_node_var will fail
// Expected:
//  - epsilon -> translation_unit triggers a call to ast_create_children_node_var with:
//    - type: AST_TYPE_TRANSLATION_UNIT
//    - children_nb: 0
//    (no additional argument)
//  - calls create_error_node_or_sentinel with:
//    - code: AST_ERROR_CODE_TRANSLATION_UNIT_NODE_CREATION_FAILED
//    - message: "ast creation for a translation unit node failed"
//  - the error node returned by create_error_node_or_sentinel becomes the semantic value of
//    translation_unit produced by the epsilon-reduction
//  - action of the rule "binding: symbol_name_atom EQUAL atom SEMICOLON" is executed,
//    this yields a statement (semantic value provided here by a stub)
//  - when reducing translation_unit : translation_unit statement
//    - NO CALL TO ast_children_append_take must occur, since $1 is already an error node
//    - ast_destroy is called with $2 (the statement returned by the stub)
//    - the semantic value of the LHS translation_unit is the same error node as $1
static void translation_unit_parse_cleans_up_and_create_error_node_for_LHS_when_1_statement_and_create_children_node_var_fails(void **state) {
    mock_lex_set(seq_1_statement, 5);
    expect_value(mock_create_children_node_var, type, AST_TYPE_TRANSLATION_UNIT);
    expect_value(mock_create_children_node_var, children_nb, 0);
    will_return(mock_create_children_node_var, NULL);
    expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_TRANSLATION_UNIT_NODE_CREATION_FAILED);
    expect_string(mock_create_error_node_or_sentinel, message, "ast creation for a translation unit node failed");
    will_return(mock_create_error_node_or_sentinel, &FAKE_AST_ERROR);
    will_return(stub_binding_statement_action, DUMMY_AST_ERROR_OR_BINDING_NODE_P);
    expect_value(mock_destroy, root, DUMMY_AST_ERROR_OR_BINDING_NODE_P);

    translation_unit_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, &FAKE_AST_ERROR);
}

// Given:
//  - lexer returns:
//    - SYMBOL_NAME("symbol_name")
//    - EQUAL
//    - STRING("chaine")
//    - SEMICOLON
//  - ast_create_children_node_var will succeed
//  - ast_children_append_take will fail
// Expected:
//  - epsilon -> translation_unit triggers a call to ast_create_children_node_var with:
//    - type: AST_TYPE_TRANSLATION_UNIT
//    - children_nb: 0
//    (no additional argument)
//  - the returned value of ast_create_children_node_var becomes the semantic value of
//    translation_unit produced by the epsilon-reduction
//  - action of the rule "binding: symbol_name_atom EQUAL atom SEMICOLON" is executed,
//    this yields a statement (semantic value provided here by a stub)
//  - when reducing translation_unit : translation_unit statement
//    - calls ast_children_append_take with:
//      - parent: $1 (returned value of ast_create_children_node_var)
//      - child: $2 (provided by stub)
//    - calls ast_destroy with:
//      - root: $1
//    - calls ast_destroy with:
//      - root: $2
//    - calls create_error_node_or_sentinel with:
//      - code: AST_ERROR_CODE_TRANSLATION_UNIT_APPEND_FAILED
//      - message: "ast append failed when adding a statement to the translation unit"
//    - the returned value of create_error_node_or_sentinel becomes the semantic value of the LHS translation_unit
static void translation_unit_parse_cleans_up_and_create_error_node_for_LHS_when_1_statement_and_children_append_take_fails(void **state) {
    mock_lex_set(seq_1_statement, 5);
    expect_value(mock_create_children_node_var, type, AST_TYPE_TRANSLATION_UNIT);
    expect_value(mock_create_children_node_var, children_nb, 0);
    will_return(mock_create_children_node_var, DUMMY_AST_TRANSLATION_UNIT_NODE_P);
    will_return(stub_binding_statement_action, DUMMY_AST_ERROR_OR_BINDING_NODE_P);
    expect_value(mock_children_append_take, parent, DUMMY_AST_TRANSLATION_UNIT_NODE_P);
    expect_value(mock_children_append_take, child, DUMMY_AST_ERROR_OR_BINDING_NODE_P);
    will_return(mock_children_append_take, false);
    expect_value(mock_destroy, root, DUMMY_AST_TRANSLATION_UNIT_NODE_P);
    expect_value(mock_destroy, root, DUMMY_AST_ERROR_OR_BINDING_NODE_P);
    expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_TRANSLATION_UNIT_APPEND_FAILED);
    expect_string(mock_create_error_node_or_sentinel, message, "ast append failed when adding a statement to the translation unit");
    will_return(mock_create_error_node_or_sentinel, DUMMY_AST_ERROR_P);

    translation_unit_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_P);
}

// Given:
//  - lexer returns:
//    - SYMBOL_NAME("symbol_name")
//    - EQUAL
//    - STRING("chaine")
//    - SEMICOLON
//  - ast_create_children_node_var will succeed
//  - ast_children_append_take will succeed
// Expected:
//  - epsilon -> translation_unit triggers a call to ast_create_children_node_var with:
//    - type: AST_TYPE_TRANSLATION_UNIT
//    - children_nb: 0
//    (no additional argument)
//  - the returned value of ast_create_children_node_var becomes the semantic value of
//    translation_unit produced by the epsilon-reduction
//  - action of the rule "binding: symbol_name_atom EQUAL atom SEMICOLON" is executed,
//    this yields a statement (semantic value provided here by a stub)
//  - when reducing translation_unit : translation_unit statement
//    - calls ast_children_append_take with:
//      - parent: $1 (returned value of ast_create_children_node_var)
//      - child: $2 (provided by stub)
//    - the returned value of ast_children_append_take becomes the semantic value of the LHS translation_unit
static void translation_unit_parse_create_translation_unit_node_for_LHS_when_1_statement_and_children_append_take_succeeds(void **state) {
    mock_lex_set(seq_1_statement, 5);
    expect_value(mock_create_children_node_var, type, AST_TYPE_TRANSLATION_UNIT);
    expect_value(mock_create_children_node_var, children_nb, 0);
    will_return(mock_create_children_node_var, DUMMY_AST_TRANSLATION_UNIT_NODE_P);
    will_return(stub_binding_statement_action, DUMMY_AST_ERROR_OR_BINDING_NODE_P);
    expect_value(mock_children_append_take, parent, DUMMY_AST_TRANSLATION_UNIT_NODE_P);
    expect_value(mock_children_append_take, child, DUMMY_AST_ERROR_OR_BINDING_NODE_P);
    will_return(mock_children_append_take, true);

    translation_unit_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_TRANSLATION_UNIT_NODE_P);
}

// Given:
//  - lexer returns:
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
//  - ast_create_children_node_var will succeed
//  - every ast_children_append_take will succeed
// Expected:
//  - epsilon -> translation_unit
//    - $$ = ast_create_children_node_var(AST_TYPE_TRANSLATION_UNIT, 0)
//  - "symbol_name EQUAL atom SEMICOLON" -> statement (stubbed)
//    - $$ = stub_binding_statement_action()
//  - translation_unit statement -> translation_unit
//    - $$ = ctx->ops.children_append_take($1, $2)
//  - "READ symbol_name SEMICOLON" -> statement (stubbed)
//    - $$ = stub_reading_statement_action()
//  - translation_unit statement -> translation_unit
//    - $$ = ctx->ops.children_append_take($1, $2)
//  - "WRITE symbol_name SEMICOLON" -> statement (stubbed)
//    - $$ = stub_writing_statement_action()
//  - translation_unit statement -> translation_unit
//    - $$ = ctx->ops.children_append_take($1, $2)
static void translation_unit_parse_create_translation_unit_node_for_LHS_when_3_statement_and_children_append_take_succeeds_3_times(void **state) {
    mock_lex_set(seq_3_statements, 11);
    expect_value(mock_create_children_node_var, type, AST_TYPE_TRANSLATION_UNIT);
    expect_value(mock_create_children_node_var, children_nb, 0);
    will_return(mock_create_children_node_var, DUMMY_AST_TRANSLATION_UNIT_NODE_P);
    will_return(stub_binding_statement_action, DUMMY_AST_ERROR_OR_BINDING_NODE_P);
    expect_value(mock_children_append_take, parent, DUMMY_AST_TRANSLATION_UNIT_NODE_P);
    expect_value(mock_children_append_take, child, DUMMY_AST_ERROR_OR_BINDING_NODE_P);
    will_return(mock_children_append_take, true);
    will_return(stub_reading_statement_action, DUMMY_AST_ERROR_OR_READING_NODE_P);
    expect_value(mock_children_append_take, parent, DUMMY_AST_TRANSLATION_UNIT_NODE_P);
    expect_value(mock_children_append_take, child, DUMMY_AST_ERROR_OR_READING_NODE_P);
    will_return(mock_children_append_take, true);
    will_return(stub_writing_statement_action, DUMMY_AST_ERROR_OR_WRITING_NODE_P);
    expect_value(mock_children_append_take, parent, DUMMY_AST_TRANSLATION_UNIT_NODE_P);
    expect_value(mock_children_append_take, child, DUMMY_AST_ERROR_OR_WRITING_NODE_P);
    will_return(mock_children_append_take, true);

    translation_unit_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_TRANSLATION_UNIT_NODE_P);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
    const struct CMUnitTest translation_unit_parse_tests[] = {
        cmocka_unit_test_setup_teardown(
            translation_unit_parse_create_error_node_for_LHS_when_0_statement_and_create_children_node_var_fails,
            translation_unit_parse_setup, translation_unit_parse_teardown),
        cmocka_unit_test_setup_teardown(
            translation_unit_parse_create_translation_unit_node_for_LHS_when_0_statement_and_create_children_node_var_succeeds,
            translation_unit_parse_setup, translation_unit_parse_teardown),
        cmocka_unit_test_setup_teardown(
            translation_unit_parse_cleans_up_and_create_error_node_for_LHS_when_1_statement_and_create_children_node_var_fails,
            translation_unit_parse_setup, translation_unit_parse_teardown),
        cmocka_unit_test_setup_teardown(
            translation_unit_parse_cleans_up_and_create_error_node_for_LHS_when_1_statement_and_children_append_take_fails,
            translation_unit_parse_setup, translation_unit_parse_teardown),
        cmocka_unit_test_setup_teardown(
            translation_unit_parse_create_translation_unit_node_for_LHS_when_1_statement_and_children_append_take_succeeds,
            translation_unit_parse_setup, translation_unit_parse_teardown),
        cmocka_unit_test_setup_teardown(
            translation_unit_parse_create_translation_unit_node_for_LHS_when_3_statement_and_children_append_take_succeeds_3_times,
            translation_unit_parse_setup, translation_unit_parse_teardown),
    };
    int failed = 0;
    failed += cmocka_run_group_tests(translation_unit_parse_tests, NULL, NULL);

    return failed;
}
