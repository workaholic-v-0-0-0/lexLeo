// src/runtime_env/include/internal/runtime_env_ctx.h

#ifndef LEXLEO_RUNTIME_ENV_CTX_H
#define LEXLEO_RUNTIME_ENV_CTX_H

typedef struct hashtable hashtable;

typedef hashtable *(*create_bindings_fn_t)(
    size_t size,
    hashtable_key_type key_type,
    hashtable_destroy_value_fn_t destroy_value_fn );

typedef void (*destroy_bindings_fn_t)(hashtable *ht);

typedef struct runtime_env_ops {
    create_bindings_fn_t create_bindings;
    destroy_bindings_fn_t destroy_bindings;
    // here
} runtime_env_ops;

extern const runtime_env_ops RUNTIME_ENV_OPS_DEFAULT;

typedef struct runtime_env_ctx {
    const runtime_env_ops *ops;
} runtime_env_ctx;

void runtime_env_set_create_bindings(create_bindings_fn_t create_bindings);
void runtime_env_set_destroy_bindings(destroy_bindings_fn_t destroy_bindings);
create_bindings_fn_t runtime_env_get_create_bindings(void);
destroy_bindings_fn_t runtime_env_get_destroy_bindings(void);

#endif //LEXLEO_RUNTIME_ENV_CTX_H
