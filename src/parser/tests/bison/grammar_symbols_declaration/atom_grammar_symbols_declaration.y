// src/parser/tests/bison/grammar_symbols_declaration/atom_grammar_symbols_declaration.y

%token <int_value> INTEGER
%token <string_value> STRING
%destructor { free($$); } STRING
%token <symbol_name_value> SYMBOL_NAME

%type <ast>
number_atom
string_atom
symbol_name_atom
atom
