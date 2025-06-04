// src/data_structures/include/internal/hashtable_internal.h

#ifndef HASHTABLE_INTERNAL_H
#define HASHTABLE_INTERNAL_H

#include "hashtable.h"

typedef struct entry {
    char *key;
    void *value;
    struct entry *next;
} entry;

struct hashtable {
    entry **buckets;
    size_t size;
    hashtable_destroy_value_fn_t destroy_value_fn;
};

static unsigned long hash_djb2(const char *str);

#endif //HASHTABLE_INTERNAL_H
