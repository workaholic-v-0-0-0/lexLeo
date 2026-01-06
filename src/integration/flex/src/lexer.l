%option reentrant
%option extra-type="lexleo_flex_scanner_extra_t *"
%option header-file="lexer.yy.h"
%option prefix="lexleo_flex_"

%option nounput
%option noinput
%option noyywrap

%{
#include "lexleo_token.h"
#include "lexleo_flex_scanner_extra.h"
#include "input_provider_reader.h"
#include "osal_mem.h"

#define YY_INPUT(buf, result, max_size)                                       \
do {                                                                          \
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner);                       \
    struct input_provider *prov = ex ? ex->provider : NULL;                   \
    if (!prov) {                                                              \
        (result) = 0;                                                         \
        break;                                                                \
    }                                                                         \
    (result) = (int) input_provider_read(prov, buf, (size_t) max_size);       \
} while (0)

static void set_kind(lexleo_flex_scanner_extra_t *ex, lexleo_token_kind_t k) {
    ex->pending.kind = k;
    ex->pending.as.string_value = NULL;
    ex->has_pending = 1;
}

static void set_int(lexleo_flex_scanner_extra_t *ex, long long v) {
    ex->pending.kind = LEX_TK_INTEGER;
    ex->pending.as.int_value = (int64_t)v;
    ex->has_pending = 1;
}

static void set_string_dup(lexleo_flex_scanner_extra_t *ex, lexleo_token_kind_t k, const char *s) {
    ex->pending.kind = k;
    ex->pending.as.string_value = osal_strdup(s);
    ex->has_pending = 1;
}

#define TOK(K) do { \
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner); \
    set_kind(ex, (K)); \
    return 1; \
} while (0)

#define TOK_INT(V) do { \
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner); \
    set_int(ex, (V)); \
    return 1; \
} while (0)

#define TOK_STR(K, S) do { \
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner); \
    set_string_dup(ex, (K), (S)); \
    return 1; \
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
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner);
    set_int(ex, atoll(yytext));
    return 1;
}

\"([^\\\"]|\\.)*\" {
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner);

    size_t len = (size_t)yyleng;
    if (len < 2) {
        // string vide/erreur => kind STRING, value NULL (ton parser décidera)
        ex->pending.kind = LEX_TK_STRING;
        ex->pending.as.string_value = NULL;
        ex->has_pending = 1;
        return 1;
    }

    char *s = osal_malloc(len - 1); // len-2 + 1
    if (!s) {
        ex->pending.kind = LEX_TK_STRING;
        ex->pending.as.string_value = NULL;
        ex->has_pending = 1;
        return 1;
    }

    memcpy(s, yytext + 1, len - 2);
    s[len - 2] = '\0';

    ex->pending.kind = LEX_TK_STRING;
    ex->pending.as.string_value = s; // ownership transféré au consumer
    ex->has_pending = 1;
    return 1;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner);
    set_string_dup(ex, LEX_TK_SYMBOL_NAME, yytext);
    return 1;
}

[ \t\n]+ { /* ignore */ }

<<EOF>> {
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner);
    set_kind(ex, LEX_TK_EOF);
    return 1;
}

. {
    lexleo_flex_scanner_extra_t *ex = yyget_extra(yyscanner);
    set_kind(ex, LEX_TK_INVALID);
    return 1;
}

%%
