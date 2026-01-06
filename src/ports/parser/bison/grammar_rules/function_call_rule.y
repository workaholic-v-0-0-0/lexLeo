// src/ports/parser/bison/grammar_rules/function_call_rule.y

function_call
  : symbol_name_atom list_of_arguments {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_FUNCTION_CALL,
            2,
            $1,
            $2 );
        if (!a) {
            ctx->ops.destroy($1);
            ctx->ops.destroy($2);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_FUNCTION_CALL_NODE_CREATION_FAILED,
                "ast creation for a function call node failed" );
        } else {
            $$ = a;
        }
  }
  ;
