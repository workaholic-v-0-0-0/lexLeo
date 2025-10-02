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

runtime_env_value *runtime_env_make_string(const char *s) {
    if (!s)
        return NULL;

    runtime_env_value *ret = RUNTIME_ENV_MALLOC(sizeof(runtime_env_value));
    if (!ret)
        return NULL;

    ret->as.s = RUNTIME_ENV_STRING_DUPLICATE(s);
    if (!ret->as.s) {
        RUNTIME_ENV_FREE(ret);
        return NULL;
    }

    ret->type = RUNTIME_VALUE_STRING;

    return ret;
}

runtime_env_value *runtime_env_make_symbol(const struct symbol *sym) {
    runtime_env_value *ret = RUNTIME_ENV_MALLOC(sizeof(runtime_env_value));
    if (!ret)
        return NULL;

    ret->type = RUNTIME_VALUE_SYMBOL;
    ret->as.sym = sym;

    return ret;
}

runtime_env_value *runtime_env_make_error(int code, const char *msg) {
    if (!msg)
        return NULL;

    runtime_env_value *ret = RUNTIME_ENV_MALLOC(sizeof(runtime_env_value));
    if (!ret)
        return NULL;

    ret->as.err.msg = RUNTIME_ENV_STRING_DUPLICATE(msg);
    if (!ret->as.err.msg) {
        RUNTIME_ENV_FREE(ret);
        return NULL;
    }

    ret->type = RUNTIME_VALUE_ERROR;
    ret->as.err.code = code;

    return ret;
}

runtime_env_value *runtime_env_make_function(
        const struct ast *function_node,
        runtime_env *closure) {
    runtime_env_value *ret = RUNTIME_ENV_MALLOC(sizeof(runtime_env_value));
    if (!ret)
        return NULL;

    ret->type = RUNTIME_VALUE_FUNCTION;
    ret->as.fn.function_node = function_node;
    ret->as.fn.closure = closure;

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