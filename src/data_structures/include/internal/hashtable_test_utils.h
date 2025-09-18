// src/data_structures/include/internal/hashtable_test_utils.h

#ifndef HASHTABLES_TEST_UTILS_H
#define HASHTABLES_TEST_UTILS_H

#include "internal/hashtable_internal.h"
#include "internal/data_structure_memory_allocator.h"
#include "internal/data_structure_string_utils.h"

typedef unsigned long (*hash_djb2_fn)(const void *key, hashtable_key_type key_type);
unsigned long real_hash_djb2(const void *key, hashtable_key_type key_type);
extern hash_djb2_fn hash_djb2_mockable;
void set_hash_djb2(hash_djb2_fn f);

typedef hashtable *(*hashtable_create_fn)(size_t size, hashtable_key_type key_type, hashtable_destroy_value_fn_t destroy_value_fn);
hashtable *real_hashtable_create(size_t size, hashtable_key_type key_type, hashtable_destroy_value_fn_t destroy_value_fn);
extern hashtable_create_fn hashtable_create_mockable;
void set_hashtable_create(hashtable_create_fn f);

static void hashtable_destroy_entry(void *item, void *user_data);
typedef void (*hashtable_destroy_fn)(hashtable *ht);
void real_hashtable_destroy(hashtable *ht);
extern hashtable_destroy_fn hashtable_destroy_mockable;
void set_hashtable_destroy(hashtable_destroy_fn f);

typedef void *(*hashtable_get_fn)(const hashtable *ht, const void *key);
void *real_hashtable_get(const hashtable *ht, const void *key);
extern hashtable_get_fn hashtable_get_mockable;
void set_hashtable_get(hashtable_get_fn f);

typedef int (*hashtable_add_fn)(hashtable *ht, const void *key, void *value);
int real_hashtable_add(hashtable *ht, const void *key, void *value);
extern hashtable_add_fn hashtable_add_mockable;
void set_hashtable_add(hashtable_add_fn f);

typedef int (*hashtable_reset_value_fn)(hashtable *ht, const void *key, void *value);
int real_hashtable_reset_value(hashtable *ht, const void *key, void *value);
extern hashtable_reset_value_fn hashtable_reset_value_mockable;
void set_hashtable_reset_value(hashtable_reset_value_fn f);

typedef int (*hashtable_remove_fn)(hashtable *ht, const void *key);
int real_hashtable_remove(hashtable *ht, const void *key);
extern hashtable_remove_fn hashtable_remove_mockable;
void set_hashtable_remove(hashtable_remove_fn f);

typedef int (*hashtable_key_is_in_use_fn)(hashtable *ht, const void *key);
int real_hashtable_key_is_in_use(hashtable *ht, const void *key);
extern hashtable_key_is_in_use_fn hashtable_key_is_in_use_mockable;
void set_hashtable_key_is_in_use(hashtable_key_is_in_use_fn f);

#endif //HASHTABLES_TEST_UTILS_H
