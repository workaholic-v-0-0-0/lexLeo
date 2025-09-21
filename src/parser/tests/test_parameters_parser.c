// src/parser/tests/test_parameters_parser.c

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>

#include "parameters_parser.tab.h"
#include "parser_ctx.h"
#include "mock_lexer.h"



//-----------------------------------------------------------------------------
// GLOBALS, TYPES, DUMMIES AND "MAGIC NUMBER KILLERS"
//-----------------------------------------------------------------------------


static ast *parsed_ast;

static const char DUMMY[7];
static ast *const DUMMY_AST_ERROR_P = (ast *) &DUMMY[0];
static ast *const DUMMY_PARAMETERS_NODE = (ast *) &DUMMY[1];
static ast *const DUMMY_CHILDREN_NODE_P = (ast *) &DUMMY[2];
static ast *const DUMMY_SYMBOL_NAME_NODE = (ast *) &DUMMY[3];
static error_info *const DUMMY_ERROR_INFO_P = (error_info *) &DUMMY[4];
static ast FAKE_AST_ERROR = {AST_TYPE_ERROR, .error = DUMMY_ERROR_INFO_P};
static ast *const DUMMY_SYMBOL_NAME_NODE_1 = (ast *) &DUMMY[5];
static ast *const DUMMY_SYMBOL_NAME_NODE_2 = (ast *) &DUMMY[6];



//-----------------------------------------------------------------------------
// MOCKS AND FAKES
//-----------------------------------------------------------------------------


mock_token seq_0_parameter[] = {
    {0, {0}}
};

mock_token seq_1_parameter[] = {
    {SYMBOL_NAME, {.symbol_name_value = "symbol_name_1"}},
    {0, {0}}
};

mock_token seq_2_parameters[] = {
    {SYMBOL_NAME, {.symbol_name_value = "symbol_name_1"}},
    {SYMBOL_NAME, {.symbol_name_value = "symbol_name_2"}},
    {0, {0}}
};

ast *stub_symbol_name_atom_action(void) {
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
// parameters_parse TESTS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


// Action under test via injected stubs:
// "parameters: /* empty */ | parameters symbol_name_atom

// mocked:
//  - action of grammar rule that the rule under test depends on:
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


static int parameters_parse_setup(void **state) {
    (void)state;
    parsed_ast = NULL;
    mock_lex_reset();
    mock_ctx.ops.children_append_take = mock_children_append_take;
    mock_ctx.ops.create_children_node_var = mock_create_children_node_var;
    mock_ctx.ops.create_error_node_or_sentinel = mock_create_error_node_or_sentinel;
    mock_ctx.ops.destroy = mock_destroy;
    return 0;
}

static int parameters_parse_teardown(void **state) {
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
//    - type: AST_TYPE_PARAMETERS
//    - children_nb: 0
//    - no more arg
//  - calls create_error_node_or_sentinel with:
//    - code: AST_ERROR_CODE_PARAMETERS_NODE_CREATION_FAILED
//    - message: "ast creation for parameters node failed"
//  - gives create_error_node_or_sentinel returned value for the parameters's semantic value
static void parameters_parse_create_error_node_for_LHS_when_0_parameter_and_create_children_node_var_fails(void **state) {
    mock_lex_set(seq_0_parameter, 1);
    expect_value(mock_create_children_node_var, type, AST_TYPE_PARAMETERS);
    expect_value(mock_create_children_node_var, children_nb, 0);
    will_return(mock_create_children_node_var, NULL);
    expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_PARAMETERS_NODE_CREATION_FAILED);
    expect_string(mock_create_error_node_or_sentinel, message, "ast creation for parameters node failed");
    will_return(mock_create_error_node_or_sentinel, DUMMY_AST_ERROR_P);

    parameters_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_P);
}

// Given:
//  - lexer returns:
//    - EOF
//  - ast_create_children_node_var will succeed
// Expected:
//  - calls ast_create_children_node_var with:
//    - type: AST_TYPE_PARAMETERS
//    - children_nb: 0
//    - no more arg
//  - gives ast_create_children_node_var returned value for the parameters's semantic value
static void parameters_parse_creates_parameters_node_for_LHS_when_0_parameter_and_create_children_node_var_succeeds(void **state) {
    mock_lex_set(seq_0_parameter, 1);
    expect_value(mock_create_children_node_var, type, AST_TYPE_PARAMETERS);
    expect_value(mock_create_children_node_var, children_nb, 0);
    will_return(mock_create_children_node_var, DUMMY_PARAMETERS_NODE);

    parameters_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_PARAMETERS_NODE);
}

