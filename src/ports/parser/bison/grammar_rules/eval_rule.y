// src/ports/parser/bison/grammar_rules/eval_rule.y

eval
  : EVAL symbol_name_atom SEMICOLON {
        ast *a = ctx->ops.create_children_node_var(
                            AST_TYPE_EVAL,
                            1,
                            $2 );
        if (!a) {
            ctx->ops.destroy($2);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_EVAL_NODE_CREATION_FAILED,
                "ast creation for an eval node failed" );
        } else {
            $$ = a;
        }
  }
  ;
