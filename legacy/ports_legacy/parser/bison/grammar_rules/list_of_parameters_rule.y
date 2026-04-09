// src/ports/parser/bison/grammar_rules/list_of_parameters_rule.y

list_of_parameters
  : LPAREN parameters RPAREN {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_LIST_OF_PARAMETERS,
            1,
            $2 );
        if (!a) {
            ctx->ops.destroy($2);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_LIST_OF_PARAMETERS_NODE_CREATION_FAILED,
                "ast creation for a node for a list of parameters failed" );
        } else {
            $$ = a;
        }
  }
  ;
