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
   #include "parser/parser.tab.h"
}


%{
//#include "include/ast/ast.h"
//#include "include/symtab/symtab.h"

#define YYDEBUG 1
int yyerror(char *);
int yylex(void);

typedef struct cons {char *key; double value; struct cons * cdr;} cons, *list;

typedef enum { NodeType1, NodeType2, NodeType3 } NodeType;

typedef int symbol; // placeholder

typedef struct TreeNode {
    NodeType type;
    union {
        int int_val;
        double float_val;
        symbol symbol_location;
    } data;
    list childrens;
} tree_node, *tree;

%}

%union {
    int t;
};

%token SEMICOLON
%token <t> NUMBER
%token <t> VARIABLE
%token <t> PLUS MINUS MULTIPLY DIVIDE ASSIGN

%left PLUS MINUS
%left MULTIPLY DIVIDE

%start program

%%

program:
    | program instruction
    ;
instruction:
    expr SEMICOLON
    ;

expr:
      VARIABLE ASSIGN expr       { /* Assign the result of expr to a variable */ }
    | expr PLUS expr             { /* Add two expressions */ }
    //| expr MINUS expr            { $$ = $1 - $3; /* Subtract two expressions */ }
    //| expr MULTIPLY expr         { $$ = $1 * $3; /* Multiply two expressions */ }
    //| expr DIVIDE expr           { $$ = $1 / $3; /* Divide two expressions */ }
    //| NUMBER                     { $$ = $1;}
    //| VARIABLE                   { $$ = retrieve_value($1); /* Retrieve variable's value */ }
    //| '(' expr ')'               { $$ = $2; /* Parentheses for grouping */ }
    ;

%%

int yydebug = 0;

int yyerror(char *s) {
    fprintf(stderr, "syntax error\n");
    return 0;
}
