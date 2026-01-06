// src/parser/bison/grammar_rules/set_rule.y

set
  : SET symbol_name_atom evaluable SEMICOLON {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_SET,
            2,
            $2,
            $3 );
        if (!a) {
            ctx->ops.destroy($2);
            ctx->ops.destroy($3);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_SET_NODE_CREATION_FAILED,
                "ast creation for a set node failed" );
        } else {
            $$ = a;
        }
  }
  ;
