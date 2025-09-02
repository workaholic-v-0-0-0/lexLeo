// src/parser/tests/bison/grammar_symbols_declaration/string_atom_grammar_symbols_declaration.y

%token <string_value> STRING
%destructor { free($$); } STRING

%type <ast>
string_atom
