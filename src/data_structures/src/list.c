// src/data_structures/src/list.c

#include <stddef.h>
#include <stdlib.h>

#include "logger.h"

#include "list.h"

list list_push(list l, void * e) {
	if (!e)
		return (list) NULL;

	list ret = list_malloc(sizeof(cons));
    if (!ret)
        return (list) NULL;

    ret->car = e;
    ret->cdr = l;

    return ret;
}

void list_free_list(list l, void (*destroy_fn_t)(void *)) {
    if (!l)
        return;
    if (destroy_fn_t) destroy_fn_t(l->car);
    list_free(l);
    return;
}
