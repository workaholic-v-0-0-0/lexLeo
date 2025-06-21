// src/symtab/include/internal/symtab_internal.h

#ifndef SYMTAB_INTERNAL_H
#define SYMTAB_INTERNAL_H

struct symtab {
    hashtable *scopes[SYMTAB_MAX_DEPTH];
    int current_scope;
};

#endif //SYMTAB_INTERNAL_H
