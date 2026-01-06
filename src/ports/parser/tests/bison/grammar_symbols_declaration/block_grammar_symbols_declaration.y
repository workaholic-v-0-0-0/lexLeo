// src/ports/parser/tests/bison/grammar_symbols_declaration/block_grammar_symbols_declaration.y

%token <string_value> STRING
%destructor { free($$); } STRING
%token <symbol_name_value> SYMBOL_NAME
%destructor { free($$); } SYMBOL_NAME

%type <ast>
block_items
block

%token SEMICOLON
%token EQUAL
%token WRITE
%token READ
%token LBRACE
%token RBRACE
