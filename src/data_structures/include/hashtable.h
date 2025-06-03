// src/data_structures/include/hashtable.h

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>

typedef struct hashtable hashtable;

typedef void (*hashtable_destroy_value_t)(void *value);

// hashtable_destroy_value == NULL means no callback
hashtable *hashtable_create(size_t size, hashtable_destroy_value_t hashtable_destroy_value);

// replace if key already exists
int hashtable_add(hashtable *ht, const char *key, void *value);

// return NULL if not found
void *hashtable_get(const hashtable *ht, const char *key);

// frees value if callback
int hashtable_remove(hashtable *ht, const char *key);

// frees values too if callback
void hashtable_destroy(hashtable *ht);

#endif // HASHTABLE_H
