// src/parser/tests/bison/grammar_rules_stubs/function_call_rule_dependencies.y

// the following grammar rules are stubs only used for unit tests

symbol_name_atom: SYMBOL_NAME {
    (void)$1 ; $$ = stub_symbol_name_atom_action(); } ;

list_of_arguments
    : LPAREN INTEGER INTEGER RPAREN {
        (void)$2; (void)$3;
        $$ = stub_list_of_arguments_action(); } ;
