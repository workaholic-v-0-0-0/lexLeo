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

void list_free_list(list l, void (*destroy_fn_t)(void *)) {
	list next;
	while (l) {
		if (destroy_fn_t)
			destroy_fn_t(l->car);
		next = l->cdr;
		DATA_STRUCTURE_FREE(l);
		l = next;
	}
}
