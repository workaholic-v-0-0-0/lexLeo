// src/ports/parser/tests/bison/grammar_symbols_declaration/reading_grammar_symbols_declaration.y

%token <symbol_name_value> SYMBOL_NAME
%destructor { free($$); } SYMBOL_NAME

%type <ast>
symbol_name_atom
reading

%token READ
%token SEMICOLON
