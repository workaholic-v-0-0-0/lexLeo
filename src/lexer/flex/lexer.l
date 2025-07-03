%option reentrant
%option bison-bridge

%{
#include "parser.tab.h"
%}

%option nounput
%option noinput

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
    yyerror(NULL, "unknown characters");
}

%%

int yyerror(yyscan_t scanner, const char *s) {
    return 1;
}