// Given:
//  - lexer returns:
//    - SYMBOL_NAME("symbol_name_1")
//  - ast_create_children_node_var will fail
// Expected:
//  - epsilon -> parameters triggers a call to ast_create_children_node_var with:
//    - type: AST_TYPE_PARAMETERS
//    - children_nb: 0
//    (no additional argument)
//  - calls create_error_node_or_sentinel with:
//    - code: AST_ERROR_CODE_PARAMETERS_NODE_CREATION_FAILED
//    - message: "ast creation for parameters node failed"
//  - the error node returned by create_error_node_or_sentinel becomes the semantic value of
//    parameters produced by the epsilon-reduction
//  - action of the rule "symbol_name_atom: SYMBOL_NAME" is executed,
//    this yields a symbol_name_atom (semantic value provided here by a stub)
//  - when reducing parameters : parameters symbol_name_atom
//    - NO CALL TO ast_children_append_take must occur, since $1 is already an error node
//    - ast_destroy is called with $2 (the symbol_name_atom returned by the stub)
//    - the semantic value of the LHS parameters is the same error node as $1
static void parameters_parse_cleans_up_and_create_error_node_for_LHS_when_1_parameter_and_create_children_node_var_fails(void **state) {
    mock_lex_set(seq_1_parameter, 2);
    expect_value(mock_create_children_node_var, type, AST_TYPE_PARAMETERS);
    expect_value(mock_create_children_node_var, children_nb, 0);
    will_return(mock_create_children_node_var, NULL);
    expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_PARAMETERS_NODE_CREATION_FAILED);
    expect_string(mock_create_error_node_or_sentinel, message, "ast creation for parameters node failed");
    will_return(mock_create_error_node_or_sentinel, &FAKE_AST_ERROR);
    will_return(stub_symbol_name_atom_action, DUMMY_SYMBOL_NAME_NODE);
    expect_value(mock_destroy, root, DUMMY_SYMBOL_NAME_NODE);

    parameters_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, &FAKE_AST_ERROR);
}

// Given:
//  - lexer returns:
//    - SYMBOL_NAME("symbol_name_1")
//  - ast_create_children_node_var will succeed
//  - ast_children_append_take will fail
// Expected:
//  - epsilon -> parameters triggers a call to ast_create_children_node_var with:
//    - type: AST_TYPE_PARAMETERS
//    - children_nb: 0
//    (no additional argument)
//  - the returned value of ast_create_children_node_var becomes the semantic value of
//    parameters produced by the epsilon-reduction
//  - action of the rule "symbol_name_atom: SYMBOL_NAME" is executed,
//    this yields a symbol_name_atom (semantic value provided here by a stub)
//  - when reducing parameters : parameters symbol_name_atom
//    - calls ast_children_append_take with:
//      - parent: $1 (returned value of ast_create_children_node_var)
//      - child: $2 (provided by stub)
//    - calls ast_destroy with:
//      - root: $1
//    - calls ast_destroy with:
//      - root: $2
//    - calls create_error_node_or_sentinel with:
//      - code: AST_ERROR_CODE_PARAMETERS_APPEND_FAILED
//      - message: "ast append failed when adding a parameter to a list of parameters"
//    - the returned value of create_error_node_or_sentinel becomes the semantic value of the LHS parameters
static void parameters_parse_cleans_up_and_create_error_node_for_LHS_when_1_parameter_and_children_append_take_fails(void **state) {
    mock_lex_set(seq_1_parameter, 2);
    expect_value(mock_create_children_node_var, type, AST_TYPE_PARAMETERS);
    expect_value(mock_create_children_node_var, children_nb, 0);
    will_return(mock_create_children_node_var, DUMMY_PARAMETERS_NODE);
    will_return(stub_symbol_name_atom_action, DUMMY_SYMBOL_NAME_NODE);
    expect_value(mock_children_append_take, parent, DUMMY_PARAMETERS_NODE);
    expect_value(mock_children_append_take, child, DUMMY_SYMBOL_NAME_NODE);
    will_return(mock_children_append_take, false);
    expect_value(mock_destroy, root, DUMMY_PARAMETERS_NODE);
    expect_value(mock_destroy, root, DUMMY_SYMBOL_NAME_NODE);
    expect_value(mock_create_error_node_or_sentinel, code, AST_ERROR_CODE_PARAMETERS_APPEND_FAILED);
    expect_string(mock_create_error_node_or_sentinel, message, "ast append failed when adding a parameter to a list of parameters");
    will_return(mock_create_error_node_or_sentinel, DUMMY_AST_ERROR_P);

    parameters_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_AST_ERROR_P);
}

