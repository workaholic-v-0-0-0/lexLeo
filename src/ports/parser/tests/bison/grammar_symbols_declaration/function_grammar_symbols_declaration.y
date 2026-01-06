// src/ports/parser/tests/bison/grammar_symbols_declaration/function_grammar_symbols_declaration.y

%token <string_value> STRING
%destructor { free($$); } STRING
%token <symbol_name_value> SYMBOL_NAME
%destructor { free($$); } SYMBOL_NAME

%type <ast>
symbol_name_atom
list_of_parameters
block
function

%token SEMICOLON
%token EQUAL
%token WRITE
%token READ
%token LBRACE
%token RBRACE
%token LPAREN
%token RPAREN
