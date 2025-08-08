// src/parser/tests/bison/atom_grammar.y

atom
  : INTEGER {
      $$ = ast_create_typed_data_wrapper(
             ast_create_typed_data_int($1)
           );
    }
  | STRING {
      $$ = ast_create_typed_data_wrapper(
             ast_create_typed_data_string($1)
           );
    }
  | SYMBOL {
      context ctx = (context)yyget_extra(scanner);
      if (symtab_contains(ctx->st, $1)) {
          $$ = ast_create_typed_data_wrapper(
                 ast_create_typed_data_symbol(
                   symtab_get(ctx->st, $1)
                 )
               );
      } else {
        printf("not yet declared\n");
        $$ = NULL;
      }
    }
  ;
