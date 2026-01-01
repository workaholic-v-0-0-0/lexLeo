%option reentrant
%option extra-type="lexer_scanner_extra_t *"
%option bison-bridge
%option header-file="lexer.yy.h"

%{
#include "parser.tab.h"
#include "input_provider_reader.h"
#include "osal.h"

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define YY_DECL int lex_lex(YYSTYPE *yylval_param, yyscan_t yyscanner)

#define YY_INPUT(buf, result, max_size)                                       \
do {                                                                          \
    lexer_scanner_extra_t *ex = yyget_extra(yyscanner);                       \
    struct input_provider *prov = ex ? ex->provider : NULL;                   \
    if (!prov) {                                                              \
        (result) = 0;                                                         \
        break;                                                                \
    }                                                                         \
    (result) = (int) input_provider_read(prov, buf, (size_t) max_size);       \
} while (0)

%}

%option nounput
%option noinput
%option noyywrap

%%

"{"                 { return LBRACE; }
"}"                 { return RBRACE; }
"("                 { return LPAREN; }
")"                 { return RPAREN; }

"+"                 { return ADD; }
"-"                 { return SUBTRACT; }
"*"                 { return MULTIPLY; }
"/"                 { return DIVIDE; }

";"                 { return SEMICOLON; }
","                 { return COMMA; }
"="                 { return EQUAL; }

"read"              { return READ; }
"write"             { return WRITE; }
"define"            { return DEFINE; }
"call"              { return CALL; }
"quote"             { return QUOTE; }
"eval"              { return EVAL; }
"symbol"            { return SYMBOL; }
"set"               { return SET; }
"if"                { return IF; }
"then"              { return THEN; }
"else"              { return ELSE; }
"while"             { return WHILE; }
"do"                { return DO; }

[0-9]+ { // number
    yylval_param->int_value = atoi(yytext);
    return INTEGER;
}

\"([^\\\"]|\\.)*\" { // string
    size_t len = (size_t)yyleng;
    if (len < 2) {
        yylval_param->string_value = NULL;
        return STRING;
    }

    char *s = malloc(len - 1); // len-2 + 1
    if (!s) {
        yylval_param->string_value = NULL;
        return STRING;
    }

    memcpy(s, yytext + 1, len - 2);
    s[len - 2] = '\0';

    yylval_param->string_value = s;
    return STRING;
}

[a-zA-Z_][a-zA-Z0-9_]* { // symbol name
    char *s = osal_strdup(yytext);
    if (!s) { // parser must handle this error
        yylval_param->string_value = NULL;
        return SYMBOL_NAME;
    }
    yylval_param->string_value = s;
    return SYMBOL_NAME;
}

[ \t\n]+ {
    // ignore white spaces
}

. { return INVALID; }

%%

int yylex(YYSTYPE *yylval_param, yyscan_t yyscanner)
{
    lexer_scanner_extra_t *ex = yyget_extra(yyscanner);

    if (!ex || !ex->state) {
        return 0;
    }

    lexer_state_t *state = ex->state;

    if (state->first_token_is_pending) {
        state->first_token_is_pending = false;
        return state->first_token;
    }

    int token = lex_lex(yylval_param, yyscanner);
    if (token == 0) {
        state->reached_input_end = true;
    }

    return token;
}
