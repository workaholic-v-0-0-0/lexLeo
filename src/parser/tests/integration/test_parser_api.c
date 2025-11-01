// src/parser/tests/integration/test_parser_api.c

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <string.h>

#include "osal.h"

#include "parser_ctx.h"
#include "parser.tab.h"
#include "parser_api.h"
#include "lexer.yy.h"
#include "ast.h"



//-----------------------------------------------------------------------------
// GENERAL HELPERS
//-----------------------------------------------------------------------------


static FILE* open_memory_as_FILE(const char *s) {
#ifdef _GNU_SOURCE
    return osal_fmemopen_ro((void*)s, strlen(s));
#else
    FILE *f = tmpfile();
    if (!f) return NULL;
    fputs(s, f);
    rewind(f);
    return f;
#endif
}



//-----------------------------------------------------------------------------
// PARAMETRIC CASE STRUCTURE
//-----------------------------------------------------------------------------


typedef struct {
    yyscan_t arg_scanner;
    ast *ast_result;
    struct parser_ctx *arg_ctx;
    FILE *input_file;
} test_parser_api_ctx;

typedef void (*test_parser_api_expected_arg_out_fn_t)(test_parser_api_ctx *ctx);
typedef void (*test_parser_api_clean_up_fn_t)(test_parser_api_ctx *ctx);

typedef struct {

    const char *description;

    // arrange utilities
    parse_goal_t goal;
    const char *lexer_input;

    // assert utilities
    bool expected_returned_value;
    test_parser_api_expected_arg_out_fn_t expected_arg_out_fn;

    // test infrastructure cleanup utilities
    test_parser_api_clean_up_fn_t clean_up_fn;

    // information sharing utilities
    test_parser_api_ctx *ctx;

} test_parser_api_case;



//-----------------------------------------------------------------------------
// CLEANUP FUNCTION DEFINITIONS (test_parser_api_clean_up_fn_t)
//-----------------------------------------------------------------------------


static void destroy_nothing(test_parser_api_ctx *ctx) {
    (void)ctx;
}

static void destroy_result_ast(test_parser_api_ctx *ctx) {
    ast_destroy(ctx->ast_result);
}



//-----------------------------------------------------------------------------
// AST RESULT EXPECTATION FUNCTIONS (test_parser_api_expected_arg_out_fn_t)
//-----------------------------------------------------------------------------


static void is_null(test_parser_api_ctx *ctx) {
    assert_null(ctx->ast_result);
}

// forwarded for readability
static void a_equal_1(test_parser_api_ctx *ctx);
static void a_function_definition(test_parser_api_ctx *ctx);
static void a_tu_with_binding_function_call(test_parser_api_ctx *ctx);
static void a_number(test_parser_api_ctx *ctx);
static void a_block(test_parser_api_ctx *ctx);



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


static int setup(void **state) {
    test_parser_api_case *p = (test_parser_api_case *) *state;
    p->ctx->input_file = open_memory_as_FILE(p->lexer_input);
    assert_non_null(p->ctx->input_file);
    p->ctx->arg_scanner = NULL;
    assert_int_equal(0, yylex_init(&p->ctx->arg_scanner));
    yyset_in(p->ctx->input_file, p->ctx->arg_scanner);
    p->ctx->ast_result = NULL;
    return 0;
}

static int teardown(void **state) {
    test_parser_api_case *p = (test_parser_api_case *) *state;
    assert_int_equal(0, yylex_destroy(p->ctx->arg_scanner));
    fclose(p->ctx->input_file);
    if (p->clean_up_fn) p->clean_up_fn(p->ctx);
    return 0;
}



//-----------------------------------------------------------------------------
// TEST RUNNER
//-----------------------------------------------------------------------------


