// src/parser/tests/bison/grammar_symbols_declaration/parameters_grammar_symbols_declaration.y

%token <symbol_name_value> SYMBOL_NAME
%destructor { free($$); } SYMBOL_NAME

%type <ast>
symbol_name_atom
parameters
