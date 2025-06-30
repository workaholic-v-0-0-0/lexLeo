// src/symtab/src/symtab.c

#include "symtab.h"

#include "ast.h"
#include "internal/symtab_internal.h"
#include "internal/symtab_memory_allocator.h"

#include <stdlib.h>

void symtab_destroy_symbol(void *value) {
    symbol *s = (symbol *) value;
    if ((s) && (s->image)) {
        if (s->image->type == AST_TYPE_DATA_WRAPPER)
            ast_destroy_typed_data_wrapper(s->image);
        else
            ast_destroy_non_typed_data_wrapper(s->image);
    }
    // s->name must not be freed here because it's key in hashtable
    // and hashtable owns memory of its keys
}

symtab *symtab_wind_scope(symtab *st) {
    symtab *ret = SYMTAB_MALLOC(sizeof(symtab));

    if (!ret)
        return NULL;

    hashtable *ht = hashtable_create(SYMTAB_SIZE, symtab_destroy_symbol);
    if (!ht) {
        SYMTAB_FREE(ret);
        return NULL;
    }

    ret->symbols = ht;
    ret->parent = st;

    return ret;
}

symtab *symtab_unwind_scope(symtab *st) {
    if (!st)
        return NULL;

    symtab *ret = st->parent;
    hashtable_destroy(st->symbols);
    SYMTAB_FREE(st);

    return ret;
}
