// src/data_structures/include/list.h

#ifndef LIST_H
#define LIST_H

typedef struct cons {void *car; struct cons *cdr;} cons, *list;

//returns NULL on error
list list_push(list l, void * e);

// precondition : if destroy_fn_t non null, it must properly frees the l'cars
void list_free_list(list l, void (*destroy_fn_t)(void *item, void *user_data), void *user_data);

#endif //LIST_H
