// src/data_structures/include/list.h

#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdbool.h>

typedef struct cons {void *car; struct cons *cdr;} cons, *list;

//returns NULL on error and if so no side effect
list list_push(list l, void * e);

void *list_pop(list *l_p);

// precondition : if destroy_fn_t non null, it must properly frees the l'cars
void list_free_list(list l, void (*destroy_fn_t)(void *item, void *user_data), void *user_data);

bool list_contains(list l, void *item);

// DEBUG TOOLS

size_t list_length(list l);

#endif //LIST_H