static void parser_api_test(void **state) {

    // ARRANGE
    test_parser_api_case *p = (test_parser_api_case *) *state;

    // ACT
    bool ret;
    switch (p->goal) {
        case PARSE_GOAL_TU:
            p->ctx->arg_ctx = get_g_parser_ctx_default_translation_unit();
            ret = parse_translation_unit(p->ctx->arg_scanner, &p->ctx->ast_result, p->ctx->arg_ctx);
            break;
        case PARSE_GOAL_ONE_STATEMENT:
            p->ctx->arg_ctx = get_g_parser_ctx_default_one_statement();
            ret = parse_one_statement(p->ctx->arg_scanner, &p->ctx->ast_result, p->ctx->arg_ctx);
            break;
        case PARSE_GOAL_READABLE:
            p->ctx->arg_ctx = get_g_parser_ctx_default_readable();
            ret = parse_readable(p->ctx->arg_scanner, &p->ctx->ast_result, p->ctx->arg_ctx);
            break;
        default:
            assert_false(true);
    }

    // ASSERT
    assert_int_equal(ret, p->expected_returned_value);
    p->expected_arg_out_fn(p->ctx);
}



//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------


static test_parser_api_ctx CTX_STATEMENT_SYNTAX_ERROR = {0};
static const test_parser_api_case STATEMENT_SYNTAX_ERROR = {
    .description = "parse_one_statement handle gracefully the syntax error \"a = 1\"",

    .goal = PARSE_GOAL_ONE_STATEMENT,
    .lexer_input = "a = 1",

    .expected_returned_value = false,
    .expected_arg_out_fn = is_null,

    .clean_up_fn = destroy_nothing,
    .ctx =&CTX_STATEMENT_SYNTAX_ERROR,
};

static test_parser_api_ctx CTX_STATEMENT_BINDING = {0};
static const test_parser_api_case STATEMENT_BINDING = {
    .description = "parse_one_statement successfully parse \"\
a = 1 ;\
\"",

    .goal = PARSE_GOAL_ONE_STATEMENT,
    .lexer_input = "a = 1 ;",

    .expected_returned_value = true,
    .expected_arg_out_fn = a_equal_1,

    .clean_up_fn = destroy_result_ast,
    .ctx =&CTX_STATEMENT_BINDING,
};

static test_parser_api_ctx CTX_STATEMENT_FUNCTION_DEFINITION = {0};
static const test_parser_api_case STATEMENT_FUNCTION_DEFINITION = {
    .description = "parse_one_statement successfully a function definition node",

    .goal = PARSE_GOAL_ONE_STATEMENT,
    .lexer_input = "\
define f(x y) { \
    z = 7 ; \
} \
",

    .expected_returned_value = true,
    .expected_arg_out_fn = a_function_definition,

    .clean_up_fn = destroy_result_ast,
    .ctx =&CTX_STATEMENT_FUNCTION_DEFINITION,
};

static test_parser_api_ctx CTX_TU_SYNTAX_ERROR = {0};
static const test_parser_api_case TU_SYNTAX_ERROR = {
    .description = "parse_translation_unit handle gracefully a syntax error",

    .goal = PARSE_GOAL_TU,
    .lexer_input = "\
a = 1 ; \
f(2)) ; \
",

    .expected_returned_value = false,
    .expected_arg_out_fn = is_null,

    .clean_up_fn = destroy_nothing,
    .ctx =&CTX_TU_SYNTAX_ERROR,
};

static test_parser_api_ctx CTX_TU_BINDING_FUNCTION_CALL = {0};
static const test_parser_api_case TU_BINDING_FUNCTION_CALL = {
    .description = "parse_translation_unit successfully parse a binding and a function call",

    .goal = PARSE_GOAL_TU,
    .lexer_input = "\
a = 1 ; \
f(2) ; \
",

    .expected_returned_value = true,
    .expected_arg_out_fn = a_tu_with_binding_function_call,

    .clean_up_fn = destroy_result_ast,
    .ctx =&CTX_TU_BINDING_FUNCTION_CALL,
};

static test_parser_api_ctx CTX_READABLE_SYNTAX_ERROR = {0};
static const test_parser_api_case READABLE_SYNTAX_ERROR = {
    .description = "parse_readable handle gracefully a syntax error",

    .goal = PARSE_GOAL_READABLE,
    .lexer_input = "\
a=1; \
",

    .expected_returned_value = false,
    .expected_arg_out_fn = is_null,

    .clean_up_fn = destroy_nothing,
    .ctx =&CTX_READABLE_SYNTAX_ERROR,
};

