// src/parser/bison/grammar_rules/while_block_rule.y

while_block
    : WHILE evaluable DO block {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_WHILE_BLOCK,
            2,
            $2,
            $4 );
        if (!a) {
            ctx->ops.destroy($2);
            ctx->ops.destroy($4);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_WHILE_BLOCK_NODE_CREATION_FAILED,
                "ast creation for a while block node failed" );
        } else {
            $$ = a;
        }
    }
