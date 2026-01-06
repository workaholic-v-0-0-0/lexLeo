// src/ports/parser/tests/bison/grammar_symbols_declaration/function_call_grammar_symbols_declaration.y

%token <int_value> INTEGER
%token <symbol_name_value> SYMBOL_NAME
%destructor { free($$); } SYMBOL_NAME

%type <ast>
symbol_name_atom
list_of_arguments
function_call

%token LPAREN
%token RPAREN