static test_parser_api_ctx CTX_READABLE_NUMBER = {0};
static const test_parser_api_case READABLE_NUMBER = {
    .description = "parse_readable successfully parse a number",

    .goal = PARSE_GOAL_READABLE,
    .lexer_input = "\
7 \
",

    .expected_returned_value = true,
    .expected_arg_out_fn = a_number,

    .clean_up_fn = destroy_result_ast,
    .ctx =&CTX_READABLE_NUMBER,
};

static test_parser_api_ctx CTX_READABLE_BLOCK = {0};
static const test_parser_api_case READABLE_BLOCK = {
    .description = "parse_readable successfully parse a block",

    .goal = PARSE_GOAL_READABLE,
    .lexer_input = "\
{\n\
    a = 1 ;\n\
    f(2) ;\n\
}\n\
",

    .expected_returned_value = true,
    .expected_arg_out_fn = a_block,

    .clean_up_fn = destroy_result_ast,
    .ctx =&CTX_READABLE_BLOCK,
};



//-----------------------------------------------------------------------------
// PARAMETRIC CASES REGISTRY
//-----------------------------------------------------------------------------
//
// Centralized registry of all parametric test cases.
// Each case is defined once here, then automatically expanded into a
// CMocka CMUnitTest array below.
//
// To add a new test case:
//     1. Define its `test_parser_api_case` struct (e.g. STATEMENT_BINDING)
//     2. Add one line in PARSER_API_CASES() below
//

#define PARSER_API_CASES(X) \
    X(STATEMENT_SYNTAX_ERROR) \
    X(STATEMENT_BINDING) \
    X(STATEMENT_FUNCTION_DEFINITION) \
    X(TU_SYNTAX_ERROR) \
    X(TU_BINDING_FUNCTION_CALL) \
    X(READABLE_SYNTAX_ERROR) \
    X(READABLE_NUMBER) \
    X(READABLE_BLOCK) \

#define MAKE_TEST(CASE) \
    { .name = CASE.description, \
    .test_func = parser_api_test, \
    .setup_func = setup, \
    .teardown_func = teardown, \
    .initial_state = (void*)&CASE },

static const struct CMUnitTest parser_api_tests[] = {
    PARSER_API_CASES(MAKE_TEST)
};

#undef MAKE_TEST



//-----------------------------------------------------------------------------
// NON PARAMETRIC TESTS
//-----------------------------------------------------------------------------


static void test_parse_one_statement_three_calls(void **state);
static void test_parse_one_statement_three_calls(void **state) {
    (void)state;

    // ARRANGE
    const char *src = "a=1;b=2;";
    FILE *f = open_memory_as_FILE(src);
    assert_non_null(f);
    yyscan_t scanner = NULL;
    assert_int_equal(0, yylex_init(&scanner));
    yyset_in(f, scanner);
    struct parser_ctx *ctx = get_g_parser_ctx_default_one_statement();
    struct ast *node = NULL;

    // ACT
    bool ret = parse_one_statement(scanner, &node, ctx);

    // ASSERT
    assert_true(ret);
    assert_non_null(node);
    assert_int_equal(node->type, AST_TYPE_BINDING);
    assert_non_null(node->children);
    assert_int_equal(node->children->children_nb, 2);
    assert_non_null(node->children->children);
    assert_non_null(node->children->children[0]);
    assert_int_equal(node->children->children[0]->type, AST_TYPE_DATA_WRAPPER);
    assert_non_null(node->children->children[0]->data);
    assert_int_equal(node->children->children[0]->data->type, TYPE_SYMBOL_NAME);
    assert_non_null(node->children->children[0]->data->data.string_value);
    assert_string_equal(node->children->children[0]->data->data.string_value, "a");
    assert_non_null(node->children->children[1]);
    assert_int_equal(node->children->children[1]->type, AST_TYPE_DATA_WRAPPER);
    assert_non_null(node->children->children[1]->data);
    assert_int_equal(node->children->children[1]->data->type, TYPE_INT);
    assert_int_equal(node->children->children[1]->data->data.int_value, 1);
    ast_destroy(node);
    node = NULL;

    // ACT
    ret = parse_one_statement(scanner, &node, ctx);

    // ASSERT
    assert_true(ret);
    assert_non_null(node);
    assert_int_equal(node->type, AST_TYPE_BINDING);
    assert_non_null(node->children);
    assert_int_equal(node->children->children_nb, 2);
    assert_non_null(node->children->children);
    assert_non_null(node->children->children[0]);
    assert_int_equal(node->children->children[0]->type, AST_TYPE_DATA_WRAPPER);
    assert_non_null(node->children->children[0]->data);
    assert_int_equal(node->children->children[0]->data->type, TYPE_SYMBOL_NAME);
    assert_non_null(node->children->children[0]->data->data.string_value);
    assert_string_equal(node->children->children[0]->data->data.string_value, "b");
    assert_non_null(node->children->children[1]);
    assert_int_equal(node->children->children[1]->type, AST_TYPE_DATA_WRAPPER);
    assert_non_null(node->children->children[1]->data);
    assert_int_equal(node->children->children[1]->data->type, TYPE_INT);
    assert_int_equal(node->children->children[1]->data->data.int_value, 2);
    ast_destroy(node);
    node = NULL;

    // ACT
    ret = parse_one_statement(scanner, &node, ctx);

    // ASSERT
    assert_false(ret);
    assert_null(node);

    assert_int_equal(0, yylex_destroy(scanner));
    fclose(f);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
    const struct CMUnitTest parser_api_non_parametric_tests[] = {
        cmocka_unit_test(test_parse_one_statement_three_calls),
    };

    int failed = 0;
    failed += cmocka_run_group_tests(parser_api_non_parametric_tests, NULL, NULL);
    failed += cmocka_run_group_tests(parser_api_tests, NULL, NULL);

    return failed;
}



