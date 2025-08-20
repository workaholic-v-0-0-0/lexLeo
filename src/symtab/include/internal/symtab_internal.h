// src/symtab/include/internal/symtab_internal.h

#ifndef SYMTAB_INTERNAL_H
#define SYMTAB_INTERNAL_H

#include "symtab.h"

#include "hashtable.h"

#define SYMTAB_SIZE 256

// list of symbol tables, one per scope ;
// the root hashtable is the global scope where
// for example resolver intern symbols with NULL
// value from symbol_name of ast provided by the
// ast module
typedef struct symtab {
    hashtable *symbols;
    struct symtab *parent;
} symtab;

#endif //SYMTAB_INTERNAL_H
