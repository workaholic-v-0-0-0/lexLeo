// src/parser/bison/grammar_rules/list_of_numbers_rule.y

list_of_numbers
  : LPAREN numbers RPAREN {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_LIST_OF_NUMBERS,
            1,
            $2 );
        if (!a) {
            ctx->ops.destroy($2);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_LIST_OF_NUMBERS_NODE_CREATION_FAILED,
                "ast creation for a node for a list of numbers failed" );
        } else {
            $$ = a;
        }
  }
  ;