//-----------------------------------------------------------------------------
// AST RESULT EXPECTATION FUNCTIONS (test_parser_api_expected_arg_out_fn_t)
// IN A VERY VERBOSE STYLE
//-----------------------------------------------------------------------------


static void a_equal_1(test_parser_api_ctx *ctx) {
    ast *res = ctx->ast_result;
    assert_non_null(res);
    assert_int_equal(res->type, AST_TYPE_BINDING);
    ast_children_t *children_info = res->children;
    assert_non_null(children_info);
    assert_int_equal(children_info->children_nb, 2);
    struct ast **children = children_info->children;
    assert_non_null(children);
    assert_non_null(children[0]);
    assert_int_equal(children[0]->type, AST_TYPE_DATA_WRAPPER);
    typed_data *data = children[0]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_SYMBOL_NAME);
    char *string_value = data->data.string_value;
    assert_non_null(string_value);
    assert_string_equal(string_value, "a");
    assert_non_null(children[1]);
    assert_int_equal(children[1]->type, AST_TYPE_DATA_WRAPPER);
    data = children[1]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_INT);
    assert_int_equal(data->data.int_value, 1);
}

static void a_function_definition(test_parser_api_ctx *ctx) {
    ast *res = ctx->ast_result;
    assert_non_null(res);
    assert_int_equal(res->type, AST_TYPE_FUNCTION_DEFINITION);
    ast_children_t *children_info = res->children;
    assert_non_null(children_info);
    assert_int_equal(children_info->children_nb, 1);
    struct ast **children = children_info->children;
    assert_non_null(children);
    assert_non_null(children[0]);
    assert_int_equal(children[0]->type, AST_TYPE_FUNCTION);
    children_info = children[0]->children;
    assert_int_equal(children_info->children_nb, 3);
    children = children_info->children;
    assert_non_null(children);
    assert_non_null(children[0]);
    assert_int_equal(children[0]->type, AST_TYPE_DATA_WRAPPER);
    typed_data *data = children[0]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_SYMBOL_NAME);
    char *string_value = data->data.string_value;
    assert_non_null(string_value);
    assert_string_equal(string_value, "f");
    assert_non_null(children[1]);
    assert_int_equal(children[1]->type, AST_TYPE_LIST_OF_PARAMETERS);
    assert_int_equal(children[1]->children->children_nb, 1);
    assert_non_null(children[1]->children->children);
    assert_non_null(children[1]->children->children[0]);
    assert_int_equal(children[1]->children->children[0]->type, AST_TYPE_PARAMETERS);
    assert_int_equal(children[1]->children->children[0]->children->children_nb, 2);
    assert_non_null(children[1]->children->children[0]->children);
    assert_non_null(children[1]->children->children[0]->children->children[0]);
    assert_int_equal(children[1]->children->children[0]->children->children[0]->type, AST_TYPE_DATA_WRAPPER);
    data = children[1]->children->children[0]->children->children[0]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_SYMBOL_NAME);
    string_value = data->data.string_value;
    assert_non_null(string_value);
    assert_string_equal(string_value, "x");
    assert_non_null(children[1]->children->children[0]->children->children[1]);
    assert_int_equal(children[1]->children->children[0]->children->children[1]->type, AST_TYPE_DATA_WRAPPER);
    data = children[1]->children->children[0]->children->children[1]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_SYMBOL_NAME);
    string_value = data->data.string_value;
    assert_non_null(string_value);
    assert_string_equal(string_value, "y");
    assert_int_equal(children[2]->type, AST_TYPE_BLOCK);
    assert_int_equal(children[2]->children->children_nb, 1);
    assert_non_null(children[2]->children);
    assert_non_null(children[2]->children->children[0]);
    assert_int_equal(children[2]->children->children[0]->type, AST_TYPE_BLOCK_ITEMS);
    assert_int_equal(children[2]->children->children[0]->children->children_nb, 1);
    assert_non_null(children[2]->children->children[0]->children);
    assert_non_null(children[2]->children->children[0]->children->children[0]);
    assert_int_equal(children[2]->children->children[0]->children->children[0]->type, AST_TYPE_BINDING);
    assert_int_equal(children[2]->children->children[0]->children->children[0]->children->children_nb, 2);
    assert_non_null(children[2]->children->children[0]->children->children[0]->children);
    assert_non_null(children[2]->children->children[0]->children->children[0]->children->children[0]);
    assert_int_equal(children[2]->children->children[0]->children->children[0]->children->children[0]->type, AST_TYPE_DATA_WRAPPER);
    data = children[2]->children->children[0]->children->children[0]->children->children[0]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_SYMBOL_NAME);
    string_value = data->data.string_value;
    assert_non_null(string_value);
    assert_string_equal(string_value, "z");
    assert_non_null(children[2]->children->children[0]->children->children[0]->children->children[1]);
    assert_int_equal(children[2]->children->children[0]->children->children[0]->children->children[1]->type, AST_TYPE_DATA_WRAPPER);
    data = children[2]->children->children[0]->children->children[0]->children->children[1]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_INT);
    assert_int_equal(data->data.int_value, 7);
}

