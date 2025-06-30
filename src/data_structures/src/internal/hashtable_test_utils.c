// src/data_structures/src/internal/hashtable_test_utils.c

#include "internal/hashtable_test_utils.h"

#include <string.h>

#ifdef UNIT_TEST

hash_djb2_fn hash_djb2_mockable = real_hash_djb2;
unsigned long real_hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    return hash;
}
void set_hash_djb2(hash_djb2_fn f) {
    hash_djb2_mockable = f ? f : real_hash_djb2;
}

hashtable_create_fn hashtable_create_mockable = real_hashtable_create;
hashtable *real_hashtable_create(size_t size, hashtable_destroy_value_fn_t destroy_value_fn) {
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
void set_hashtable_create(hashtable_create_fn f) {
    hashtable_create_mockable = f ? f : real_hashtable_create;
}

static void hashtable_destroy_entry(void *item, void *user_data) {
    entry *e = (entry *)item;
    hashtable_destroy_value_fn_t destroy_fn =
        (hashtable_destroy_value_fn_t) user_data;
    DATA_STRUCTURE_FREE(e->key);
    if (destroy_fn) destroy_fn(e->value);
    DATA_STRUCTURE_FREE(e);
}
hashtable_destroy_fn hashtable_destroy_mockable = real_hashtable_destroy;
void real_hashtable_destroy(hashtable *ht) {
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
void set_hashtable_destroy(hashtable_destroy_fn f) {
    hashtable_destroy_mockable = f ? f : real_hashtable_destroy;
}

#endif
