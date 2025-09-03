// src/parser/tests/bison/grammar_rules_stubs/reading_rule_dependencies.y

// the following grammar rules are stubs only used for unit tests

symbol_name_atom: SYMBOL_NAME { $$ = stub_symbol_name_atom_action($1); } ;
