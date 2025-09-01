// src/parser/bison/grammar_rules/string_atom_rume.y

string_atom
  : STRING {
        ast *a = ctx->ops.create_string_node($1);
        $$ = (a) ? a
                 :
                 ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_STRING_NODE_CREATION_FAILED,
                    "ast creation for a string failed" );
    }
  ;
