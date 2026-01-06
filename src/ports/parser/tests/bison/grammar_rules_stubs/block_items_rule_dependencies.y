// src/ports/parser/tests/bison/grammar_rules_stubs/block_items_rule_dependencies.y

// the following grammar rules are stubs only used for unit tests

statement
  : SYMBOL_NAME EQUAL STRING SEMICOLON
    { (void)$1; (void)$3; $$ = stub_binding_statement_action(); }
  | READ SYMBOL_NAME SEMICOLON
    { (void)$2; $$ = stub_reading_statement_action(); }
  | WRITE SYMBOL_NAME SEMICOLON
    { (void)$2; $$ = stub_writing_statement_action(); }
