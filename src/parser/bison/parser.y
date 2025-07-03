// src/parser/src/parser.y

%define api.pure full
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
atom

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
      /* empty */ { $$ = NULL;}
    | parameter list_of_parameters
    ;
atom:
      INTEGER {
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
parameter:
    atom
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
      SYMBOL EQUAL right_value {}
    ;
evaluation:
      EVALUATE evaluable {}
    ;
execution:
      EXECUTE SYMBOL {}
    | EXECUTE program {}
    ;
computation:
      COMPUTE SYMBOL {}
    | COMPUTE arithmetic_expression  {}
    ;
reading:
      READ readable {}
    ;
readable:
      INTEGER {}
    | STRING {}
    ;
writing:
      WRITE right_value {}
      ;
right_value:
      INTEGER {}
    | STRING {}
    | SYMBOL {}
    ;
evaluable:
      INTEGER {}
    | STRING {}
    | SYMBOL {}
    ;
arithmetic_expression:
      INTEGER {}
    | arithmetic_expression PLUS arithmetic_expression {}
    ;

%%

int yydebug = 0;

int yyerror(void *ctx, const char *s) {
    fprintf(stderr, "syntax error: %s\n", s);
    return EXIT_SUCCESS;
}
