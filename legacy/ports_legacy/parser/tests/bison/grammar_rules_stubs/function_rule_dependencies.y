// src/ports/parser/tests/bison/grammar_rules_stubs/function_rule_dependencies.y

// the following grammar rules are stubs only used for unit tests

symbol_name_atom: SYMBOL_NAME {
    (void)$1 ; $$ = stub_symbol_name_atom_action(); } ;

list_of_parameters
    : LPAREN SYMBOL_NAME SYMBOL_NAME RPAREN {
        (void)$2; (void)$3;
        $$ = stub_list_of_parameters_action(); } ;

block
    : LBRACE
        SYMBOL_NAME EQUAL STRING SEMICOLON
        READ SYMBOL_NAME SEMICOLON
        WRITE SYMBOL_NAME SEMICOLON
      RBRACE {
        (void)$2; (void)$4; (void)$7; (void)$10;
        $$ = stub_block_action(); } ;
