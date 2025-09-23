// src/parser/tests/bison/grammar_rules_stubs/statement_rule_dependencies.y

// the following grammar rules are stubs only used for unit tests

binding: SYMBOL_NAME EQUAL STRING SEMICOLON
    { (void)$1; (void)$3; $$ = stub_binding_action(); } ;
writing: WRITE SYMBOL_NAME SEMICOLON
    { (void)$2; $$ = stub_writing_action(); } ;
reading: READ SYMBOL_NAME SEMICOLON
    { (void)$2; $$ = stub_reading_action(); } ;
function_definition:
        SYMBOL_NAME LPAREN INTEGER INTEGER RPAREN
        LBRACE
            SYMBOL_NAME EQUAL INTEGER SEMICOLON
        RBRACE
    { (void)$1; (void)$3; (void)$4; (void)$7; (void)$9;
      $$ = stub_function_definition_action(); } ;
function_call:
        SYMBOL_NAME LPAREN SYMBOL_NAME SYMBOL_NAME RPAREN
    { (void)$1; (void)$3; (void)$4;
      $$ = stub_function_call_action(); } ;
