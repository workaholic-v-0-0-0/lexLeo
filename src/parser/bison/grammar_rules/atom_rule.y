// src/parser/bison/grammar_rules/atom_rule.y

atom
  : number_atom { $$ = $1; }
  | string_atom { $$ = $1; }
  | symbol_name_atom { $$ = $1; }
  ;
