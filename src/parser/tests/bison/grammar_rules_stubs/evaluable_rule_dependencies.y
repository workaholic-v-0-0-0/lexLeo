// src/parser/tests/bison/grammar_rules_stubs/evaluable_rule_dependencies.y

// the following grammar rules are stubs only used for unit tests

function_call:
        SYMBOL_NAME LPAREN INTEGER INTEGER RPAREN
        { (void)$1; (void)$3; (void)$4;
          $$ = stub_function_call_action(); } ;

atom: SYMBOL_NAME { (void)$1; $$ = stub_atom_action(); } ;

computable:
        INTEGER MULTIPLY LPAREN INTEGER ADD INTEGER RPAREN
        { (void)$1; (void)$4; (void)$6;
          $$ = stub_computable_action(); } ;
