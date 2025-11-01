// // src/parser/include/parser_api.h

#ifndef LEXLEO_PARSER_API_H
#define LEXLEO_PARSER_API_H

parser_ctx *get_g_parser_ctx_default_one_statement(void);
parser_ctx *get_g_parser_ctx_default_translation_unit(void);
parser_ctx *get_g_parser_ctx_default_readable(void);

bool parse_translation_unit(yyscan_t scanner, ast **out, struct parser_ctx *ctx);
bool parse_one_statement(yyscan_t scanner, ast **out, struct parser_ctx *ctx);
bool parse_readable (yyscan_t scanner, ast **out, struct parser_ctx *ctx);

#endif //LEXLEO_PARSER_API_H
