// src/ports/parser/tests/bison/grammar_rules_stubs/statement_rule_dependencies.y

// the following grammar rules are stubs only used for unit tests

binding: SYMBOL_NAME EQUAL STRING SEMICOLON
    { (void)$1; (void)$3; $$ = stub_binding_action(); } ;
writing: WRITE SYMBOL_NAME SEMICOLON
    { (void)$2; $$ = stub_writing_action(); } ;
reading: READ SYMBOL_NAME SEMICOLON
    { (void)$2; $$ = stub_reading_action(); } ;
function_definition:
        SYMBOL_NAME LPAREN INTEGER INTEGER RPAREN
        LBRACE
            SYMBOL_NAME EQUAL INTEGER SEMICOLON
        RBRACE
    { (void)$1; (void)$3; (void)$4; (void)$7; (void)$9;
      $$ = stub_function_definition_action(); } ;
evaluable: function_call {$$ = $1;}
function_call:
        SYMBOL_NAME LPAREN SYMBOL_NAME SYMBOL_NAME RPAREN
    { (void)$1; (void)$3; (void)$4;
      $$ = stub_function_call_action(); } ;
eval:
        EVAL SYMBOL_NAME SEMICOLON
    { (void)$2;
      $$ = stub_eval_action(); } ;
set:
        SET SYMBOL_NAME STRING SEMICOLON
    { (void)$2; (void)$3;
      $$ = stub_set_action(); } ;
conditional_block:
        IF INTEGER THEN
            LBRACE
                SYMBOL_NAME EQUAL INTEGER SEMICOLON
            RBRACE
    { (void)$2; (void)$5; (void)$7;
      $$ = stub_conditional_block_action(); } ;
while_block:
        WHILE INTEGER DO
            LBRACE
                SYMBOL_NAME EQUAL INTEGER SEMICOLON
            RBRACE
    { (void)$2; (void)$5; (void)$7;
      $$ = stub_while_block_action(); } ;
