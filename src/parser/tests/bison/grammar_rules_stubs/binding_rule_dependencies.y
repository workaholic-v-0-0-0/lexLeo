// src/parser/tests/bison/grammar_rules_stubs/binding_rule_dependencies.y

// the following grammar rules are stubs only used for unit tests

symbol_name_atom: SYMBOL_NAME { $$ = stub_symbol_name_atom_action($1); } ;
atom
  : INTEGER { $$ = stub_atom_from_int_action($1); }
  | STRING { $$ = stub_atom_from_string_action($1); }
  | SYMBOL_NAME { $$ = stub_atom_from_symbol_name_action($1); }
  ;
