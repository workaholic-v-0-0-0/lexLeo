// src/data_structures/include/hashtable.h

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>

typedef struct hashtable hashtable;

typedef void (*hashtable_destroy_value_fn_t)(void *value);

// hashtable_destroy_value_fn == NULL means no callback
hashtable *hashtable_create(size_t size, hashtable_destroy_value_fn_t destroy_value_fn);

// replace if key already exists
int hashtable_add(hashtable *ht, const char *key, void *value);

// return NULL if not found
void *hashtable_get(const hashtable *ht, const char *key);

// frees value via callback if callback
int hashtable_remove(hashtable *ht, const char *key);

// frees values too (via callback) if callback
void hashtable_destroy(hashtable *ht);

#endif // HASHTABLE_H
