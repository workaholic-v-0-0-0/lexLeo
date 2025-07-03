// src/parser/src/parser.y

%define api.pure full
%expect 1
%lex-param   {yyscan_t scanner}
%parse-param {yyscan_t scanner}

%code requires {
   #include "ast.h"
   typedef struct symtab symtab;
   typedef struct context { symtab *st; } *context;

   // forward declaration of yyscan_t for Bison (as Flex generates it)
   #ifndef YY_TYPEDEF_YY_SCANNER_T
   #define YY_TYPEDEF_YY_SCANNER_T
   typedef void* yyscan_t;
   extern void* yyget_extra(yyscan_t scanner);
   #endif
}

%{

#include "ast.h"
#include "symtab.h"
#include <stdlib.h>

#define YYDEBUG 1

%}

%code provides {
    int yyerror(yyscan_t scanner, const char *s);
    int yylex(YYSTYPE *yylval, yyscan_t scanner);
}

%union {
    int int_value;
    char *string_value;
    char *symbol_name_value;
    ast *ast;
}

%token <int_value> INTEGER
%token <string_value> STRING
%token <symbol_name_value> SYMBOL

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
%token LEFT_PARENTHESIS
%token RIGHT_PARENTHESIS

// arithmetic
%token PLUS MINUS MULTIPLY DIVIDE

// rules avoiding ambiguities
%left PLUS MINUS
%left MULTIPLY DIVIDE

// non-terminal lexeme types
%type <ast>
program
statements
binding
evaluation
execution
computation
//reading
//writing
atom

%start program

%%

program
    : statements
    ;

statements
    : /* empty */ {}
    | statement_list {}
    ;

statement_list
    : statement
    | statement_list SEMICOLON statement
    ;

statement
    : binding
    | evaluation
    | execution
    | computation
 // | reading
 // | writing
 // | function_call
    ;

binding
    : SYMBOL EQUAL expression {}
    ;

evaluation
    : EVALUATE SYMBOL {}
    ;

execution
    : EXECUTE program {}
    ;

computation
    : COMPUTE expression {}
    ;

// function_call
//  :

expression
    : atom
    | LEFT_PARENTHESIS expression RIGHT_PARENTHESIS
    | expression PLUS expression
    | expression MINUS expression
    | expression MULTIPLY expression
    | expression DIVIDE expression
 // | function_call
    ;

atom
    : INTEGER {
$$ =
    ast_create_typed_data_wrapper(
        ast_create_typed_data_int($1) );
}
    | STRING {
$$ = ast_create_typed_data_wrapper(ast_create_typed_data_string($1));
}
    | SYMBOL {
context ctx = (context)yyget_extra(scanner);
if (symtab_contains_local(ctx->st, $1)) {
    $$ =
        ast_create_typed_data_wrapper(
            ast_create_typed_data_symbol(
                symtab_get_local(ctx->st, $1) ) );
} else {
    $$ =
        ast_create_typed_data_wrapper(
            ast_create_typed_data_symbol(
                symtab_get_local(ctx->st, $1) ) );
}
}
    ;

%%

int yydebug = 0;

int yyerror(void *ctx, const char *s) {
    fprintf(stderr, "syntax error: %s\n", s);
    return EXIT_SUCCESS;
}
