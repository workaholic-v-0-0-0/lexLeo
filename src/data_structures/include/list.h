// src/data_structures/include/list.h

#ifndef LIST_H
#define LIST_H

typedef struct cons {void *car; struct cons *cdr;} cons, *list;

void set_allocators(void *(*alloc)(size_t), void (*fr)(void *));

//returns NULL on error
list list_push(list l, void * e);

void list_free_list(list l, void (*destroy_fn_t)(void *));

#endif //LIST_H
