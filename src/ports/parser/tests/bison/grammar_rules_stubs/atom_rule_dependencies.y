// src/ports/parser/tests/bison/grammar_rules_stubs/atom_rule_dependencies.y

// the following grammar rules are stubs only used for unit tests

number_atom: INTEGER { $$ = stub_number_atom_action($1); } ;
string_atom: STRING { $$ = stub_string_atom_action($1); } ;
symbol_name_atom: SYMBOL_NAME { $$ = stub_symbol_name_atom_action($1); } ;
