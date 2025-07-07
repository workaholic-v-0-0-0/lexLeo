// src/symtab/src/internal/symtab_test_utils.c

#ifdef UNIT_TEST

#include "internal/symtab_test_utils.h"

symtab_get_local_fn symtab_get_local_mockable = real_symtab_get_local;
symbol *real_symtab_get_local(symtab *st, const char *name) {
    if (!st)
        return NULL;

    return hashtable_get(st->symbols, name);
}
void set_symtab_get_local(symtab_get_local_fn f) {
    symtab_get_local_mockable = f ? f : real_symtab_get_local;
}

symtab_contains_local_fn symtab_contains_local_mockable = real_symtab_contains_local;
int real_symtab_contains_local(symtab *st, const char *name) {
    if (!st)
        return 0;

    return hashtable_key_is_in_use(st->symbols, name);
}
void set_symtab_contains_local(symtab_contains_local_fn f) {
    symtab_contains_local_mockable = f ? f : real_symtab_contains_local;
}

symtab_get_fn symtab_get_mockable = real_symtab_get;
symbol *real_symtab_get(symtab *st, const char *name) {
    if (!st)
        return NULL;

    if (symtab_contains_local(st, name))
        return symtab_get_local(st, name);

    else
        return symtab_get(st->parent, name);
}
void set_symtab_get(symtab_get_fn f) {
    symtab_get_mockable = f ? f : real_symtab_get;
}

symtab_reset_local_fn symtab_reset_local_mockable = real_symtab_reset_local;
int real_symtab_reset_local(symtab *st, const char *name, ast *image) {
    if (!st)
        return 1;

    return hashtable_reset_value(st->symbols, name, (void *) image);
}
void set_symtab_reset_local(symtab_reset_local_fn f) {
    symtab_reset_local_mockable = f ? f : real_symtab_reset_local;
}

symtab_reset_fn symtab_reset_mockable = real_symtab_reset;
int real_symtab_reset(symtab *st, const char *name, ast *image) {
    if (!st)
        return 1;

    if (symtab_reset_local(st, name, image) == 0)
        return 0;

    return symtab_reset(st->parent, name, image);
}
void set_symtab_reset(symtab_reset_fn f) {
    symtab_reset_mockable = f ? f : real_symtab_reset;
}

symtab_contains_fn symtab_contains_mockable = real_symtab_contains;
int real_symtab_contains(symtab *st, const char *name) {
    if (!st)
        return 0;

    if (symtab_contains_local(st, name) == 1)
        return 1;

    return symtab_contains(st->parent, name);
}
void set_symtab_contains(symtab_contains_fn f) {
    symtab_contains_mockable = f ? f : real_symtab_contains;
}

#endif
