// src/parser/src/parser.y

%code requires {
   #include "parser.tab.h"
   #include "ast/ast.h"
}

%{

#include "symtab.h"
#include "ast.h"

#include <stdlib.h>

#define YYDEBUG 1
int yyerror(char *);
int yylex(void);

%}

%union {
    int int_value;
    char *identifier;
}

%token <integer> INTEGER
%token <char *> IDENTIFIER

%token PARAMETERS // just INTEGER for the first version

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
%token PLUS MINUS MULTIPLY DIVIDE

// rules avoiding ambiguities
%left PLUS MINUS
%left MULTIPLY DIVIDE

// non-terminal lexeme types
%type <ast *>
program
list_of_parameters
parameter
instruction
binding
evaluation
execution
computation
reading
writing
arithmetic_expression

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
      INTEGER
    // | IDENTIFIER // for another version later
    ;
instruction:
      binding
    | evaluation
    | execution
    | computation
    | reading
    | writing
    ;
binding:
      IDENTIFIER EQUAL right_value
    ;
evaluation:
      EVALUATE evaluable
    ;
execution:
      EXECUTE IDENTIFIER
    | EXECUTE program
    ;
computation:
      COMPUTE IDENTIFIER
    | COMPUTE arithmetic_expression
    ;
reading:
      READ IDENTIFIER;
writing:
      WRITE right_value;
right_value:
      INTEGER
    | IDENTIFIER
    ;
evaluable:
      INTEGER
    | IDENTIFIER
    ;
arithmetic_expression:
      INTEGER
    | arithmetic_expression PLUS arithmetic_expression
    ;

%%

int yydebug = 0;

int yyerror(char *s) {
    fprintf(stderr, "syntax error: %s\n", s);
    return EXIT_SUCCESS;
}
