// src/parser/bison/grammar_rules/evaluable_rule.y

evaluable
  : function_call { $$ = $1; }
  | atom { $$ = $1; }
  | computable { $$ = $1; }
  | QUOTE evaluable {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_QUOTE,
            1,
            $2 );
        if (!a) {
            ctx->ops.destroy($2);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_QUOTE_NODE_CREATION_FAILED,
                "ast creation for a quote node failed" );
        } else {
            $$ = a;
        }
  }
  ;
