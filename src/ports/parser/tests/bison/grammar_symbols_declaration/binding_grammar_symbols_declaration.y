// src/ports/parser/tests/bison/grammar_symbols_declaration/binding_grammar_symbols_declaration.y

%token <int_value> INTEGER
%token <string_value> STRING
%destructor { free($$); } STRING
%token <symbol_name_value> SYMBOL_NAME
%destructor { free($$); } SYMBOL_NAME

%type <ast>
symbol_name_atom
evaluable
binding

%token EQUAL
%token SEMICOLON
