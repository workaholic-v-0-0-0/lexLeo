// src/parser/include/parser_api.h

#ifndef LEXLEO_PARSER_API_H
#define LEXLEO_PARSER_API_H

#include "parser_types.h"
#include "parser_ctx.h"
#include "ast.h"

parser_ctx *get_g_parser_ctx_default_one_statement(void);
parser_ctx *get_g_parser_ctx_default_translation_unit(void);
parser_ctx *get_g_parser_ctx_default_readable(void);

parse_status parse_translation_unit(
	yyscan_t scanner,
	ast **out,
	struct parser_ctx *ctx,
	input_provider *provider );

parse_status parse_one_statement(
	yyscan_t scanner,
	ast **out,
	struct parser_ctx *ctx,
	input_provider *provider );

parse_status parse_readable(
	yyscan_t scanner,
	ast **out,
	struct parser_ctx *ctx,
	input_provider *provider );

#endif //LEXLEO_PARSER_API_H