static void a_tu_with_binding_function_call(test_parser_api_ctx *ctx) {
    ast *res = ctx->ast_result;
    assert_non_null(res);
    assert_int_equal(res->type, AST_TYPE_TRANSLATION_UNIT);
    ast_children_t *children_info = res->children;
    assert_non_null(children_info);
    assert_int_equal(children_info->children_nb, 2);
    struct ast **children = children_info->children;
    assert_non_null(children);
    assert_non_null(children[0]);
    assert_int_equal(children[0]->type, AST_TYPE_BINDING);
    children_info = children[0]->children;
    assert_non_null(children_info);
    assert_int_equal(children_info->children_nb, 2);
    children = children_info->children;
    assert_non_null(children);
    assert_non_null(children[0]);
    assert_int_equal(children[0]->type, AST_TYPE_DATA_WRAPPER);
    typed_data *data = children[0]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_SYMBOL_NAME);
    char *string_value = data->data.string_value;
    assert_non_null(string_value);
    assert_string_equal(string_value, "a");
    assert_non_null(children[1]);
    assert_int_equal(children[1]->type, AST_TYPE_DATA_WRAPPER);
    data = children[1]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_INT);
    assert_int_equal(data->data.int_value, 1);
    children = res->children->children;
    assert_non_null(children[1]);
    assert_int_equal(children[1]->type, AST_TYPE_FUNCTION_CALL);
    children_info = children[1]->children;
    assert_non_null(children_info);
    assert_int_equal(children_info->children_nb, 2);
    children = children_info->children;
    assert_non_null(children);
    assert_non_null(children[0]);
    assert_int_equal(children[0]->type, AST_TYPE_DATA_WRAPPER);
    data = children[0]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_SYMBOL_NAME);
    string_value = data->data.string_value;
    assert_non_null(string_value);
    assert_string_equal(string_value, "f");
    assert_non_null(children[1]);
    assert_int_equal(children[1]->type, AST_TYPE_LIST_OF_NUMBERS);
    children_info = children[1]->children;
    assert_non_null(children_info);
    assert_int_equal(children_info->children_nb, 1);
    children = children_info->children;
    assert_non_null(children);
    assert_non_null(children[0]);
    assert_int_equal(children[0]->type, AST_TYPE_NUMBERS);
    children_info = children[0]->children;
    assert_non_null(children_info);
    assert_int_equal(children_info->children_nb, 1);
    children = children_info->children;
    assert_non_null(children);
    assert_non_null(children[0]);
    assert_int_equal(children[0]->type, AST_TYPE_DATA_WRAPPER);
    data = children[0]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_INT);
    assert_int_equal(data->data.int_value, 2);
}

