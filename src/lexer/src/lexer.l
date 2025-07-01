# src/lexer/lexer.l

%{
#include "parser.tab.h"

extern void yyerror(const char *s);
%}

%option nounput
%option noinput

%%

[0-9]+        {
                    yylval.integer = atoi(yytext);
                    return NUMBER;
                }

[a-zA-Z_][a-zA-Z0-9_]*    {
                    yylval.string = strdup(yytext);
                    return IDENTIFIER;
                }

\"[^"]*\"     {
                    yylval.string = strdup(yytext);
                    return CODE;
                }

"="           { return EQUAL; }
"evaluate"    { return EVALUATE; }
"execute"     { return EXECUTE; }
"compute"     { return COMPUTE; }
"read"        { return READ; }
"write"       { return WRITE; }

";"           { return SEMICOLON; }
"("           { return LEFT_PARENTHESE; }
")"           { return RIGHT_PARENTHESE; }

"+"           { return PLUS; }
"-"           { return MINUS; }
"*"           { return MULTIPLY; }
"/"           { return DIVIDE; }

[ \t\n]+      { /* Ignore white spaaces */ }
.             { /* Unknown characters */ yyerror("Caractère inconnu."); }

%%

int yywrap() {
    return 1;
}
