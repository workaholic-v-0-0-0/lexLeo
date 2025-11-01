// src/parser/tests/mock_lexer/src/mock_lexer.c

#include <stddef.h>

#ifndef STYPE
# error "STYPE must be defined"
#endif
#ifndef UNIT_PARSER_HEADER
# error "UNIT_PARSER_HEADER must be defined"
#endif

#include "mock_lexer.h"
#include UNIT_PARSER_HEADER
#include <string.h>

static const mock_token *g_seq = NULL;
static size_t g_len = 0;
static size_t g_idx = 0;
static mock_dup_fn g_dup = NULL;

void mock_lex_set(const mock_token *seq, size_t len) {
    g_seq = seq;
    g_len = len;
    g_idx = 0;
}

void mock_lex_reset() {
    g_seq = NULL;
    g_len = 0;
    g_idx = 0;
    g_dup = NULL;
}

void mock_lex_set_dupper(mock_dup_fn fn) {
    g_dup = fn;
}

int yylex(STYPE *yylval, void *yyscanner /* yyscan_t */) {
    (void)yyscanner;
    if (!g_seq || g_idx >= g_len) return 0;

    const int tok = g_seq[g_idx].tok;
    if (tok != 0 && yylval) {
        if (g_dup) {
            g_dup(yylval, &g_seq[g_idx].yv, tok);  // test-provided deep copy
        } else {
            *yylval = g_seq[g_idx].yv;             // shallow as a fallback
        }
    }
    //int tok = g_seq[g_idx].tok;
    //if (tok != 0 && yylval) *yylval = g_seq[g_idx].yv;
    /*
    if (tok != 0 && yylval) {
        // Par défaut, copie "brute" de la valeur simulée
        *yylval = g_seq[g_idx].yv;

        // Mais pour les tokens "à chaîne", on duplique sur le tas :
        switch (tok) {
            case STRING:
                // yv.string_value vient de la séquence fournie par le test (littéral possible),
                // on renvoie une copie heap que Bison pourra free via %destructor.
                yylval->string_value = strdup(g_seq[g_idx].yv.string_value);
                break;
            case SYMBOL_NAME:
                yylval->symbol_name_value = strdup(g_seq[g_idx].yv.symbol_name_value);
                break;
            default:
                // rien de spécial pour INTEGER, etc.
                break;
        }
    }
    */
    g_idx++;
    return tok;
}
