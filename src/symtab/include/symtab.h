// src/symtab/include/symtab.h

// for resolution environment (relative to resolver)

#ifndef SYMTAB_H
#define SYMTAB_H

#include "internal/symtab_internal.h"

#define MAXIMUM_SYMBOL_NAME_LENGTH 255
typedef struct symbol {
    char *name; // owned ; must be not NULL and not exceeding MAXIMUM_SYMBOL_NAME_LENGTH characters
} symbol;

// Returns NULL on error.
// On success, returns a symbol* that owns a strdup of `name`.
// The caller retains ownership of the input string.
symbol *symtab_create_symbol(char *name);

typedef struct symtab symtab;

symtab *symtab_wind_scope(symtab *st); // the caller is responsible for passing either NULL or a well-formed symtab pointer
symtab *symtab_unwind_scope(symtab *st);  // the caller is responsible for passing either NULL or a well-formed symtab pointer

// the caller is responsible for passing either NULL or a well-formed symtab and symbol pointers
// on success (return value 0), symtab is responsible for sym
// on error, the caller is responsible for sym
int symtab_add(symtab *st, symbol *sym);
int symtab_intern_symbol(symtab *st, char *name); // the caller is responsible for st ; do nothing if symbol already interned

symbol *symtab_get_local(symtab *st, const char *name); // the caller is responsible for passing either NULL or a well-formed symtab pointer
int symtab_remove(symtab *st, const char *name); // the caller is responsible for passing either NULL or a well-formed symtab pointer
int symtab_contains_local(symtab *st, const char *name);
symbol *symtab_get(symtab *st, const char *name); // the caller is responsible for passing either NULL or a well-formed symtab pointer
int symtab_contains(symtab *st, const char *name);  // the caller is responsible for passing either NULL or a well-formed symtab pointer

#endif //SYMTAB_H
