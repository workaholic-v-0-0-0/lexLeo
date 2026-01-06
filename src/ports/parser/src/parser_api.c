// src/ports/parser/src/parser_api.c

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

static const parser_cfg CFG_ONE_STATEMENT = {
    .ops = PARSER_OPS_DEFAULT,
    .goal = PARSE_GOAL_ONE_STATEMENT,
};

static const parser_cfg CFG_TRANSLATION_UNIT = {
    .ops = PARSER_OPS_DEFAULT,
    .goal = PARSE_GOAL_TU,
};

static const parser_cfg CFG_READABLE = {
    .ops = PARSER_OPS_DEFAULT,
    .goal = PARSE_GOAL_READABLE,
};

parser_cfg get_parser_cfg_one_statement() {
    return CFG_ONE_STATEMENT;
}

parser_cfg get_parser_cfg_translation_unit() {
    return CFG_TRANSLATION_UNIT;
}

parser_cfg get_parser_cfg_readable() {
    return CFG_READABLE;
}

// forward declaration
void yyset_extra(struct lexer_extra *extra, yyscan_t scanner);

static parse_status map_rc_to_status(
		int rc,
		const ast *out,
		const lexer_extra_t *ex,
		struct parser_cfg *ctx ) {
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
        struct parser_cfg *ctx,
        input_provider *provider ) {
    if (!out || !ctx)
        return PARSE_STATUS_ERROR;
    *out = NULL;
    lexer_extra_t ex = {
		.goal = PARSE_GOAL_TU,
		.sent_mode_token = 0,
		.reached_input_end = 0,
        .provider = provider };
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
        struct parser_cfg *ctx,
        input_provider *provider ) {
    if (!out || !ctx)
        return PARSE_STATUS_ERROR;
    *out = NULL;
    lexer_extra_t ex = {
		.goal = PARSE_GOAL_ONE_STATEMENT,
		.sent_mode_token = 0,
        .reached_input_end = 0,
        .provider = provider };
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
        struct parser_cfg *ctx,
        input_provider *provider ) {
    if (!out || !ctx)
        return PARSE_STATUS_ERROR;
    *out = NULL;
    lexer_extra_t ex = {
		.goal = PARSE_GOAL_READABLE,
		.sent_mode_token = 0,
        .reached_input_end = 0,
        .provider = provider };
    yyset_extra(&ex, scanner);
    ctx->goal = PARSE_GOAL_READABLE;
	ctx->syntax_errors = 0;
    int rc = yyparse(scanner, out, ctx);
    return map_rc_to_status(rc, *out, &ex, ctx);
}
