// src/parser/bison/grammar_rules/reading_rule.y

reading
  : READ symbol_name_atom SEMICOLON {
        ast *a = ctx->ops.create_children_node_var(
                    AST_TYPE_READING,
                    1,
                    $2 );
        if (!a) {
            ctx->ops.destroy($2);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_READING_NODE_CREATION_FAILED,
                "ast creation for a reading node failed" );
        } else {
            $$ = a;
        }
  }
  ;
