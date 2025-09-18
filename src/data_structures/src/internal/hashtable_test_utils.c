// src/data_structures/src/internal/hashtable_test_utils.c

#ifdef UNIT_TEST

#include <string.h>
#include <stdint.h>

#include "internal/hashtable_test_utils.h"

hash_djb2_fn hash_djb2_mockable = real_hash_djb2;
unsigned long real_hash_djb2(const void *key, hashtable_key_type key_type) {
    if (!key)
        return (unsigned long)0;

    if (key_type == HASHTABLE_KEY_TYPE_STRING) {
        const unsigned char *str = (const unsigned char *)key;
        unsigned long hash = 5381;
        int c;
        while ((c = *str++))
            hash = ((hash << 5) + hash) + c;
        return hash;
    } else {
        return (unsigned long)(uintptr_t)key;
    }
}
void set_hash_djb2(hash_djb2_fn f) {
    hash_djb2_mockable = f ? f : real_hash_djb2;
}

hashtable_create_fn hashtable_create_mockable = real_hashtable_create;
hashtable *real_hashtable_create(size_t size, hashtable_key_type key_type, hashtable_destroy_value_fn_t destroy_value_fn) {
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
    ret->key_type = key_type;
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

hashtable_get_fn hashtable_get_mockable = real_hashtable_get;
void *real_hashtable_get(const hashtable *ht, const void *key) {
    if ((!ht) || (!key))
        return NULL;
    list bucket = (ht->buckets)[hash_djb2(key, ht->key_type) % ht->size];


    if (ht->key_type == HASHTABLE_KEY_TYPE_STRING) {
        while (bucket) {
            if (
                    DATA_STRUCTURE_STRING_COMPARE(
                        key,
                        ((entry *) (bucket->car))->key )
                        ==
                        0 )
                return ((entry *) (bucket->car))->value;
            bucket = bucket->cdr;
        }

    } else if (ht->key_type == HASHTABLE_KEY_TYPE_POINTER) {
        while (bucket) {
            if (key == ((entry *) (bucket->car))->key)
                return ((entry *) (bucket->car))->value;
            bucket = bucket->cdr;
        }
    }

    return NULL;
}
void set_hashtable_get(hashtable_get_fn f) {
    hashtable_get_mockable = f ? f : real_hashtable_get;
}

hashtable_add_fn hashtable_add_mockable = real_hashtable_add;
int real_hashtable_add(hashtable *ht, const void *key, void *value) {
    if ((!ht) || (!key) || (hashtable_key_is_in_use(ht, key)))
        return 1;

    entry *new_entry = DATA_STRUCTURE_MALLOC(sizeof(entry));
    if (!new_entry)
        return 1;

    new_entry->key = ((ht->key_type == HASHTABLE_KEY_TYPE_STRING)) ?
        (void *) DATA_STRUCTURE_STRING_DUPLICATE((const char *) key)
        :
        (void *) key;
    if (!new_entry->key) {
        DATA_STRUCTURE_FREE(new_entry);
        return 1;
    }

    new_entry->value = value;

    cons *c = DATA_STRUCTURE_MALLOC(sizeof(cons));
    if (!c) {
        if (ht->key_type == HASHTABLE_KEY_TYPE_STRING)
            DATA_STRUCTURE_FREE(new_entry->key);
        DATA_STRUCTURE_FREE(new_entry);
        return 1;
    }

    c->car = new_entry;

    list *bucket = &((ht->buckets)[hash_djb2(key, ht->key_type) % ht->size]);
    c->cdr = *bucket;
    *bucket = c;

    return 0;
}
void set_hashtable_add(hashtable_add_fn f) {
    hashtable_add_mockable = f ? f : real_hashtable_add;
}

hashtable_reset_value_fn hashtable_reset_value_mockable = real_hashtable_reset_value;
int real_hashtable_reset_value(hashtable *ht, const void *key, void *value) {
    if (!ht || !key || ht->size == 0)
        return 1;

    list bucket = (ht->buckets)[hash_djb2(key, ht->key_type) % ht->size];
    entry *entry_p = NULL;

    if (ht->key_type == HASHTABLE_KEY_TYPE_STRING) {
        while (bucket) {
            if (
                    DATA_STRUCTURE_STRING_COMPARE(
                        key,
                        ((entry *) (bucket->car))->key )
                        ==
                        0 )
                entry_p = (entry *) (bucket->car);
            bucket = bucket->cdr;
        }

    } else if (ht->key_type == HASHTABLE_KEY_TYPE_POINTER) {
        while (bucket) {
            if (key == ((entry *) (bucket->car))->key)
                entry_p = (entry *) (bucket->car);
            bucket = bucket->cdr;
        }
    }

    if (!entry_p)
        return 1;

    if (ht->destroy_value_fn)
        ht->destroy_value_fn(entry_p->value);

    entry_p->value = value;

    return 0;
}
void set_hashtable_reset_value(hashtable_reset_value_fn f) {
    hashtable_reset_value_mockable = f ? f : real_hashtable_reset_value;
}

hashtable_remove_fn hashtable_remove_mockable = real_hashtable_remove;
int real_hashtable_remove(hashtable *ht, const void *key) {
    if (!ht || !key || ht->size == 0) {
		return 1;
	}

    // find the cons cell to be removed ;
    // if it's not the first, keep track of the previous cell
    // (before_to_be_removed) to update the bucket list after removal
    size_t index = hash_djb2(key, ht->key_type) % ht->size;
    list bucket = (ht->buckets)[index];
    if (!bucket)
        return 1;

    list before_to_be_removed = NULL;

    if ( // not the first element of the bucket
            (
                (ht->key_type == HASHTABLE_KEY_TYPE_STRING)
                &&
                (DATA_STRUCTURE_STRING_COMPARE(
                    ((entry*) (bucket->car))->key,
                    key) != 0 ) )
            ||
            (
                (ht->key_type == HASHTABLE_KEY_TYPE_POINTER)
                &&
                (((entry*) (bucket->car))->key != key) ) ) {

        before_to_be_removed = bucket;
        if (!(bucket = bucket->cdr))
            return 1;

        if (ht->key_type == HASHTABLE_KEY_TYPE_STRING) {
            while (
                    DATA_STRUCTURE_STRING_COMPARE(
                        ((entry*)(bucket->car))->key,
                        key )
                        !=
                        0 ) {
                before_to_be_removed = bucket;
                if (!(bucket = bucket->cdr))
                    return 1;
            }

        } else if (ht->key_type == HASHTABLE_KEY_TYPE_POINTER) {
            while (((entry*)(bucket->car))->key != key) {
                before_to_be_removed = bucket;
                if (!(bucket = bucket->cdr))
                    return 1;
            }
        }
    }

    // update the bucket list to remove the targeted cons cell
    if (!before_to_be_removed)
        (ht->buckets)[index] = bucket->cdr; // removed head of the list
    else
        before_to_be_removed->cdr = bucket->cdr; // bypass the removed cell

    // cleanup of all dynamically allocated memory associated with the
    // entry to be removed
    if (ht->destroy_value_fn)
        ht->destroy_value_fn(((entry *) (bucket->car))->value);
    if (ht->key_type == HASHTABLE_KEY_TYPE_STRING)
        DATA_STRUCTURE_FREE(((entry *) (bucket->car))->key);
    DATA_STRUCTURE_FREE(bucket->car);
    DATA_STRUCTURE_FREE(bucket);

    return 0;
}
void set_hashtable_remove(hashtable_remove_fn f) {
    hashtable_remove_mockable = f ? f : real_hashtable_remove;
}

hashtable_key_is_in_use_fn hashtable_key_is_in_use_mockable = real_hashtable_key_is_in_use;
int real_hashtable_key_is_in_use(hashtable *ht, const void *key) {
    if ((!ht) || (!key))
        return 0;

    list bucket = (ht->buckets)[hash_djb2(key, ht->key_type) % ht->size];

    if (ht->key_type == HASHTABLE_KEY_TYPE_STRING) {
        while (bucket) {
            if (
                    DATA_STRUCTURE_STRING_COMPARE(
                        key,
                        ((entry *) (bucket->car))->key )
                        ==
                        0 )
                return 1;
            bucket = bucket->cdr;
        }
    } else if (ht->key_type == HASHTABLE_KEY_TYPE_POINTER) {
        while (bucket) {
            if (key == ((entry *) (bucket->car))->key)
                return 1;
            bucket = bucket->cdr;
        }
    }
    return 0;
}
void set_hashtable_key_is_in_use(hashtable_key_is_in_use_fn f) {
    hashtable_key_is_in_use_mockable = f ? f : real_hashtable_key_is_in_use;
}

#endif
