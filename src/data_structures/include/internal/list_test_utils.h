// src/data_structures/include/internal/list_test_utils.h

#ifndef LEXLEO_LIST_TEST_UTILS_H
#define LEXLEO_LIST_TEST_UTILS_H

#include "list.h"
#include "internal/data_structure_memory_allocator.h"

typedef list (*list_push_fn)(list l, void *e);
list real_list_push(list l, void *e);
extern list_push_fn list_push_mockable;
void set_list_push(list_push_fn f);
list_push_fn get_list_push(void);

typedef void *(*list_pop_fn)(list *l_p);
void *real_list_pop(list *l_p);
extern list_pop_fn list_pop_mockable;
void set_list_pop(list_pop_fn f);

#endif //LEXLEO_LIST_TEST_UTILS_H
