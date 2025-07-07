// src/symtab/include/symtab.h

#ifndef SYMTAB_H
#define SYMTAB_H

#include "internal/symtab_internal.h"

// forward declaration to handle cross-dependency
typedef struct ast ast;
// note: '#include "ast.h"' in symtab.c

typedef struct symbol {
    char *name;
    ast *image; // must be NULL or correctly formed
} symbol;
// note: registrer in hashtable with an entry with
//       key = symbol.name and *value = symbol

typedef struct symtab symtab;

// for info:
// typedef void (*hashtable_destroy_value_fn_t)(void *value);
void symtab_destroy_symbol(void *value);

symtab *symtab_wind_scope(symtab *st); // the caller is responsible for passing either NULL or a well-formed symtab pointer
symtab *symtab_unwind_scope(symtab *st);  // the caller is responsible for passing either NULL or a well-formed symtab pointer

int symtab_add(symtab *st, symbol *sym); // the caller is responsible for passing either NULL or a well-formed symtab and symbol pointers
symbol *symtab_get_local(symtab *st, const char *name); // the caller is responsible for passing either NULL or a well-formed symtab pointer
int symtab_reset_local(symtab *st, const char *name, ast *image);// the caller is responsible for passing either NULL or a well-formed symtab and ast pointers
int symtab_remove(symtab *st, const char *name); // the caller is responsible for passing either NULL or a well-formed symtab pointer
int symtab_contains_local(symtab *st, const char *name);
symbol *symtab_get(symtab *st, const char *name); // the caller is responsible for passing either NULL or a well-formed symtab pointer
int symtab_reset(symtab *st, const char *name, ast *image);// the caller is responsible for passing either NULL or a well-formed symtab and ast pointers
int symtab_contains(symtab *st, const char *name);

#endif //SYMTAB_H
