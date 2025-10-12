// src/runtime_env/include/internal/runtime_env_ctx.h

#ifndef LEXLEO_RUNTIME_ENV_CTX_H
#define LEXLEO_RUNTIME_ENV_CTX_H

#include <stdbool.h>

typedef struct hashtable hashtable;

typedef hashtable *(*create_bindings_fn_t)(
    size_t size,
    hashtable_key_type key_type,
    hashtable_destroy_value_fn_t destroy_value_fn );
typedef void (*destroy_bindings_fn_t)(hashtable *ht);
typedef int (*hashtable_key_is_in_use_fn_t)(hashtable *ht, const void *key);
typedef void *(*hashtable_get_fn_t)(const hashtable *ht, const void *key);
typedef int (*hashtable_add_fn_t)(hashtable *ht, const void *key, void *value);
typedef int (*hashtable_reset_value_fn_t)(hashtable *ht, const void *key, void *value);
typedef int (*hashtable_remove_fn_t)(hashtable *ht, const void *key);

typedef bool (*runtime_env_set_local_fn_t)(runtime_env *e, const struct symbol *key, const runtime_env_value *value);

typedef struct hashtable_ops_t {
    create_bindings_fn_t create_bindings;
    destroy_bindings_fn_t destroy_bindings;
    hashtable_key_is_in_use_fn_t hashtable_key_is_in_use;
    hashtable_get_fn_t hashtable_get;
    hashtable_add_fn_t hashtable_add;
    hashtable_reset_value_fn_t hashtable_reset_value;
    hashtable_remove_fn_t hashtable_remove;
} hashtable_ops_t;

typedef struct runtime_env_ops_t {
  runtime_env_set_local_fn_t set_local;
} runtime_env_ops_t;

typedef struct runtime_env_ctx {
    const hashtable_ops_t *hashtable_ops;
    const runtime_env_ops_t *ops;
} runtime_env_ctx;



// setters and getters

void runtime_env_set_hashtable_ops(const hashtable_ops_t *overrides);
void runtime_env_reset_hashtable_ops(void);
void runtime_env_set_create_bindings(create_bindings_fn_t create_bindings);
void runtime_env_set_destroy_bindings(destroy_bindings_fn_t destroy_bindings);
void runtime_env_set_hashtable_key_is_in_use(hashtable_key_is_in_use_fn_t fn);
void runtime_env_set_hashtable_get(hashtable_get_fn_t fn);
void runtime_env_set_hashtable_add(hashtable_add_fn_t fn);
void runtime_env_set_hashtable_reset_value(hashtable_reset_value_fn_t fn);
void runtime_env_set_hashtable_remove(hashtable_remove_fn_t fn);

void runtime_env_set_set_local(runtime_env_set_local_fn_t fn);


create_bindings_fn_t runtime_env_get_create_bindings(void);
destroy_bindings_fn_t runtime_env_get_destroy_bindings(void);
hashtable_key_is_in_use_fn_t runtime_env_get_hashtable_key_is_in_use(void);
hashtable_get_fn_t runtime_env_get_hashtable_get(void);
hashtable_add_fn_t runtime_env_get_hashtable_add(void);
hashtable_reset_value_fn_t runtime_env_get_hashtable_reset_value(void);
hashtable_remove_fn_t runtime_env_get_hashtable_remove(void);

runtime_env_set_local_fn_t runtime_env_get_set_local(void);

#endif //LEXLEO_RUNTIME_ENV_CTX_H
