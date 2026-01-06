// src/ports/parser/bison/grammar_rules/binding_rule.y

binding
  : symbol_name_atom EQUAL evaluable SEMICOLON {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_BINDING,
            2,
            $1,
            $3 );
        if (!a) {
            ctx->ops.destroy($1);
            ctx->ops.destroy($3);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_BINDING_NODE_CREATION_FAILED,
                "ast creation for a binding node failed" );
        } else {
            $$ = a;
        }
  }
  ;
