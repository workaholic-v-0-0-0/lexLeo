// src/parser/tests/bison/grammar_symbols_declaration/numbers_grammar_symbols_declaration.y

%token <int_value> INTEGER

%type <ast>
number_atom
numbers
