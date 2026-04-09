// src/ports/parser/bison/grammar_rules/statement_rule.y

statement
    : binding { $$ = $1; }
    | writing { $$ = $1; }
    | reading { $$ = $1; }
    | set { $$ = $1; }
    | function_definition { $$ = $1; }
    | conditional_block { $$ = $1; }
    | while_block { $$ = $1; }
    | evaluable SEMICOLON { $$ = $1; }
    | eval
    ;
