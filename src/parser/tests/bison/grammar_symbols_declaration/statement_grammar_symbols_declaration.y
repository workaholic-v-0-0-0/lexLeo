// src/parser/tests/bison/grammar_symbols_declaration/statement_grammar_symbols_declaration.y

%token <string_value> STRING
%destructor { free($$); } STRING
%token <symbol_name_value> SYMBOL_NAME
%destructor { free($$); } SYMBOL_NAME

%type <ast>
binding
writing
reading
statement

%token SEMICOLON
%token EQUAL
%token WRITE
%token READ
