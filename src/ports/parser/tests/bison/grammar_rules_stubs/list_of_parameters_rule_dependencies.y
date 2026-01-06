// src/ports/parser/tests/bison/grammar_rules_stubs/list_of_parameters_rule_dependencies.y

// the following grammar rule is stub only used for unit tests

parameters
  : SYMBOL_NAME SYMBOL_NAME
    { (void)$1; (void)$2;
      $$ = stub_parameters_action(); }
