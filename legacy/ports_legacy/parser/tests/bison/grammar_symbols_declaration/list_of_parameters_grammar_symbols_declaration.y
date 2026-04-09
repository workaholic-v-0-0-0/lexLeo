// src/ports/parser/tests/bison/grammar_symbols_declaration/list_of_parameters_grammar_symbols_declaration.y

%token <symbol_name_value> SYMBOL_NAME
%destructor { free($$); } SYMBOL_NAME

%type <ast>
parameters
list_of_parameters

%token LPAREN
%token RPAREN
