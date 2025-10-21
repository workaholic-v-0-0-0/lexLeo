// src/parser/bison/grammar_rules/numbers_rule.y

numbers
  : %empty {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_NUMBERS,
            0 );
        if (!a) {
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_NUMBERS_NODE_CREATION_FAILED,
                "ast creation for numbers node failed" );
        } else {
            $$ = a;
        }
  }
  | numbers number_atom {
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
                    AST_ERROR_CODE_NUMBERS_APPEND_FAILED,
                    "ast append failed when adding a number to a list of numbers" );
            }
        }
  }
  ;
