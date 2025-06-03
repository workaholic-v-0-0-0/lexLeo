// src/data_structures/src/hashtable.c

#include "hashtable.h"

#include "internal/data_structure_memory_allocator.h"

typedef struct entry {
    char *key;
    void *value;
    struct entry *next;
} entry;

struct hashtable {
    entry **buckets;
    size_t size;
    hashtable_destroy_value_t destroy_value;
};

static unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash;
}

hashtable *hashtable_create(size_t size, hashtable_destroy_value_t hashtable_destroy_value) {
    if (size == 0) return NULL;
    return (void *) -2;
}