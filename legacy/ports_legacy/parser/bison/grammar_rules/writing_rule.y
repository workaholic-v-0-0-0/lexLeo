// src/ports/parser/bison/grammar_rules/writing_rule.y

writing
  : WRITE evaluable SEMICOLON {
        ast *a = ctx->ops.create_children_node_var(
                            AST_TYPE_WRITING,
                            1,
                            $2 );
        if (!a) {
            ctx->ops.destroy($2);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_WRITING_NODE_CREATION_FAILED,
                "ast creation for a writing node failed" );
        } else {
            $$ = a;
        }
  }
  ;
