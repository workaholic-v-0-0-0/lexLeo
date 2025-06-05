// src/data_structures/src/hashtable.c

#include "internal/hashtable_internal.h"

#include "internal/data_structure_memory_allocator.h"

#include <stdlib.h>
#include <string.h>

static unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash;
}

hashtable *hashtable_create(size_t size, hashtable_destroy_value_fn_t destroy_value_fn) {
    if (size == 0)
        return NULL;

    hashtable *ret = DATA_STRUCTURE_MALLOC(sizeof(hashtable));
    if (!ret)
        return NULL;

    ret->buckets = DATA_STRUCTURE_MALLOC(size * sizeof(list));
    if (!ret->buckets) {
        DATA_STRUCTURE_FREE(ret);
        return NULL;
    }
    memset(ret->buckets, 0, size * sizeof(list));

    ret->size = size;
    ret->destroy_value_fn = destroy_value_fn;

    return ret;
}

void hashtable_destroy(hashtable *ht) {
    if (!ht)
        return;
}