// src/ports/parser/tests/bison/grammar_symbols_declaration/function_definition_grammar_symbols_declaration.y

%token <string_value> STRING
%destructor { free($$); } STRING
%token <symbol_name_value> SYMBOL_NAME
%destructor { free($$); } SYMBOL_NAME

%type <ast>
function
function_definition

%token SEMICOLON
%token EQUAL
%token WRITE
%token READ
%token LBRACE
%token RBRACE
%token LPAREN
%token RPAREN
%token DEFINE
