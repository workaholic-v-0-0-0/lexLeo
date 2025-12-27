// src/parser/bison/grammar_rules/symbol_rule.y

symbol
  : SYMBOL SYMBOL_NAME {
        ast *a =
            ctx->ops.create_children_node_var(
                 AST_TYPE_SYMBOL,
                 1,
                 ctx->ops.create_symbol_name_node($2) );
        free($2);
        $$ = (a) ? a
                 :
                 ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_SYMBOL_NODE_CREATION_FAILED,
                    "ast creation for a symbol failed" );
  }
  ;
