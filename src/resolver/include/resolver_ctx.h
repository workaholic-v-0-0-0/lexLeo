// src/resolver/include/resolver_ctx.h

#ifndef LEXLEO_RESOLVER_CTX_H
#define LEXLEO_RESOLVER_CTX_H

#include "list.h"
#include "symtab.h"

typedef struct resolver_ops {
    list (*push)(list l, void *e);
    void *(*pop)(list *l_p);
    int (*intern_symbol)(symtab *st, char *name);
    symbol *(*get)(symtab *st, const char *name);
} resolver_ops;

extern const resolver_ops RESOLVER_OPS_DEFAULT;

typedef struct resolver_ctx {
    resolver_ops ops;
    symtab *st;
} resolver_ctx;

#endif //LEXLEO_RESOLVER_CTX_H