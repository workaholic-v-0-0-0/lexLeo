// src/data_structures/include/internal/hashtable_internal.h

#ifndef HASHTABLE_INTERNAL_H
#define HASHTABLE_INTERNAL_H

#include "hashtable.h"
#include "list.h"

typedef struct entry {
    void *key;
    void *value;
} entry;

struct hashtable {
    hashtable_key_type key_type;
    list *buckets;
    size_t size;
    hashtable_destroy_value_fn_t destroy_value_fn; // eg a fct that checks
         // ref_nb to know if an interpreter environment must be destroyed
};

#ifndef UNIT_TEST
static
#endif
unsigned long hash_djb2(const void *key, hashtable_key_type key_type);

#endif //HASHTABLE_INTERNAL_H
