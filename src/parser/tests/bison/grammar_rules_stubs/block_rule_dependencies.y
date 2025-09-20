// src/parser/tests/bison/grammar_rules_stubs/block_rule_dependencies.y

// the following grammar rule is stub only used for unit tests

block_items
  : SYMBOL_NAME EQUAL STRING SEMICOLON
    READ SYMBOL_NAME SEMICOLON
    WRITE SYMBOL_NAME SEMICOLON
    { (void)$1; (void)$3; (void)$6; (void)$9;
      $$ = stub_block_items_action(); }
