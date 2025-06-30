// src/symtab/include/symtab.h

#ifndef SYMTAB_H
#define SYMTAB_H

// forward declaration to handle cross-dependency
typedef struct ast ast;
// note: symtab.c will need '#include "ast.h"'

typedef struct symbol {
    char *name;
    ast *image; // must be NULL or correctly formed
} symbol;
// note: registrer in hashtable with an entry with
//       key = symbol.name and *value = symbol

typedef struct symtab symtab;

// for info :
// typedef void (*hashtable_destroy_value_fn_t)(void *value);
void symtab_destroy_symbol(void *value);

symtab *symtab_create(void);
void symtab_destroy(symtab *st);

void symtab_wind_scope(symtab *st);
void symtab_unwind_scope(symtab *st);

int symtab_add(symtab *st, symbol *sym);
symbol *symtab_lookup(symtab *st, const char *name);

#endif //SYMTAB_H
