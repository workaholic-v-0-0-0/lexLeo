// src/ports/parser/include/parser_api.h

#ifndef LEXLEO_PARSER_API_H
#define LEXLEO_PARSER_API_H

#include "policy/parser_types.h"
#include "parser_cfg.h"
#include "ast.h"

parser_cfg get_parser_cfg_one_statement(void);
parser_cfg get_parser_cfg_translation_unit(void);
parser_cfg get_parser_cfg_readable(void);

parse_status parse_translation_unit(
	yyscan_t scanner,
	ast **out,
	struct parser_cfg *ctx,
	input_provider *provider );

parse_status parse_one_statement(
	yyscan_t scanner,
	ast **out,
	struct parser_cfg *ctx,
	input_provider *provider );

parse_status parse_readable(
	yyscan_t scanner,
	ast **out,
	struct parser_cfg *ctx,
	input_provider *provider );

#endif //LEXLEO_PARSER_API_H
