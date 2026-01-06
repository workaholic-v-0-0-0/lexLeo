// src/core/symtab/include/internal/symtab_internal.h

#ifndef SYMTAB_INTERNAL_H
#define SYMTAB_INTERNAL_H

#include "symtab.h"

#include "hashtable.h"

#define SYMTAB_SIZE 256

extern const hashtable_key_type SYMTAB_KEY_TYPE;

typedef struct symtab {
    hashtable *symbols;
    struct symtab *parent;
} symtab;

#endif //SYMTAB_INTERNAL_H
