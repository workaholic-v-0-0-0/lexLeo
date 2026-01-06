// src/ports/parser/tests/bison/grammar_rules_stubs/parameters_rule_dependencies.y

// the following grammar rule is stub only used for unit tests

symbol_name_atom
  : SYMBOL_NAME
    { (void)$1; $$ = stub_symbol_name_atom_action(); }
