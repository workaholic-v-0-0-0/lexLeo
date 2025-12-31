// src/lexer/include/lexer.h

#ifndef LEXLEO_LEXER_H
#define LEXLEO_LEXER_H

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void *yyscan_t;
#endif

int yylex_init(yyscan_t *scanner);
int yylex_destroy(yyscan_t scanner);
void yyset_extra(void *user_defined, yyscan_t scanner);

#endif //LEXLEO_LEXER_H
