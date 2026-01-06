// src/ports/parser/bison/grammar_rules/translation_unit_rule.y

translation_unit
  : %empty {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_TRANSLATION_UNIT,
            0 );
        if (!a) {
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_TRANSLATION_UNIT_NODE_CREATION_FAILED,
                "ast creation for a translation unit node failed" );
        } else {
            $$ = a;
        }
  }
  | translation_unit statement {
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
                    AST_ERROR_CODE_TRANSLATION_UNIT_APPEND_FAILED,
                    "ast append failed when adding a statement to the translation unit" );
            }
        }
  }
  ;
