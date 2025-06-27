// src/symtab/src/symtab.c

#include "internal/symtab_internal.h"

#include "internal/symtab_memory_allocator.h"

#include <stdlib.h>

symtab *symtab_create(void) { // CURRENTLY IMPLEMENED VIA TDD ; PENDING FULL AST IMPLEMENTATION
    symtab *ret = SYMTAB_MALLOC(sizeof(symtab));
    if (!ret)
        return NULL;
    return (symtab *)-2;
}
