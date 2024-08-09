/*******************************************************************************
# Name: lexer.l
# Role:
# Author: Sylvain Labopin
# Version: 0.0.0
# License: GPLv3
# Compilation:
#
# Usage:
#
*******************************************************************************/

%{
#include "parser/parser.tab.h"
%}

%%

[ \t]+              { /* ignore white spaces */ }

"-"?[0-9]+          { return NUMBER; }

.                   { fprintf(stderr, "Character '%c' not recognized by the grammar.\n", *yytext); }

%%

int yywrap() {
    return 1;
}
