// src/data_structures/src/list.c

#include "list.h"

#include <stddef.h>
#include <stdlib.h>

#include "internal/data_structure_memory_allocator.h"
#include "logger.h"

#ifdef UNIT_TEST
#include "internal/list_test_utils.h"
#endif

list list_push(list l, void * e) {
#ifdef UNIT_TEST
	return list_push_mockable(l, e);
#else
	if (!e)
		return (list) NULL;

	list ret = DATA_STRUCTURE_MALLOC(sizeof(cons));
    if (!ret)
        return (list) NULL;

    ret->car = e;
    ret->cdr = l;

    return ret;
#endif
}

void *list_pop(list *l_p) {
#ifdef UNIT_TEST
	return list_pop_mockable(l_p);
#else
	if ((!l_p) || (!*l_p))
		return NULL;

	void *ret = (*l_p)->car;
	list cons_to_be_freed_p = *l_p;
	*l_p = (*l_p)->cdr;
	DATA_STRUCTURE_FREE(cons_to_be_freed_p);

	return ret;
#endif
}

void list_free_list(list l, void (*destroy_fn)(void *item, void *user_data), void *user_data) {
	list next;
	while (l) {
		if (destroy_fn)
			destroy_fn(l->car, user_data);
		next = l->cdr;
		DATA_STRUCTURE_FREE(l);
		l = next;
	}
}
