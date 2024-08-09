#include <stdio.h>
#include "parser/parser.tab.h"

int main(void)
{
    printf("Hello, World!\n");
    yyparse();
    return 0;
}
