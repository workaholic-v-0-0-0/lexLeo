// src/data_structures/src/internal/list_test_utils.c

#ifdef UNIT_TEST

#include "internal/list_test_utils.h"

#include <string.h>

list_push_fn list_push_mockable = real_list_push;
list real_list_push(list l, void *e) {
    if (!e)
        return (list) NULL;

    list ret = DATA_STRUCTURE_MALLOC(sizeof(cons));
    if (!ret)
        return (list) NULL;

    ret->car = e;
    ret->cdr = l;

    return ret;
}
void set_list_push(list_push_fn f) {
    list_push_mockable = f ? f : real_list_push;
}
list_push_fn get_list_push() {
    return list_push_mockable;
}

list_pop_fn list_pop_mockable = real_list_pop;
void *real_list_pop(list *l_p) {
	if ((!l_p) || (!*l_p))
		return NULL;

	void *ret = (*l_p)->car;
	list cons_to_be_freed_p = *l_p;
	*l_p = (*l_p)->cdr;
	DATA_STRUCTURE_FREE(cons_to_be_freed_p);

	return ret;
}
void set_list_pop(list_pop_fn f) {
    list_pop_mockable = f ? f : real_list_pop;
}

#endif