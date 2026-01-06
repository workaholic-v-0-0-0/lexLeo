// src/parser/bison/grammar_rules/list_of_arguments_rule.y

list_of_arguments
  : LPAREN arguments RPAREN {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_LIST_OF_ARGUMENTS,
            1,
            $2 );
        if (!a) {
            ctx->ops.destroy($2);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_LIST_OF_ARGUMENTS_NODE_CREATION_FAILED,
                "ast creation for a node for a list of arguments failed" );
        } else {
            $$ = a;
        }
  }
  ;
