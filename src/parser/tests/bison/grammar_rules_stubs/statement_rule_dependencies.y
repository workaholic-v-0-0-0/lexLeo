// src/parser/tests/bison/grammar_rules_stubs/statement_rule_dependencies.y

// the following grammar rules are stubs only used for unit tests

binding: SYMBOL_NAME EQUAL STRING SEMICOLON
    { (void)$1; (void)$3; $$ = stub_binding_action(); } ;
writing: WRITE SYMBOL_NAME SEMICOLON
    { (void)$2; $$ = stub_writing_action(); } ;
reading: READ SYMBOL_NAME SEMICOLON
    { (void)$2; $$ = stub_reading_action(); } ;
