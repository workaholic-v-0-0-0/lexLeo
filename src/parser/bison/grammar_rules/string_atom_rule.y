// src/parser/bison/grammar_rules/string_atom_rule.y

string_atom
  : STRING {

/*
    ast *a = ctx->ops.create_int_node($1);
    if (!a) {
        ast *err = ctx->ops.create_error_node(
            AST_ERROR_CODE_INT_NODE_CREATION_FAILED,
            "ast creation for a number failed" );
        $$ = err ? err : ctx->ops.error_sentinel();
    } else {
        $$ = a;
    }
*/
    }
  ;
