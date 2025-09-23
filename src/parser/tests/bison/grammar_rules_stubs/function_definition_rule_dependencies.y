// src/parser/tests/bison/grammar_rules_stubs/function_definition_rule_dependencies.y

// the following grammar rule is stub only used for unit tests

function:
    SYMBOL_NAME
    LPAREN
        SYMBOL_NAME
        SYMBOL_NAME
    RPAREN
    LBRACE
        SYMBOL_NAME EQUAL STRING SEMICOLON
        READ SYMBOL_NAME SEMICOLON
        WRITE SYMBOL_NAME SEMICOLON
    RBRACE
    { (void)$1; (void)$3; (void)$4; (void)$7;
      (void)$9; (void)$12; (void)$15;
      $$ = stub_function_action(); }
