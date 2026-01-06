// src/ports/parser/tests/bison/grammar_symbols_declaration/statement_grammar_symbols_declaration.y

%token <int_value> INTEGER
%token <string_value> STRING
%destructor { free($$); } STRING
%token <symbol_name_value> SYMBOL_NAME
%destructor { free($$); } SYMBOL_NAME

%type <ast>
binding
writing
reading
function_definition
function_call
statement
evaluable
eval
set
conditional_block
while_block

%token SEMICOLON
%token EQUAL
%token WRITE
%token READ
%token LPAREN
%token RPAREN
%token LBRACE
%token RBRACE
%token EVAL
%token SET
%token IF
%token THEN
%token ELSE
%token WHILE
%token DO

