// PLACEHOLDER

/*******************************************************************************
# Name: parser.y
# Role:
# Author: Sylvain Labopin
# Version: 0.0.0
# License: GPLv3
# Compilation:
#
# Usage:
#
*******************************************************************************/

%code requires {
   #include "parser.tab.h"
}

%{

#include "data_structures/list.h"
#include "data_structures/hashtable.h"
#include "data_structures/tree.h"
#include "symtab/symtab.h"
#include "ast/ast.h"

#include <stdlib.h>

#define YYDEBUG 1
int yyerror(char *);
int yylex(void);

%}

%union {
    int integer;
    char *string;
};

%token <integer> NUMBER
%token <string> IDENTIFIER
%token <string> CODE // can be a non executable fragment of a program

%token PARAMETERS // just NUMBER for the first version

// key-word lexeme relative to instruction
%token EQUAL
%token EVALUATE
%token EXECUTE
%token COMPUTE
%token READ
%token WRITE

//punctuation
%token SEMICOLON
%token LEFT_PARENTHESE
%token RIGHT_PARENTHESE

// arithmetic
%type <integer> arithmetical_expression
%token <integer> PLUS MINUS MULTIPLY DIVIDE

// rules avoiding ambiguities
%left PLUS MINUS
%left MULTIPLY DIVIDE

%start program

%%

program:
      instruction program_end PARAMETERS list_of_parameters
    | instruction program_end
    ;
program_end:
      /* empty */
    | SEMICOLON program
    ;
list_of_parameters:
      /* empty */
    | parameter list_of_parameters
    ;
parameter:
      NUMBER
    // | IDENTIFIER // TO DO FOR NEXT VERSION
    // | CODE // TO DO FOR NEXT VERSION
    ;
instruction:
      binding
    | evaluation
    | execution
    | computation
    | reading
    | writing
    ;
binding: IDENTIFIER EQUAL right_value
{printf("This is an assignement.\n");}
;
evaluation: EVALUATE evaluable;
execution:
      EXECUTE CODE
    | EXECUTE IDENTIFIER
    ;
computation:
      arithmetic_expression
      COMPUTE IDENTIFIER
    | COMPUTE arithmetic_expression
    ;
reading: READ IDENTIFIER;
writing: WRITE right_value;
right_value:
      NUMBER
    | IDENTIFIER
    | CODE
    ;
evaluable:
      NUMBER
    | IDENTIFIER
    ;
arithmetic_expression:
    | arithmetic_expression PLUS arithmetic_expression
//                 {$$ = $1 + $3; /* Add two expressions */ }// ICI
//    | expr MINUS expr            { $$ = $1 - $3; /* Subtract two expressions */ }
//    | expr MULTIPLY expr         { $$ = $1 * $3; /* Multiply two expressions */ }
//    | expr DIVIDE expr           { $$ = $1 / $3; /* Divide two expressions */ }
//    | NUMBER                     { $$ = $1; /* Direct number */ }
//    | VARIABLE                   { $$ = retrieve_value($1); /* Retrieve variable's value */ }
//    | '(' expr ')'               { $$ = $2; /* Parentheses for grouping */ }
    ;

    %%

int yydebug = 0;

int yyerror(char *s) {
    fprintf(stderr, "syntax error: %s\n", s);
    return EXIT_SUCCESS;
}
