// src/parser/tests/bison/grammar_rules_stubs/list_of_numbers_rule_dependencies.y

// the following grammar rule is stub only used for unit tests

numbers
  : INTEGER INTEGER
    { (void)$1; (void)$2;
      $$ = stub_numbers_action(); }
