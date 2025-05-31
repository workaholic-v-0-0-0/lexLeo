// src/data_structures/include/list.h

#ifndef LIST_H
#define LIST_H

// make testable
#ifdef DEBUG
void * list_malloc(size_t size);
void list_free(void * ptr);
#endif // DEBUG

typedef struct cons {void *car; struct cons *cdr;} cons, *list;

//returns NULL on error
list list_push(list l, void * e);

void list_free_list(list l, void (*destroy_fn_t)(void *));

#endif //LIST_H
