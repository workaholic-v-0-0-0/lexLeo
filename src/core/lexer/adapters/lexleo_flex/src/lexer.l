/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/core/lexer/adapters/lexleo_flex/src/
 * lexer.l
 *
 * Flex lexer rules for lexLeo.
 *
 * This scanner is reentrant and communicates tokens via yyextra.
 * Tokens may be buffered and returned later via the pending mechanism.
 * Memory allocation is delegated to OSAL memory operations.
 */

%option reentrant
%option extra-type="lexleo_flex_scanner_extra_t *"
%option prefix="lexleo_flex_"

%option nounput
%option noinput
%option noyywrap

%{
#include "lexleo_assert.h"
#include "lexleo_panic.h"
#include "lexleo_token.h"
#include "lexleo_flex_state.h"

#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <limits.h>

#define YY_INPUT(buf, result, max_size)                                       \
do {                                                                          \
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner);                 \
    LEXLEO_ASSERT(ex && ex->provider && ex->read);                            \
    (result) = (int)ex->read(ex->provider, (buf), (size_t)max_size);          \
} while (0)

static void set_kind(lexleo_flex_scanner_extra_t *ex, lexleo_token_kind_t k) {
    ex->pending.kind = k;
    ex->pending.as.int_value = 0;
    ex->pending.as.string_value = NULL;
    ex->pending.as.symbol_value = NULL;
    ex->has_pending = 1;
}

static void set_int(lexleo_flex_scanner_extra_t *ex, int i) {
    ex->pending.kind = LEX_TK_INTEGER;
    ex->pending.as.int_value = i;
    ex->pending.as.string_value = NULL;
    ex->pending.as.symbol_value = NULL;
    ex->has_pending = 1;
}

static void set_string_owned(
        lexleo_flex_scanner_extra_t *ex,
        lexleo_token_kind_t k,
        char *s ) {
    set_kind(ex, k);
    ex->pending.as.string_value = s;
    ex->has_pending = 1;
}

#define TOK(K) do {                                           \
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner); \
    set_kind(ex, (K));                                        \
    return 1;                                                 \
} while (0)

#define TOK_INT(I) do {                                       \
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner); \
    set_int(ex, (I));                                         \
    return 1;                                                 \
} while (0)

#define TOK_STR_OWNED(K, S) do {                                    \
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner); \
    set_string_owned(ex, (K), (S));                                 \
    return 1;                                                 \
} while (0)

%}

%%

"{"        { TOK(LEX_TK_LBRACE); }
"}"        { TOK(LEX_TK_RBRACE); }
"("        { TOK(LEX_TK_LPAREN); }
")"        { TOK(LEX_TK_RPAREN); }

"+"        { TOK(LEX_TK_ADD); }
"-"        { TOK(LEX_TK_SUBTRACT); }
"*"        { TOK(LEX_TK_MULTIPLY); }
"/"        { TOK(LEX_TK_DIVIDE); }

";"        { TOK(LEX_TK_SEMICOLON); }
","        { TOK(LEX_TK_COMMA); }
"="        { TOK(LEX_TK_EQUAL); }

"read"     { TOK(LEX_TK_READ); }
"write"    { TOK(LEX_TK_WRITE); }
"define"   { TOK(LEX_TK_DEFINE); }
"call"     { TOK(LEX_TK_CALL); }
"quote"    { TOK(LEX_TK_QUOTE); }
"eval"     { TOK(LEX_TK_EVAL); }
"symbol"   { TOK(LEX_TK_SYMBOL); }
"set"      { TOK(LEX_TK_SET); }
"if"       { TOK(LEX_TK_IF); }
"then"     { TOK(LEX_TK_THEN); }
"else"     { TOK(LEX_TK_ELSE); }
"while"    { TOK(LEX_TK_WHILE); }
"do"       { TOK(LEX_TK_DO); }

[0-9]+ {
    /* Integer literal parsing.
     *
     * The matched text is converted using strtoll and validated
     * to ensure it fits into a signed int. Invalid or out-of-range
     * values produce an INVALID token.
     */
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner);
    errno = 0;
    char *end = NULL;
    long long v = strtoll(yytext, &end, 10);
    if (
               errno != 0
            || end == yytext
            || *end != '\0'
            || v < INT_MIN
            || v > INT_MAX ) {
        TOK(LEX_TK_INVALID);
    }
    TOK_INT((int)v);
}

\"([^\\\"]|\\.)*\" {
    /* String literal parsing.
     *
     * The surrounding quotes are stripped and the payload is copied
     * into newly allocated memory using OSAL memory operations.
     * Ownership of the resulting string is transferred to the token.
     */
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner);
    LEXLEO_ASSERT(ex && ex->mem && ex->mem->malloc && ex->mem->memcpy);
    size_t len = (size_t)yyleng;
    LEXLEO_ASSERT(len >= 2);
    size_t payload_len = len - 2;
    char *s = ex->mem->malloc(payload_len + 1);
    if (!s) lexleo_panic_oom();
    ex->mem->memcpy(s, yytext + 1, payload_len);
    s[payload_len] = '\0';
    TOK_STR_OWNED(LEX_TK_STRING, s);
}

[a-zA-Z_][a-zA-Z0-9_]* {
    /* Identifier / symbol name.
     *
     * The matched identifier is copied into newly allocated memory.
     * The token takes ownership of the allocated string.
     */
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner);
    LEXLEO_ASSERT(ex && ex->mem && ex->mem->malloc && ex->mem->memcpy);
    char *s = ex->mem->malloc((size_t)yyleng + 1);
    if (!s) lexleo_panic_oom();
    ex->mem->memcpy(s, yytext, (size_t)yyleng);
    s[(size_t)yyleng] = '\0';
    TOK_STR_OWNED(LEX_TK_SYMBOL_NAME, s);
}

[ \t\n]+ { /* ignore */ }

<<EOF>>     { TOK(LEX_TK_EOF); }
.           { TOK(LEX_TK_INVALID); }

%%