static void a_number(test_parser_api_ctx *ctx) {
    ast *res = ctx->ast_result;
    assert_non_null(res);
    assert_int_equal(res->type, AST_TYPE_DATA_WRAPPER);
    assert_non_null(res->data);
    assert_int_equal(res->data->type, TYPE_INT);
    assert_int_equal(res->data->data.int_value, 7);
}

static void a_block(test_parser_api_ctx *ctx) {
    ast *res = ctx->ast_result;
    assert_non_null(res);
    assert_int_equal(res->type, AST_TYPE_BLOCK);
    ast_children_t *children_info = res->children;
    assert_non_null(children_info);

    assert_int_equal(children_info->children_nb, 1);
    struct ast **children = children_info->children;
    assert_non_null(children);
    assert_non_null(children[0]);
    assert_int_equal(children[0]->type, AST_TYPE_BLOCK_ITEMS);
    children_info = children[0]->children;
    assert_non_null(children_info);

    assert_int_equal(children_info->children_nb, 2);
    children = children_info->children;
    assert_non_null(children);
    assert_non_null(children[0]);
    assert_int_equal(children[0]->type, AST_TYPE_BINDING);
    children_info = children[0]->children;
    assert_non_null(children_info);
    assert_int_equal(children_info->children_nb, 2);
    children = children_info->children;
    assert_non_null(children);
    assert_non_null(children[0]);
    assert_int_equal(children[0]->type, AST_TYPE_DATA_WRAPPER);
    typed_data *data = children[0]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_SYMBOL_NAME);
    char *string_value = data->data.string_value;
    assert_non_null(string_value);
    assert_string_equal(string_value, "a");
    assert_non_null(children[1]);
    assert_int_equal(children[1]->type, AST_TYPE_DATA_WRAPPER);
    data = children[1]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_INT);
    assert_int_equal(data->data.int_value, 1);
    children = res->children->children[0]->children->children;
    assert_non_null(children[1]);
    assert_int_equal(children[1]->type, AST_TYPE_FUNCTION_CALL);
    children_info = children[1]->children;
    assert_non_null(children_info);
    assert_int_equal(children_info->children_nb, 2);
    children = children_info->children;
    assert_non_null(children);
    assert_non_null(children[0]);
    assert_int_equal(children[0]->type, AST_TYPE_DATA_WRAPPER);
    data = children[0]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_SYMBOL_NAME);
    string_value = data->data.string_value;
    assert_non_null(string_value);
    assert_string_equal(string_value, "f");
    assert_non_null(children[1]);
    assert_int_equal(children[1]->type, AST_TYPE_LIST_OF_NUMBERS);
    children_info = children[1]->children;
    assert_non_null(children_info);
    assert_int_equal(children_info->children_nb, 1);
    children = children_info->children;
    assert_non_null(children);
    assert_non_null(children[0]);
    assert_int_equal(children[0]->type, AST_TYPE_NUMBERS);
    children_info = children[0]->children;
    assert_non_null(children_info);
    assert_int_equal(children_info->children_nb, 1);
    children = children_info->children;
    assert_non_null(children);
    assert_non_null(children[0]);
    assert_int_equal(children[0]->type, AST_TYPE_DATA_WRAPPER);
    data = children[0]->data;
    assert_non_null(data);
    assert_int_equal(data->type, TYPE_INT);
    assert_int_equal(data->data.int_value, 2);
}
