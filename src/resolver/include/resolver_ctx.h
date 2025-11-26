// src/resolver/include/resolver_ctx.h

#ifndef LEXLEO_RESOLVER_CTX_H
#define LEXLEO_RESOLVER_CTX_H

#include "list.h"
#include "symtab.h"

#include <stdbool.h>

typedef struct resolver_ops {
    list (*push)(list l, void *e);
    void *(*pop)(list *l_p);
    int (*intern_symbol)(symtab *st, char *name);
    symbol *(*get)(symtab *st, const char *name);
    symtab *(*wind_scope)(symtab *st);
    symtab *(*unwind_scope)(symtab *st);
    // hook for cli_store_symbol
    bool (*store_symbol)(symbol *sym, void *user_data);
} resolver_ops;

extern const resolver_ops RESOLVER_OPS_DEFAULT;

typedef struct resolver_ctx {
    resolver_ops ops;
    symtab *st;
    // opaque argument for the store_symbol callback
    void *user_data;
} resolver_ctx;

#endif //LEXLEO_RESOLVER_CTX_H
