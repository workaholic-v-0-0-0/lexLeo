// src/data_structures/include/list.h

#ifndef LIST_H
#define LIST_H

typedef struct cons {void *car; struct cons *cdr;} cons, *list;

void set_allocators(void *(*alloc)(size_t), void (*fr)(void *));
void *get_current_free();

//returns NULL on error
list list_push(list l, void * e);

// precondition : if destroy_fn_t non null, it must properly frees the l'cars
void list_free_list(list l, void (*destroy_fn_t)(void *));

#endif //LIST_H
