// src/data_structures/src/hashtable.c

#include "internal/hashtable_internal.h"

#include "internal/data_structure_memory_allocator.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h> // debug

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
        ret = NULL;
        return NULL;
    }
    memset(ret->buckets, 0, size * sizeof(list));

    ret->size = size;
    ret->destroy_value_fn = destroy_value_fn;

    return ret;
}


// pb: destroy_fn_t does not know how to destroy entry cause it depends on how to destroy entry.value
// we need a fct which map a fct destroying an entity of type t to a fct destroying an entry type with value field of type t
static void hashtable_destroy_entry(entry * e, void (*destroy_fn_t)(void *)) {
    DATA_STRUCTURE_FREE(e->key);
    if (destroy_value_fn) destroy_value_fn(e->value);
}

// precondition: ht null or correctly initialized
void hashtable_destroy(hashtable *ht) {
    if (!ht)
        return;
    for (size_t i = 0 ; i < ht->size ; i++) {
        if ((ht->buckets)[i]) {
            list_free_list(
                (ht->buckets)[i],
                hashtable_destroy_entry
            );

/*
            DATA_STRUCTURE_FREE(((entry *) ((ht->buckets)[i])->car)->key);
            if (ht->destroy_value_fn)
                (ht->destroy_value_fn)(((entry *) ((ht->buckets)[i])->car)->value);
            DATA_STRUCTURE_FREE(((ht->buckets)[i])->car);
            DATA_STRUCTURE_FREE((ht->buckets)[i]);
*/
        }
    }
	DATA_STRUCTURE_FREE(ht->buckets);
	ht->buckets = NULL;
	DATA_STRUCTURE_FREE(ht);
	ht = NULL;
}
