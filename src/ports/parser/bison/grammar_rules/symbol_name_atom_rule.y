// src/ports/parser/bison/grammar_rules/symbol_name_atom_rule.y

symbol_name_atom
  : SYMBOL_NAME {
        ast *a = ctx->ops.create_symbol_name_node($1);
        free($1);
        $$ = (a) ? a
                 :
                 ctx->ops.create_error_node_or_sentinel(
                    AST_ERROR_CODE_SYMBOL_NAME_NODE_CREATION_FAILED,
                    "ast creation for a symbol name failed" );
  }
  ;
