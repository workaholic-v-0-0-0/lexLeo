// src/parser/tests/bison/grammar_rules_stubs/writing_rule_dependencies.y

// the following grammar rule is stub only used for unit tests

symbol_name_atom: SYMBOL_NAME { $$ = stub_symbol_name_atom_action($1); } ;
