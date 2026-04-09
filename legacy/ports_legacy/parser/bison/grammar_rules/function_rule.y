// src/ports/parser/bison/grammar_rules/function_rule.y

function
  : symbol_name_atom list_of_parameters block {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_FUNCTION,
            3,
            $1,
            $2,
            $3 );
        if (!a) {
            ctx->ops.destroy($1);
            ctx->ops.destroy($2);
            ctx->ops.destroy($3);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_FUNCTION_NODE_CREATION_FAILED,
                "ast creation for a function node failed" );
        } else {
            $$ = a;
        }
  }
  ;
