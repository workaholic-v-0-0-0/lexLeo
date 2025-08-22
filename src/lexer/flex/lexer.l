%option reentrant
%option bison-bridge
%option header-file="lexer.yy.h"

%{
#include "parser.tab.h"
#include "parser_types.h"
%}

%option nounput
%option noinput

%%

[0-9]+ {
    yylval->int_value = atoi(yytext);
    return INTEGER;
}

[ \t\n]+ {
    // ignore white spaces
}

. {
    yyerror(NULL, NULL, "unknown characters");
}

%%
