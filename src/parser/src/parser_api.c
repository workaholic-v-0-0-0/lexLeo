// src/parser/src/parser_api.c

#include <stdbool.h>
#include "parser_ctx.h"
#include "ast.h"
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

bool parse_translation_unit(
        yyscan_t scanner,
        ast **out,
        struct parser_ctx *ctx ) {
    if (!out || !ctx)
        return false;
    *out = NULL;
    lexer_extra_t ex = { .goal = PARSE_GOAL_TU, .sent_mode_token = 0 };
    yyset_extra(&ex, scanner);
    ctx->goal = PARSE_GOAL_TU;
    int rc = yyparse(scanner, out, ctx);
    return (rc == 0) && (*out != NULL);
}

bool parse_one_statement(
        yyscan_t scanner,
        ast **out,
        struct parser_ctx *ctx) {
    if (!out || !ctx)
        return false;
    *out = NULL;
    lexer_extra_t ex = { .goal = PARSE_GOAL_ONE_STATEMENT, .sent_mode_token = 0 };
    yyset_extra(&ex, scanner);
    ctx->goal = PARSE_GOAL_ONE_STATEMENT;
    int rc = yyparse(scanner, out, ctx);
    return (rc == 0) && (*out != NULL);
}

bool parse_readable(
        yyscan_t scanner,
        ast **out,
        struct parser_ctx *ctx) {
    if (!out || !ctx)
        return false;
    *out = NULL;
    lexer_extra_t ex = { .goal = PARSE_GOAL_READABLE, .sent_mode_token = 0 };
    yyset_extra(&ex, scanner);
    ctx->goal = PARSE_GOAL_READABLE;
    int rc = yyparse(scanner, out, ctx);
    return (rc == 0) && (*out != NULL);
}
