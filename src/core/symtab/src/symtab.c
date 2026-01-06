// src/core/symtab/src/symtab.c

#ifdef UNIT_TEST
#include "internal/symtab_test_utils.h"
#endif

#include "internal/symtab_internal.h"
#include "internal/symtab_internal.c"
#include "internal/symtab_memory_allocator.h"
#include "internal/symtab_string_utils.h"

#include <stdlib.h>
#include <string.h>

symtab *symtab_wind_scope(symtab *st) {
    symtab *ret = SYMTAB_MALLOC(sizeof(symtab));

    if (!ret)
        return NULL;

	// Passing NULL as the third parameter ensures that symbols are not destroyed
	// when unwinding scopes. Symbols are owned by runtime_session
    hashtable *ht = hashtable_create(SYMTAB_SIZE, SYMTAB_KEY_TYPE, NULL);

    if (!ht) {
        SYMTAB_FREE(ret);
        return NULL;
    }

    ret->symbols = ht;
    ret->parent = st;

    return ret;
}

symtab *symtab_unwind_scope(symtab *st) {
    if (!st)
        return NULL;

    symtab *ret = st->parent;
    hashtable_destroy(st->symbols);
    SYMTAB_FREE(st);

    return ret;
}

int symtab_intern_symbol(symtab *st, const char *name) {
    if ((!st) || (!(st->symbols)) || (!name) || (strlen(name) > MAXIMUM_SYMBOL_NAME_LENGTH))
        return 1;

    if (!hashtable_key_is_in_use(st->symbols, name)) {
        symbol *sym = SYMTAB_MALLOC(sizeof(symbol));
	    if (!sym)
		    return 1;
        sym->name = SYMTAB_STRING_DUPLICATE(name);
    	if (!sym->name) {
	    	SYMTAB_FREE(sym);
		    return 1;
	    }

        if (hashtable_add(st->symbols, name, sym) == 1) {
            SYMTAB_FREE(sym->name);
            SYMTAB_FREE(sym);
		    return 1;
        }

        return 0;
    }

    return 0;
}

symbol *symtab_get_local(symtab *st, const char *name) {
#ifdef UNIT_TEST
    return symtab_get_local_mockable(st, name);
#else
    if (!st)
        return NULL;

    return hashtable_get(st->symbols, name);
#endif
}

int symtab_remove(symtab *st, const char *name) {
    if (!st)
        return 1;

	return hashtable_remove(st->symbols, name);
}

int symtab_contains_local(symtab *st, const char *name) {
#ifdef UNIT_TEST
    return symtab_contains_local_mockable(st, name);
#else
    if (!st)
        return 0;

    return hashtable_key_is_in_use(st->symbols, name);
#endif
}

symbol *symtab_get(symtab *st, const char *name) {
#ifdef UNIT_TEST
    return symtab_get_mockable(st, name);
#else
    if (!st)
        return NULL;

    if (symtab_contains_local(st, name))
        return symtab_get_local(st, name);

    else
        return symtab_get(st->parent, name);
#endif
}

int symtab_contains(symtab *st, const char *name) {
#ifdef UNIT_TEST
    return symtab_contains_mockable(st, name);
#else
    if (!st)
        return 0;

    if (symtab_contains_local(st, name) == 1)
        return 1;

    return symtab_contains(st->parent, name);
#endif
}
