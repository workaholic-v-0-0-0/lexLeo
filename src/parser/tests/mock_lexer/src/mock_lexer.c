// src/parser/tests/mock_lexer/src/mock_lexer.c

#include <stddef.h>

#ifndef STYPE
# error "STYPE must be defined"
#endif
#ifndef UNIT_PARSER_HEADER
# error "UNIT_PARSER_HEADER must be defined"
#endif

#include UNIT_PARSER_HEADER
#include "mock_lexer.h"

static const mock_token *g_seq = NULL;
static size_t g_len = 0;
static size_t g_idx = 0;

void mock_lex_set(const mock_token *seq, size_t len) {
    g_seq = seq;
    g_len = len;
    g_idx = 0;
}

void mock_lex_reset() {
    g_seq = NULL;
    g_len = 0;
    g_idx = 0;
}

int yylex(STYPE *yylval, void *yyscanner /* yyscan_t */) {
    (void)yyscanner;
    if (!g_seq || g_idx >= g_len) return 0;
    int tok = g_seq[g_idx].tok;
    if (tok != 0 && yylval) *yylval = g_seq[g_idx].yv;
    g_idx++;
    return tok;
}
