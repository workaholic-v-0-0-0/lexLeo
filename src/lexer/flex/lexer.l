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

"="           { return EQUAL; }
"evaluate"    { return EVALUATE; }
"execute"     { return EXECUTE; }
"compute"     { return COMPUTE; }
"read"        { return READ; }
"write"       { return WRITE; }

";"           { return SEMICOLON; }
"("           { return LEFT_PARENTHESIS; }
")"           { return RIGHT_PARENTHESIS; }

"+"           { return PLUS; }
"-"           { return MINUS; }
"*"           { return MULTIPLY; }
"/"           { return DIVIDE; }


[0-9]+ {
    yylval->int_value = atoi(yytext);
    printf("here2\n");
    return INTEGER;
}

\"([^\\\"]|\\.)*\" {
    char *s = strdup(yytext + 1);
    s[strlen(s) - 1] = '\0';
    yylval->string_value = s;
    return STRING;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval->symbol_name_value = strdup(yytext);
    return SYMBOL;
}

[ \t\n]+ {
    // ignore white spaces
}

. {
    yyerror(NULL, NULL, "unknown characters");
}

