// src/ports/parser/bison/grammar_rules/parameters_rule.y

parameters
  : %empty {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_PARAMETERS,
            0 );
        if (!a) {
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_PARAMETERS_NODE_CREATION_FAILED,
                "ast creation for parameters node failed" );
        } else {
            $$ = a;
        }
  }
  | parameters symbol_name_atom {
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
                    AST_ERROR_CODE_PARAMETERS_APPEND_FAILED,
                    "ast append failed when adding a parameter to a list of parameters" );
            }
        }
  }
  ;
