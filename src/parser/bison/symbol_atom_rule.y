// src/parser/bison/symbol_atom_rule.y

symbol_atom
    : SYMBOL {
        context ctx = (context) yyget_extra(scanner);
        if (!symtab_contains(ctx->st, $1)) {
            symbol *s = symtab_create_symbol($1, NULL);
            if (!s) {
                // handle error node for the symbol's not been properly created
                // $$ = error...
                // "return"
            }
            if (symtab_add(ctx->st, s)) { // interning new symbol unbound
                // handle error node for symbol interning has not been done properly
                //$$ = error...
                // "return"
            }
        }
        // from now on, the symbol has been interned
        symbol s = symtab_get(ctx->st, $1);
        typed_data *td = ast_create_typed_data_symbol(s);
        if (!td) {
            // handle error node for typed data for the symbol cannot be constructed properly
            //$$ = error...
            // "return"
        }
        ast *ast_create_typed_data_wrapper(a);
        if (!a) {
            // handle error node for ast for wrap the typed data for the symbol cannot be constructed properly
            //$$ = error...
            // "return"
        }
        $$ = a;
    }
    ;
