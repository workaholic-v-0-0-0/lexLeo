%option reentrant
%option bison-bridge
%option header-file="lexer.yy.h"

%{
#include "parser.tab.h"
#include "parser_types.h"
%}

%option nounput
%option noinput
%option noyywrap

%%

[0-9]+ {
    yylval->int_value = atoi(yytext);
    return INTEGER;
}

\"([^\\\"]|\\.)*\" {
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

[ \t\n]+ {
    // ignore white spaces
}

. {
    yyerror(NULL, NULL, NULL, "unknown characters");
}

%%
