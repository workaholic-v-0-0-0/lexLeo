// bison/data_structures/include/internal/hashtable_internal.h

#ifndef HASHTABLE_INTERNAL_H
#define HASHTABLE_INTERNAL_H

#include "hashtable.h"

#include "list.h"

typedef struct entry {
    char *key;
    void *value;
} entry;
// Note: The 'key' field in 'entry' is defined as 'char *' instead of 'const char *'
// because the hashtable internally duplicates the provided key (using strdup)
// and is responsible for freeing it when the entry is removed or destroyed.
// This prevents accidental double-free or use-after-free bugs
// and ensures the hash table fully owns the memory for each key.

struct hashtable {
    list *buckets;
    size_t size;
    hashtable_destroy_value_fn_t destroy_value_fn;
};

#ifndef UNIT_TEST
static
#endif
unsigned long hash_djb2(const char *str);

#endif //HASHTABLE_INTERNAL_H
