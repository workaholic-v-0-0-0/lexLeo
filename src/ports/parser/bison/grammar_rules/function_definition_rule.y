// src/ports/parser/bison/grammar_rules/function_definition_rule.y

function_definition
  : DEFINE function {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_FUNCTION_DEFINITION,
            1,
            $2 );
        if (!a) {
            ctx->ops.destroy($2);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_FUNCTION_DEFINITION_NODE_CREATION_FAILED,
                "ast creation for a function definition node failed" );
        } else {
            $$ = a;
        }
  }
  ;
