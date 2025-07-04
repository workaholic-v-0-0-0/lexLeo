// src/data_structures/src/list.c

#include "list.h"

#include <stddef.h>
#include <stdlib.h>

#include "internal/data_structure_memory_allocator.h"
#include "logger.h"

list list_push(list l, void * e) {
	if (!e)
		return (list) NULL;

	list ret = DATA_STRUCTURE_MALLOC(sizeof(cons));
    if (!ret)
        return (list) NULL;

    ret->car = e;
    ret->cdr = l;

    return ret;
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
