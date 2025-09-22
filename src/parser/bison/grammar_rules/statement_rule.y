// src/parser/bison/grammar_rules/statement_rule.y

statement
  : binding { $$ = $1; }
  | writing { $$ = $1; }
  | reading { $$ = $1; }
  | function_definition { $$ = $1; }
  | function_call SEMICOLON { $$ = $1; }
  ;
