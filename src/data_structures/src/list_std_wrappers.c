// src/data_structures/src/list_std_wrappers.c

#include <stdlib.h>

void * list_malloc(size_t size) {
    return malloc(size);
}

void list_free(void * ptr) {
    free(ptr);
}
