%option reentrant
%option bison-bridge
%option header-file="lexer_legacy.yy.h"

%{
#include "osal.h"
#include "parser.tab.h"
#include "parser_types.h"
#include "input_provider_legacy_reader.h"

#define YY_DECL int lex_lex(YYSTYPE *yylval_param, yyscan_t yyscanner)

#define LEX_EXTRA  ((lexer_extra_t*) yyget_extra(yyscanner))
#define GOAL       (LEX_EXTRA->goal)

#define YY_INPUT(buf, result, max_size)                                      \
do {                                                                         \
    input_provider *prov = LEX_EXTRA ? LEX_EXTRA->provider : NULL;     \
    if (!prov) {                                                             \
        (result) = 0;                                                        \
        break;                                                               \
    }                                                                        \
    (result) = (int)input_provider_legacy_read(prov, buf, (size_t)max_size);        \
} while (0)

/* Prototypes de la vraie func et du wrapper */
//int lex_lex(YYSTYPE *yylval_param, yyscan_t scanner);       /* générée par Flex */
//int yylex(YYSTYPE *yylval_param, yyscan_t scanner);         /* wrapper exposé à Bison */
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
    yylval->int_value = atoi(yytext);
    return INTEGER;
}

\"([^\\\"]|\\.)*\" { // string
    size_t len = yyleng - 2;
    char *s = malloc(len + 1);
    if (!s) { // parser must handle this error
        yylval->string_value = NULL;
        return STRING;
    }
    memcpy(s, yytext + 1, len);
    s[len] = '\0';

    yylval->string_value = s;
    return STRING;
}

[a-zA-Z_][a-zA-Z0-9_]* { // symbol name
    char *s = osal_strdup(yytext);
    if (!s) { // parser must handle this error
        yylval->string_value = NULL;
        return SYMBOL_NAME;
    }
    yylval->string_value = s;
    return SYMBOL_NAME;
}

[ \t\n]+ {
    // ignore white spaces
}

. { return INVALID; }

%%
int yylex(YYSTYPE *yylval_param, yyscan_t yyscanner)
{
    lexer_extra_t *ex = LEX_EXTRA;
    if (ex && !ex->sent_mode_token) {
        ex->sent_mode_token = 1;
        switch (ex->goal) {
            case PARSE_GOAL_TU:
                return TU_MODE;
            case PARSE_GOAL_ONE_STATEMENT:
                return ONE_STATEMENT_MODE;
            case PARSE_GOAL_READABLE:
                return READABLE_MODE;
            default:
                break;
        }
    }
    int token = lex_lex(yylval_param, yyscanner);
    if (token == 0 && ex) {
        ex->reached_input_end = 1;
    }
    return token;
}
