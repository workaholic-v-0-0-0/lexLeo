// src/data_structures/src/hashtable.c

#include "internal/hashtable_internal.h"

#include "internal/data_structure_memory_allocator.h"
#include "internal/data_structure_string_utils.h"

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

int hashtable_key_is_in_use(hashtable *ht, const char *key) {
    if (!ht)
        return 0;
    list bucket = (ht->buckets)[hash_djb2(key) % ht->size];
    while (bucket) {
        if (DATA_STRUCTURE_STRING_COMPARE(key, ((entry *) (bucket->car))->key) == 0)
            return 1;
        bucket = bucket->cdr;
    }
    return 0;
}

void *hashtable_get(const hashtable *ht, const char *key) {
    if (!ht)
        return NULL;
    list bucket = (ht->buckets)[hash_djb2(key) % ht->size];
    while (bucket) {
        if (DATA_STRUCTURE_STRING_COMPARE(key, ((entry *) (bucket->car))->key) == 0)
            return ((entry *) (bucket->car))->value;
        bucket = bucket->cdr;
    }
    return NULL;
}

static void hashtable_destroy_entry(void *item, void *user_data) {
    entry *e = (entry *)item;
    hashtable_destroy_value_fn_t destroy_fn = (hashtable_destroy_value_fn_t) user_data;
    DATA_STRUCTURE_FREE(e->key);
    if (destroy_fn) destroy_fn(e->value);
    DATA_STRUCTURE_FREE(e);
}

// precondition: ht null or correctly initialized
void hashtable_destroy(hashtable *ht) {
    if (!ht)
        return;
    for (size_t i = 0 ; i < ht->size ; i++) {
        if ((ht->buckets)[i]) {
            // list_free_list will call hashtable_destroy_entry with
            // ht->destroy_value_fn for second argument in order to properly
            // destroy the value field of each entries
            list_free_list(
                (ht->buckets)[i],
                hashtable_destroy_entry,
                ht->destroy_value_fn
            );
        }
    }
	DATA_STRUCTURE_FREE(ht->buckets);
	ht->buckets = NULL;
	DATA_STRUCTURE_FREE(ht);
	ht = NULL;
}

int hashtable_add(hashtable *ht, const char *key, void *value) {
    if (!ht)
        return 1;

    if (!key)
        return 1;

    if (hashtable_key_is_in_use(ht, key))
        return 1;
    entry *new_entry = DATA_STRUCTURE_MALLOC(sizeof(entry));
    if (!new_entry)
        return 1;

    new_entry->key = DATA_STRUCTURE_STRING_DUPLICATE(key);
    if (!new_entry->key) {
        DATA_STRUCTURE_FREE(new_entry);
        return 1;
    }

    new_entry->value = value;

    cons *c = DATA_STRUCTURE_MALLOC(sizeof(cons));
    if (!c) {
        DATA_STRUCTURE_FREE(new_entry->key);
        DATA_STRUCTURE_FREE(new_entry);
        return 1;
    }

    c->car = new_entry;

    list *bucket = &((ht->buckets)[hash_djb2(key) % ht->size]);
    c->cdr = *bucket;
    *bucket = c;

    return 0;
}
