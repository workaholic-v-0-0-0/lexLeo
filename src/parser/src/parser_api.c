// src/parser/src/parser_api.c

#include "parser_api.h"

#include "parser.tab.h"

static const parser_ops PARSER_OPS_DEFAULT = {
    .create_int_node = ast_create_int_node,
    .create_string_node = ast_create_string_node,
    .create_symbol_name_node = ast_create_symbol_name_node,
    .create_error_node_or_sentinel = ast_create_error_node_or_sentinel,
    .create_children_node_var = ast_create_children_node_var,
    .destroy = ast_destroy,
    .children_append_take = ast_children_append_take,
};

static parser_ctx g_parser_ctx_default_one_statement = {
    .ops = PARSER_OPS_DEFAULT,
    .goal = PARSE_GOAL_ONE_STATEMENT,
};

static parser_ctx g_parser_ctx_default_translation_unit = {
    .ops = PARSER_OPS_DEFAULT,
    .goal = PARSE_GOAL_TU,
};

static parser_ctx g_parser_ctx_default_readable = {
    .ops = PARSER_OPS_DEFAULT,
    .goal = PARSE_GOAL_READABLE,
};

parser_ctx *get_g_parser_ctx_default_one_statement() {
    return &g_parser_ctx_default_one_statement;
}

parser_ctx *get_g_parser_ctx_default_translation_unit() {
    return &g_parser_ctx_default_translation_unit;
}

parser_ctx *get_g_parser_ctx_default_readable() {
    return &g_parser_ctx_default_readable;
}

// forward declaration
void yyset_extra(struct lexer_extra *extra, yyscan_t scanner);

static parse_status map_rc_to_status(
		int rc,
		const ast *out,
		const lexer_extra_t *ex,
		struct parser_ctx *ctx ) {
    if (!ctx || ctx->syntax_errors != 0)
        return PARSE_STATUS_ERROR;
    if (rc != 0) {
        if (ex && ex->reached_input_end)
            return PARSE_STATUS_INCOMPLETE;
        return PARSE_STATUS_ERROR;
    }
    return (out == NULL) ? PARSE_STATUS_EOF : PARSE_STATUS_OK;
}

parse_status
parse_translation_unit(
        yyscan_t scanner,
        ast **out,
        struct parser_ctx *ctx ) {
    if (!out || !ctx)
        return PARSE_STATUS_ERROR;
    *out = NULL;
    lexer_extra_t ex = {
		.goal = PARSE_GOAL_TU,
		.sent_mode_token = 0,
		.reached_input_end = 0 };
    yyset_extra(&ex, scanner);
    ctx->goal = PARSE_GOAL_TU;
	ctx->syntax_errors = 0;
    int rc = yyparse(scanner, out, ctx);
    return map_rc_to_status(rc, *out, &ex, ctx);
}

parse_status
parse_one_statement(
        yyscan_t scanner,
        ast **out,
        struct parser_ctx *ctx) {
    if (!out || !ctx)
        return PARSE_STATUS_ERROR;
    *out = NULL;
    lexer_extra_t ex = {
		.goal = PARSE_GOAL_ONE_STATEMENT,
		.sent_mode_token = 0,
		.reached_input_end = 0 };
    yyset_extra(&ex, scanner);
    ctx->goal = PARSE_GOAL_ONE_STATEMENT;
	ctx->syntax_errors = 0;
    int rc = yyparse(scanner, out, ctx);
    return map_rc_to_status(rc, *out, &ex, ctx);
}

parse_status
parse_readable(
        yyscan_t scanner,
        ast **out,
        struct parser_ctx *ctx) {
    if (!out || !ctx)
        return PARSE_STATUS_ERROR;
    *out = NULL;
    lexer_extra_t ex = {
		.goal = PARSE_GOAL_READABLE,
		.sent_mode_token = 0,
		.reached_input_end = 0 };
    yyset_extra(&ex, scanner);
    ctx->goal = PARSE_GOAL_READABLE;
	ctx->syntax_errors = 0;
    int rc = yyparse(scanner, out, ctx);
    return map_rc_to_status(rc, *out, &ex, ctx);
}
