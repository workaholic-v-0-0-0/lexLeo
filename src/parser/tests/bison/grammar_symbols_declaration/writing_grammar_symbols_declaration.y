// src/parser/tests/bison/grammar_symbols_declaration/writing_grammar_symbols_declaration.y

%token <symbol_name_value> SYMBOL_NAME
%destructor { free($$); } SYMBOL_NAME

%type <ast>
symbol_name_atom
writing

%token WRITE
%token SEMICOLON
