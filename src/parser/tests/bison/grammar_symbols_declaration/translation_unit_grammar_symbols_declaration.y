// src/parser/tests/bison/grammar_symbols_declaration/translation_unit_grammar_symbols_declaration.y

%token <string_value> STRING
%destructor { free($$); } STRING
%token <symbol_name_value> SYMBOL_NAME
%destructor { free($$); } SYMBOL_NAME

%type <ast>
statement
translation_unit

%token SEMICOLON
%token EQUAL
%token WRITE
%token READ
