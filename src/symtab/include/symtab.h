// src/symtab/include/symtab.h

#ifndef SYMTAB_H
#define SYMTAB_H

typedef struct symbol {
    char *name;
    void *image;
} symbol;
// note: in hashtable, an entry with key = symbol.name and *value = symbol

typedef struct symtab symtab;

symtab *symtab_create(void);
void symtab_destroy(symtab *st);

void symtab_wind_scope(symtab *st);
void symtab_unwind_scope(symtab *st);

int symtab_add(symtab *st, symbol *sym);
symbol *symtab_lookup(symtab *st, const char *name);

#endif //SYMTAB_H
