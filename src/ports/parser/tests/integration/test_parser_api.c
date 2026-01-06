// src/ports/parser/tests/integration/test_parser_api.c

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include "parser_cfg.h"
#include "parser.tab.h"
#include "parser_api.h"
#include "lexer_legacy.yy.h"
#include "ast.h"
#include "input_provider_legacy.h"


//-----------------------------------------------------------------------------
// PARAMETRIC CASE STRUCTURE
//-----------------------------------------------------------------------------


typedef struct {
	yyscan_t arg_scanner;
	ast *ast_result;
	struct parser_cfg arg_ctx;
	input_provider *arg_input_provider_legacy;
} test_parser_api_ctx;

typedef void (*test_parser_api_expected_arg_out_fn_t)(test_parser_api_ctx *ctx);

typedef void (*test_parser_api_clean_up_fn_t)(test_parser_api_ctx *ctx);

typedef struct {
	const char *description;

	// arrange utilities
	parse_goal_t goal;
	const char *lexer_input;

	// assert utilities
	parse_status expected_returned_value;
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
	(void) ctx;
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

	// arg_scanner initialisation
	p->ctx->arg_scanner = NULL;
	assert_int_equal(0, yylex_init(&p->ctx->arg_scanner));

	// arg_input_provider_legacy initialisation
	p->ctx->arg_input_provider_legacy = input_provider_legacy_create();
	input_provider_legacy_set_mode_chunks(p->ctx->arg_input_provider_legacy);

	// bind arg_input_provider_legacy to arg_scanner
	assert_true(input_provider_legacy_bind_to_scanner(p->ctx->arg_input_provider_legacy, p->ctx->arg_scanner));

	p->ctx->ast_result = NULL;
	return 0;
}

static int teardown(void **state) {
	test_parser_api_case *p = (test_parser_api_case *) *state;
	assert_int_equal(0, yylex_destroy(p->ctx->arg_scanner));
	input_provider_legacy_destroy(p->ctx->arg_input_provider_legacy);
	if (p->clean_up_fn) p->clean_up_fn(p->ctx);
	return 0;
}


//-----------------------------------------------------------------------------
// TEST RUNNER
//-----------------------------------------------------------------------------


