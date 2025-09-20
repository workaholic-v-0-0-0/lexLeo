// src/parser/bison/grammar_rules/block_rule.y

block
  : LBRACE block_items RBRACE {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_BLOCK,
            1,
            $2 );
        if (!a) {
            ctx->ops.destroy($2);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_BLOCK_NODE_CREATION_FAILED,
                "ast creation for a block node failed" );
        } else {
            $$ = a;
        }
  }
  ;
