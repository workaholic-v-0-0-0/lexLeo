// src/parser/bison/grammar_rules/conditional_block_rule.y

conditional_block
    : IF evaluable THEN block {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_CONDITIONAL_BLOCK,
            2,
            $2,
            $4 );
        if (!a) {
            ctx->ops.destroy($2);
            ctx->ops.destroy($4);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_CONDITIONAL_BLOCK_NODE_CREATION_FAILED,
                "ast creation for a conditional block node failed" );
        } else {
            $$ = a;
        }
    }
    | IF evaluable THEN block ELSE block {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_CONDITIONAL_BLOCK,
            3,
            $2,
            $4,
            $6 );
        if (!a) {
            ctx->ops.destroy($2);
            ctx->ops.destroy($4);
            ctx->ops.destroy($6);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_CONDITIONAL_BLOCK_NODE_CREATION_FAILED,
                "ast creation for a conditional block node failed" );
        } else {
            $$ = a;
        }
    }
    ;
