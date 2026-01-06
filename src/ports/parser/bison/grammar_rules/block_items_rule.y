// src/ports/parser/bison/grammar_rules/block_items_rule.y

block_items
  : %empty {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_BLOCK_ITEMS,
            0 );
        if (!a) {
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_BLOCK_ITEMS_NODE_CREATION_FAILED,
                "ast creation for the content of a block node failed" );
        } else {
            $$ = a;
        }
  }
  | block_items statement {
        if ((!$1) || ($1->type == AST_TYPE_ERROR)) {
            ctx->ops.destroy($2);
            $$ = $1;
        } else {
            if (ctx->ops.children_append_take($1, $2)) {
                $$ = $1;
            } else {
                ctx->ops.destroy($1);
                ctx->ops.destroy($2);
                $$ = ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_BLOCK_ITEMS_APPEND_FAILED,
                    "ast append failed when adding a statement to the content of a block" );
            }
        }
  }
  ;
