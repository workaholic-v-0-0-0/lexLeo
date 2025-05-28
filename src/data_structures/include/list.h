// src/data_structures/include/list.h

#ifndef LIST_H
#define LIST_H

// make testable
#ifdef DEBUG
void * list_malloc(size_t size);
#endif // DEBUG

typedef struct cons {void *car; struct cons *cdr;} cons, *list;

//returns NULL on error
list list_push(list l, void * e);

int list_free(list l);

#endif //LIST_H
