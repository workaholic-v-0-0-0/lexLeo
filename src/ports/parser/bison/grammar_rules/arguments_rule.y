// src/parser/bison/grammar_rules/arguments_rule.y

arguments
  : %empty {
      ast *a = ctx->ops.create_children_node_var(AST_TYPE_ARGUMENTS, 0);
      if (!a) $$ = ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_ARGUMENTS_NODE_CREATION_FAILED,
                    "ast creation for arguments node failed");
      else $$ = a;
    }
  | argument_list { $$ = $1; }
  ;

argument_list
  : evaluable {
      ast *a = ctx->ops.create_children_node_var(AST_TYPE_ARGUMENTS, 1, $1);
      if (!a) { ctx->ops.destroy($1); $$ = ctx->ops.create_error_node_or_sentinel(
                               AST_ERROR_CODE_ARGUMENTS_NODE_CREATION_FAILED,
                               "ast creation for arguments node failed"); }
      else $$ = a;
    }
  | argument_list COMMA evaluable {
      if (!$1 || $1->type == AST_TYPE_ERROR) { ctx->ops.destroy($3); $$ = $1; }
      else if (ctx->ops.children_append_take($1, $3)) $$ = $1;
      else { ctx->ops.destroy($1); ctx->ops.destroy($3);
             $$ = ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_ARGUMENTS_APPEND_FAILED,
                    "ast append failed when adding an evaluable to a list of arguments"); }
    }
  ;
