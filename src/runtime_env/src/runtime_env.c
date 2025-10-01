// src/runtime_env/src/runtime_env.c

#include "internal/runtime_env_internal.h"

#include <stdbool.h>

runtime_env_value *runtime_env_make_number(int i) {
    runtime_env_value * ret = RUNTIME_ENV_MALLOC(sizeof(runtime_env_value));
    if (!ret)
        return NULL;

    ret->type = RUNTIME_VALUE_NUMBER;
    ret->as.i = i;

    return ret;
}

/*
runtime_env *runtime_env_make_toplevel(void) {
    runtime_env *ret = runtime_env_wind(NULL);
    if (!ret)
        return NULL;
    ret->is_root = true; // makes the toplevel immortal
    return ret;
}
*/