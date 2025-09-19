%option reentrant
%option bison-bridge
%option header-file="lexer.yy.h"

%{
#include "osal.h"
#include "parser.tab.h"
#include "parser_types.h"
%}

%option nounput
%option noinput
%option noyywrap

%%

"{"                 { return LBRACE; }
"}"                 { return RBRACE; }
"("                 { return LPARENTHESES; }
")"                 { return RPARENTHESES; }

";"           { return SEMICOLON; }
"="           { return EQUAL; }

"read"           { return READ; }
"write"             { return WRITE; }
"define"            { return DEFINE; }
"call"              { return CALL; }

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

. {
    yyerror(NULL, NULL, NULL, "unknown characters");
}

%%
