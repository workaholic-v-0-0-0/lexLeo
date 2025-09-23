// src/parser/tests/bison/grammar_rules_stubs/numbers_rule_dependencies.y

// the following grammar rule is stub only used for unit tests

number_atom
  : INTEGER
    { (void)$1; $$ = stub_number_atom_action(); }
