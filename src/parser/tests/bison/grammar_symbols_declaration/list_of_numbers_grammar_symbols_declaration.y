// src/parser/tests/bison/grammar_symbols_declaration/list_of_numbers_grammar_symbols_declaration.y

%token <int_value> INTEGER

%type <ast>
numbers
list_of_numbers

%token LPAREN
%token RPAREN
