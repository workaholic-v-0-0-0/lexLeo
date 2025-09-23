// src/parser/tests/bison/grammar_rules_stubs/binding_rule_dependencies.y

// the following grammar rules are stubs only used for unit tests

symbol_name_atom: SYMBOL_NAME
    { (void)$1; $$ = stub_symbol_name_atom_action(); } ;

evaluable: INTEGER
    { (void)$1; $$ = stub_evaluable_action(); } ;
