// src/parser/tests/bison/grammar_symbols_declaration/evaluable_grammar_symbols_declaration.y

%token <int_value> INTEGER
%token <symbol_name_value> SYMBOL_NAME
%destructor { free($$); } SYMBOL_NAME

%type <ast>
function_call
atom
computable
evaluable

%token LPAREN
%token RPAREN
%token QUOTE
%token MULTIPLY
%token ADD
