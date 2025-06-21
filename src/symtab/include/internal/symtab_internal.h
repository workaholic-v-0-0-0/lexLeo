// src/symtab/include/internal/symtab_internal.h

#ifndef SYMTAB_INTERNAL_H
#define SYMTAB_INTERNAL_H

#include "symtab.h"

#include "hashtable.h"

#define SYMTAB_SIZE 256

//  list of symbol tables, one per scope
typedef struct symtab {
    hashtable *symbols;
    struct symtab *parent;
} symtab, *symtab_stack;

#endif //SYMTAB_INTERNAL_H
