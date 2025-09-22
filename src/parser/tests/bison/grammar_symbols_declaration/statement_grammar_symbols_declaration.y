// src/parser/tests/bison/grammar_symbols_declaration/statement_grammar_symbols_declaration.y

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

%token SEMICOLON
%token EQUAL
%token WRITE
%token READ
%token LPAREN
%token RPAREN
%token LBRACE
%token RBRACE
