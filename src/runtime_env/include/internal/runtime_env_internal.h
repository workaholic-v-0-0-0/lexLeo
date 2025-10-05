// src/runtime_env/include/internal/runtime_env_internal.h

#ifndef LEXLEO_RUNTIME_ENV_INTERNAL_H
#define LEXLEO_RUNTIME_ENV_INTERNAL_H

#include "runtime_env.h"

#include "hashtable.h"

#include <stdbool.h>

#define RUNTIME_ENV_SIZE 256

extern const hashtable_key_type RUNTIME_ENV_KEY_TYPE;
void runtime_env_value_destroy_adapter(void *value);

struct runtime_env {
    hashtable *bindings; // key: const symbol* (borrowed), value: runtime_env_value (owned)
    int refcount;
    bool is_root;
    struct runtime_env *parent;
};

// make the following fcts static?

runtime_env *runtime_env_make_toplevel(void);

runtime_env_value *runtime_env_value_clone(const runtime_env_value *value);
void runtime_env_destroy(runtime_env *e);
void runtime_env_retain(runtime_env *e);
void runtime_env_release(runtime_env *e);

#endif //LEXLEO_RUNTIME_ENV_INTERNAL_H
