// src/data_structures/include/hashtable.h

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>

typedef struct hashtable hashtable;

typedef void (*hashtable_destroy_value_fn_t)(void *value);

// hashtable_destroy_value_fn == NULL means no callback
hashtable *hashtable_create(size_t size, hashtable_destroy_value_fn_t destroy_value_fn);

// frees values too (via callback) if callback
void hashtable_destroy(hashtable *ht);

// returns 1 if the key is already in use, 0 otherwise
int hashtable_key_is_in_use(hashtable *ht, const char *key);

// returns NULL if the key is not found, but also if the found entry's value is NULL.
void *hashtable_get(const hashtable *ht, const char *key);

// returns 1 on error, 0 on success.
// error if the key already exists
int hashtable_add(hashtable *ht, const char *key, void *value);

// returns 1 on error, 0 on success.
// error if the key does not exist
int hashtable_reset_value(hashtable *ht, const char *key, void *value);

// frees value via callback if callback
int hashtable_remove(hashtable *ht, const char *key);

#endif // HASHTABLE_H
