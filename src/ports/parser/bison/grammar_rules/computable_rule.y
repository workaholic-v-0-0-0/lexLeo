// src/ports/parser/bison/grammar_rules/computable_rule.y

computable
    : SUBTRACT computable %prec UMINUS {
        ast *a = ctx->ops.create_children_node_var(
           AST_TYPE_NEGATION,
           1,
           $2 );
        if (!a) {
           ctx->ops.destroy($2);
           $$ = ctx->ops.create_error_node_or_sentinel(
               AST_ERROR_CODE_NEGATION_NODE_CREATION_FAILED,
               "ast creation for a negation node failed");
        } else {
           $$ = a;
        }
    }
    | computable ADD computable {
        ast *a = ctx->ops.create_children_node_var(
            AST_TYPE_ADDITION,
            2,
            $1,
            $3 );
        if (!a) {
            ctx->ops.destroy($1);
            ctx->ops.destroy($3);
            $$ = ctx->ops.create_error_node_or_sentinel(
                AST_ERROR_CODE_ADDITION_NODE_CREATION_FAILED,
                "ast creation for a addition node failed" );
        } else {
            $$ = a;
        }
     }
    | computable SUBTRACT computable {
         ast *a = ctx->ops.create_children_node_var(
             AST_TYPE_SUBTRACTION,
             2,
             $1,
             $3 );
         if (!a) {
             ctx->ops.destroy($1);
             ctx->ops.destroy($3);
             $$ = ctx->ops.create_error_node_or_sentinel(
                 AST_ERROR_CODE_SUBTRACTION_NODE_CREATION_FAILED,
                 "ast creation for a subtraction node failed" );
         } else {
             $$ = a;
         }
     }
    | computable MULTIPLY computable {
          ast *a = ctx->ops.create_children_node_var(
              AST_TYPE_MULTIPLICATION,
              2,
              $1,
              $3 );
          if (!a) {
              ctx->ops.destroy($1);
              ctx->ops.destroy($3);
              $$ = ctx->ops.create_error_node_or_sentinel(
                  AST_ERROR_CODE_MULTIPLICATION_NODE_CREATION_FAILED,
                  "ast creation for a multiplication node failed" );
          } else {
              $$ = a;
          }
      }
    | computable DIVIDE computable {
          ast *a = ctx->ops.create_children_node_var(
              AST_TYPE_DIVISION,
              2,
              $1,
              $3 );
          if (!a) {
              ctx->ops.destroy($1);
              ctx->ops.destroy($3);
              $$ = ctx->ops.create_error_node_or_sentinel(
                  AST_ERROR_CODE_DIVISION_NODE_CREATION_FAILED,
                  "ast creation for a division node failed" );
          } else {
              $$ = a;
          }
      }
    | LPAREN computable RPAREN {
          $$ = $2;
      }
    | atom {
          $$ = $1;
      }
    ;

