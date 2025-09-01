// src/parser/bison/grammar_rules/number_atom_rule.y

number_atom
  : INTEGER {
        ast *a = ctx->ops.create_int_node($1);
        $$ = (a) ? a
                 :
                 ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_INT_NODE_CREATION_FAILED,
                    "ast creation for a number failed" );
    }
  ;