static void parser_api_test(void **state) {
	// ARRANGE
	test_parser_api_case *p = (test_parser_api_case *) *state;
	input_provider_legacy_append_string_as_line(
		p->ctx->arg_input_provider_legacy,
		p->lexer_input
	);

	// ACT
	parse_status ret;
	switch (p->goal) {
		case PARSE_GOAL_TU:
			p->ctx->arg_ctx = get_parser_cfg_translation_unit();
			ret = parse_translation_unit(p->ctx->arg_scanner, &p->ctx->ast_result, &p->ctx->arg_ctx,
			                             p->ctx->arg_input_provider_legacy);
			break;
		case PARSE_GOAL_ONE_STATEMENT:
			p->ctx->arg_ctx = get_parser_cfg_one_statement();
			ret = parse_one_statement(p->ctx->arg_scanner, &p->ctx->ast_result, &p->ctx->arg_ctx,
			                          p->ctx->arg_input_provider_legacy);
			break;
		case PARSE_GOAL_READABLE:
			p->ctx->arg_ctx = get_parser_cfg_readable();
			ret = parse_readable(p->ctx->arg_scanner, &p->ctx->ast_result, &p->ctx->arg_ctx, p->ctx->arg_input_provider_legacy);
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
	.description = "parse_one_statement handle gracefully the syntax error \"a = ;\"",

	.goal = PARSE_GOAL_ONE_STATEMENT,
	.lexer_input = "a = ;",

	.expected_returned_value = PARSE_STATUS_ERROR,
	.expected_arg_out_fn = is_null,

	.clean_up_fn = destroy_nothing,
	.ctx = &CTX_STATEMENT_SYNTAX_ERROR,
};

static test_parser_api_ctx CTX_STATEMENT_INCOMPLETE = {0};
static const test_parser_api_case STATEMENT_INCOMPLETE = {
	.description = "parse_one_statement returns PARSE_STATUS_INCOMPLETE when \"a = \"",

	.goal = PARSE_GOAL_ONE_STATEMENT,
	.lexer_input = "a = ",

	.expected_returned_value = PARSE_STATUS_INCOMPLETE,
	.expected_arg_out_fn = is_null,

	.clean_up_fn = destroy_nothing,
	.ctx = &CTX_STATEMENT_INCOMPLETE,
};

static test_parser_api_ctx CTX_STATEMENT_BINDING = {0};
static const test_parser_api_case STATEMENT_BINDING = {
	.description = "parse_one_statement successfully parse \"\
a = 1 ;\
\"",

	.goal = PARSE_GOAL_ONE_STATEMENT,
	.lexer_input = "a = 1 ;",

	.expected_returned_value = PARSE_STATUS_OK,
	.expected_arg_out_fn = a_equal_1,

	.clean_up_fn = destroy_result_ast,
	.ctx = &CTX_STATEMENT_BINDING,
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

	.expected_returned_value = PARSE_STATUS_OK,
	.expected_arg_out_fn = a_function_definition,

	.clean_up_fn = destroy_result_ast,
	.ctx = &CTX_STATEMENT_FUNCTION_DEFINITION,
};

static test_parser_api_ctx CTX_TU_SYNTAX_ERROR = {0};
static const test_parser_api_case TU_SYNTAX_ERROR = {
	.description = "parse_translation_unit handle gracefully a syntax error",

	.goal = PARSE_GOAL_TU,
	.lexer_input = "\
a = 1 ; \
f(2)) ; \
",

	.expected_returned_value = PARSE_STATUS_ERROR,
	.expected_arg_out_fn = is_null,

	.clean_up_fn = destroy_nothing,
	.ctx = &CTX_TU_SYNTAX_ERROR,
};

static test_parser_api_ctx CTX_TU_INCOMPLETE = {0};
static const test_parser_api_case TU_INCOMPLETE = {
	.description = "parse_translation_unit returns PARSE_STATUS_INCOMPLETE when incomplete TU",

	.goal = PARSE_GOAL_TU,
	.lexer_input = "\
a = 1 ; \
f(2) \
",

	.expected_returned_value = PARSE_STATUS_INCOMPLETE,
	.expected_arg_out_fn = is_null,

	.clean_up_fn = destroy_nothing,
	.ctx = &CTX_TU_INCOMPLETE,
};

static test_parser_api_ctx CTX_TU_BINDING_FUNCTION_CALL = {0};
static const test_parser_api_case TU_BINDING_FUNCTION_CALL = {
	.description = "parse_translation_unit successfully parse a binding and a function call",

	.goal = PARSE_GOAL_TU,
	.lexer_input = "\
a = 1 ; \
f(2) ; \
",

	.expected_returned_value = PARSE_STATUS_OK,
	.expected_arg_out_fn = a_tu_with_binding_function_call,

	.clean_up_fn = destroy_result_ast,
	.ctx = &CTX_TU_BINDING_FUNCTION_CALL,
};

static test_parser_api_ctx CTX_READABLE_SYNTAX_ERROR = {0};
static const test_parser_api_case READABLE_SYNTAX_ERROR = {
	.description = "parse_readable handle gracefully a syntax error",

	.goal = PARSE_GOAL_READABLE,
	.lexer_input = "\
a=1; \
",
	.expected_returned_value = PARSE_STATUS_ERROR,
	.expected_arg_out_fn = is_null,

	.clean_up_fn = destroy_nothing,
	.ctx = &CTX_READABLE_SYNTAX_ERROR,
};

static test_parser_api_ctx CTX_READABLE_NUMBER = {0};
static const test_parser_api_case READABLE_NUMBER = {
	.description = "parse_readable successfully parse a number",

	.goal = PARSE_GOAL_READABLE,
	.lexer_input = "\
7 \
",

	.expected_returned_value = PARSE_STATUS_OK,
	.expected_arg_out_fn = a_number,

	.clean_up_fn = destroy_result_ast,
	.ctx = &CTX_READABLE_NUMBER,
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

	.expected_returned_value = PARSE_STATUS_OK,
	.expected_arg_out_fn = a_block,

	.clean_up_fn = destroy_result_ast,
	.ctx = &CTX_READABLE_BLOCK,
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
	X(STATEMENT_INCOMPLETE) \
    X(STATEMENT_BINDING) \
    X(STATEMENT_FUNCTION_DEFINITION) \
    X(TU_SYNTAX_ERROR) \
	X(TU_INCOMPLETE) \
    X(TU_BINDING_FUNCTION_CALL) \
	X(READABLE_SYNTAX_ERROR) \
    X(READABLE_NUMBER) \
    X(READABLE_BLOCK)

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


static void test_parse_one_statement_three_calls(void **state) {
	(void) state;

	// ARRANGE

	const char *src = "a=1;b=2;";

	// scanner initialization
	yyscan_t scanner = NULL;
	assert_int_equal(0, yylex_init(&scanner));

	// input_provider_legacy initialization
	input_provider *provider = input_provider_legacy_create();
	input_provider_legacy_set_mode_chunks(provider);

	// bind input_provider to scanner
	assert_true(input_provider_legacy_bind_to_scanner(provider, scanner));

	// prepare lexer input
	input_provider_legacy_append_string_as_line(provider, src);

	struct parser_cfg ctx = get_parser_cfg_one_statement();
	struct ast *node = NULL;


	// ACT

	parse_status ret = parse_one_statement(scanner, &node, &ctx, provider);


	// ASSERT

	assert_int_equal(ret, PARSE_STATUS_OK);
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

	ret = parse_one_statement(scanner, &node, &ctx, provider);


	// ASSERT

	assert_int_equal(ret, PARSE_STATUS_OK);
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

	ret = parse_one_statement(scanner, &node, &ctx, provider);


	// ASSERT

	assert_int_equal(ret, PARSE_STATUS_EOF);
	assert_null(node);

	// TEST INFRASTRUCTURE CLEANUP

	assert_int_equal(0, yylex_destroy(scanner));
	input_provider_legacy_destroy(provider);
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
	assert_int_equal(children[2]->children->children[0]->children->children[0]->children->children[0]->type,
	                 AST_TYPE_DATA_WRAPPER);
	data = children[2]->children->children[0]->children->children[0]->children->children[0]->data;
	assert_non_null(data);
	assert_int_equal(data->type, TYPE_SYMBOL_NAME);
	string_value = data->data.string_value;
	assert_non_null(string_value);
	assert_string_equal(string_value, "z");
	assert_non_null(children[2]->children->children[0]->children->children[0]->children->children[1]);
	assert_int_equal(children[2]->children->children[0]->children->children[0]->children->children[1]->type,
	                 AST_TYPE_DATA_WRAPPER);
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
	assert_int_equal(children[1]->type, AST_TYPE_LIST_OF_ARGUMENTS);
	children_info = children[1]->children;
	assert_non_null(children_info);
	assert_int_equal(children_info->children_nb, 1);
	children = children_info->children;
	assert_non_null(children);
	assert_non_null(children[0]);
	assert_int_equal(children[0]->type, AST_TYPE_ARGUMENTS);
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
	assert_int_equal(children[1]->type, AST_TYPE_LIST_OF_ARGUMENTS);
	children_info = children[1]->children;
	assert_non_null(children_info);
	assert_int_equal(children_info->children_nb, 1);
	children = children_info->children;
	assert_non_null(children);
	assert_non_null(children[0]);
	assert_int_equal(children[0]->type, AST_TYPE_ARGUMENTS);
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