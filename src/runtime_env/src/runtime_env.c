// src/runtime_env/src/runtime_env.c

#include "internal/runtime_env_internal.h"

#include <stdbool.h>

runtime_env *runtime_env_make_toplevel(void) {
    runtime_env *ret = runtime_env_wind(NULL);
    if (!ret)
        return NULL;
    ret->is_root = true; // makes the toplevel immortal
    return ret;
}
