// src/parser/bison/number_atom_rule.y

number_atom
  : INTEGER {
      ctx->ops.create_typed_data_int($1);
/* should not have been written yet for one wants a TDD approach
      typed_data *td = ctx->ops.create_typed_data_int($1);
      if (!td) {
        ast *err = ast_create_error_node(
            MEMORY_ALLOCATION_ERROR_CODE,
            "Data wrapper creation for a number failed.");
        $$ = err ? err : ast_error_sentinel();
      } else {
        ast *a = ast_create_typed_data_wrapper(td);
        if (!a) {
          ast_destroy_typed_data_int(td);
          ast *err = ast_create_error_node(
              MEMORY_ALLOCATION_ERROR_CODE,
              "Data wrapper ast creation for a number failed.");
          $$ = err ? err : ast_error_sentinel();
        } else {
          $$ = a;
        }
      }
*/
    }
  ;
