// src/data_structures/include/hashtable.h

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>

typedef struct hashtable hashtable;

typedef enum {
    HASHTABLE_KEY_TYPE_STRING, // keys are owned and strcmp is used
    HASHTABLE_KEY_TYPE_POINTER // keys are not owned and "=" is used for key comparision
} hashtable_key_type;

typedef void (*hashtable_destroy_value_fn_t)(void *value);

// hashtable_destroy_value_fn == NULL means no callback
hashtable *hashtable_create(
    size_t size,
    hashtable_key_type key_type,
    hashtable_destroy_value_fn_t destroy_value_fn );

// frees values too (via callback) if callback
void hashtable_destroy(hashtable *ht);

// returns 0 if one param at least is NULL
// otherwise:
// returns 1 if the key is already in use, 0 otherwise
int hashtable_key_is_in_use(hashtable *ht, const void *key);

// returns NULL if the key is not found, but also if the found entry's value is NULL.
void *hashtable_get(const hashtable *ht, const void *key);

// returns 1 on error, 0 on success.
// error if the key already exists
int hashtable_add(hashtable *ht, const void *key, void *value);

// returns 1 on error, 0 on success.
// error if the key does not exist or ht is NULL
int hashtable_reset_value(hashtable *ht, const void *key, void *value);

// frees value via callback if callback
int hashtable_remove(hashtable *ht, const void *key);

#endif // HASHTABLE_H
