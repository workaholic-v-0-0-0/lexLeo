// src/symtab/include/internal/symtab_test_utils.h

#ifndef SYMTAB_TEST_UTILS_H
#define SYMTAB_TEST_UTILS_H

#include "internal/symtab_internal.h"

typedef symbol *(*symtab_get_local_fn)(symtab *st, const char *name);
symbol *real_symtab_get_local(symtab *st, const char *name);
extern symtab_get_local_fn symtab_get_local_mockable;
void set_symtab_get_local(symtab_get_local_fn f);

typedef int (*symtab_contains_local_fn)(symtab *st, const char *name);
int real_symtab_contains_local(symtab *st, const char *name);
extern symtab_contains_local_fn symtab_contains_local_mockable;
void set_symtab_contains_local(symtab_contains_local_fn f);

typedef symbol *(*symtab_get_fn)(symtab *st, const char *name);
symbol *real_symtab_get(symtab *st, const char *name);
extern symtab_get_fn symtab_get_mockable;
void set_symtab_get(symtab_get_fn f);

#endif //SYMTAB_TEST_UTILS_H
