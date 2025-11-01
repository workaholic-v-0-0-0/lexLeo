// src/parser/tests/mock_lexer/include/mock_lexer.h

#ifndef LEXLEO_MOCK_LEXER_H
#define LEXLEO_MOCK_LEXER_H


#ifndef UNIT_PARSER_HEADER
# error "UNIT_PARSER_HEADER must be defined to the unit parser .tab.h"
#endif

#include UNIT_PARSER_HEADER

typedef void (*mock_dup_fn)(STYPE *dst, const STYPE *src, int tok);

typedef struct {
    int tok;
    STYPE yv;
} mock_token;

void mock_lex_set(const mock_token *seq, size_t len);
void mock_lex_reset();
void mock_lex_set_dupper(mock_dup_fn fn);

int yylex(STYPE *yylval, void *yyscanner /* yyscan_t */);

#endif //LEXLEO_MOCK_LEXER_H