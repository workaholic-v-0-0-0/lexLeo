// src/parser/tests/mock_lexer/include/mock_lexer.h

#ifndef LEXLEO_MOCK_LEXER_H
#define LEXLEO_MOCK_LEXER_H

typedef struct {
    int tok;
    STYPE yv;
} mock_token;

void mock_lex_set(const mock_token *seq, size_t len);
void mock_lex_reset();
int yylex(STYPE *yylval, void *yyscanner /* yyscan_t */);

#endif //LEXLEO_MOCK_LEXER_H