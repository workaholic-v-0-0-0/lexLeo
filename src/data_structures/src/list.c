// src/data_structures/src/list.c

#include <stddef.h>
#include <stdlib.h>

#include "logger.h"

#include "list.h"

// make testable with dependency injection
static void *(*current_malloc)(size_t) = malloc;
static void (*current_free)(void *) = free;
void set_allocators(void *(*alloc)(size_t), void (*fr)(void *)) {
    current_malloc = alloc ? alloc : malloc;
    current_free = fr ? fr : free;
}
void *get_current_free() {
	return current_free;
}

list list_push(list l, void * e) {
	if (!e)
		return (list) NULL;

	list ret = current_malloc(sizeof(cons));
    if (!ret)
        return (list) NULL;

    ret->car = e;
    ret->cdr = l;

    return ret;
}

void list_free_list(list l, void (*destroy_fn_t)(void *)) {
	list next;
	while (l) {
		if (destroy_fn_t) {
			destroy_fn_t(l->car);
			l->car = NULL;
		}
		next = l->cdr;
		current_free(l);
		l = NULL;
		l = next;
	}
}