// Given:
//  - lexer returns:
//    - SYMBOL_NAME("symbol_name_1")
//  - ast_create_children_node_var will succeed
//  - ast_children_append_take will succeed
// Expected:
//  - epsilon -> parameters triggers a call to ast_create_children_node_var with:
//    - type: AST_TYPE_PARAMETERS
//    - children_nb: 0
//    (no additional argument)
//  - the returned value of ast_create_children_node_var becomes the semantic value of
//    parameters produced by the epsilon-reduction
//  - action of the rule "symbol_name_atom: SYMBOL_NAME" is executed,
//    this yields a symbol_name_atom (semantic value provided here by a stub)
//  - when reducing parameters : parameters symbol_name_atom
//    - calls ast_children_append_take with:
//      - parent: $1 (returned value of ast_create_children_node_var)
//      - child: $2 (provided by stub)
//    - the returned value of ast_children_append_take becomes the semantic value of the LHS parameters
static void parameters_parse_create_parameters_node_for_LHS_when_1_parameter_and_children_append_take_succeeds(void **state) {
    mock_lex_set(seq_1_parameter, 2);
    expect_value(mock_create_children_node_var, type, AST_TYPE_PARAMETERS);
    expect_value(mock_create_children_node_var, children_nb, 0);
    will_return(mock_create_children_node_var, DUMMY_PARAMETERS_NODE);
    will_return(stub_symbol_name_atom_action, DUMMY_SYMBOL_NAME_NODE);
    expect_value(mock_children_append_take, parent, DUMMY_PARAMETERS_NODE);
    expect_value(mock_children_append_take, child, DUMMY_SYMBOL_NAME_NODE);
    will_return(mock_children_append_take, true);

    parameters_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_PARAMETERS_NODE);
}

// Given:
//  - lexer returns:
//    - SYMBOL_NAME("symbol_name_1")
//    - SYMBOL_NAME("symbol_name_2")
//  - ast_create_children_node_var will succeed
//  - every ast_children_append_take will succeed
// Expected:
//  - epsilon -> parameters
//    - $$ = ast_create_children_node_var(AST_TYPE_PARAMETERS, 0)
//  - SYMBOL_NAME -> symbol_name_atom (stubbed)
//    - $$ = stub_symbol_name_atom_action()
//  - parameters symbol_name_atom -> parameters
//    - $$ = ctx->ops.children_append_take($1, $2)
//  - SYMBOL_NAME -> symbol_name_atom (stubbed)
//    - $$ = stub_symbol_name_atom_action()
//  - parameters symbol_name_atom -> parameters
//    - $$ = ctx->ops.children_append_take($1, $2)
static void parameters_parse_create_parameters_node_for_LHS_when_2_parameters_and_children_append_take_succeeds_2_times(void **state) {
    mock_lex_set(seq_2_parameters, 3);
    expect_value(mock_create_children_node_var, type, AST_TYPE_PARAMETERS);
    expect_value(mock_create_children_node_var, children_nb, 0);
    will_return(mock_create_children_node_var, DUMMY_PARAMETERS_NODE);
    will_return(stub_symbol_name_atom_action, DUMMY_SYMBOL_NAME_NODE_1);
    expect_value(mock_children_append_take, parent, DUMMY_PARAMETERS_NODE);
    expect_value(mock_children_append_take, child, DUMMY_SYMBOL_NAME_NODE_1);
    will_return(mock_children_append_take, true);
    will_return(stub_symbol_name_atom_action, DUMMY_SYMBOL_NAME_NODE_2);
    expect_value(mock_children_append_take, parent, DUMMY_PARAMETERS_NODE);
    expect_value(mock_children_append_take, child, DUMMY_SYMBOL_NAME_NODE_2);
    will_return(mock_children_append_take, true);

    parameters_parse(NULL, &parsed_ast, &mock_ctx);

    assert_ptr_equal(parsed_ast, DUMMY_PARAMETERS_NODE);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
    const struct CMUnitTest parameters_parse_tests[] = {
        cmocka_unit_test_setup_teardown(
            parameters_parse_create_error_node_for_LHS_when_0_parameter_and_create_children_node_var_fails,
            parameters_parse_setup, parameters_parse_teardown),
        cmocka_unit_test_setup_teardown(
            parameters_parse_creates_parameters_node_for_LHS_when_0_parameter_and_create_children_node_var_succeeds,
            parameters_parse_setup, parameters_parse_teardown),
        cmocka_unit_test_setup_teardown(
            parameters_parse_cleans_up_and_create_error_node_for_LHS_when_1_parameter_and_create_children_node_var_fails,
            parameters_parse_setup, parameters_parse_teardown),
        cmocka_unit_test_setup_teardown(
            parameters_parse_cleans_up_and_create_error_node_for_LHS_when_1_parameter_and_children_append_take_fails,
            parameters_parse_setup, parameters_parse_teardown),
        cmocka_unit_test_setup_teardown(
            parameters_parse_create_parameters_node_for_LHS_when_1_parameter_and_children_append_take_succeeds,
            parameters_parse_setup, parameters_parse_teardown),
        cmocka_unit_test_setup_teardown(
            parameters_parse_create_parameters_node_for_LHS_when_2_parameters_and_children_append_take_succeeds_2_times,
            parameters_parse_setup, parameters_parse_teardown),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(parameters_parse_tests, NULL, NULL);

    return failed;
}
