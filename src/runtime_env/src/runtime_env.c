// src/runtime_env/src/runtime_env.c

#include "runtime_env.h"
#include "internal/runtime_env_internal.h"
#include "internal/runtime_env_ctx.h"

#include "hashtable.h"

#include <stdbool.h>

void runtime_env_value_destroy_adapter(void *value) {
    runtime_env_value_destroy((runtime_env_value *)value);
}

hashtable *create_bindings_default() {
    return
        hashtable_create(
            RUNTIME_ENV_SIZE,
            RUNTIME_ENV_KEY_TYPE,
            runtime_env_value_destroy_adapter );
}

const runtime_env_ops RUNTIME_ENV_OPS_DEFAULT = {
    .create_bindings = create_bindings_default,
    .destroy_bindings = hashtable_destroy,
};

static runtime_env_ctx g_runtime_env_ctx = {
    .ops = &RUNTIME_ENV_OPS_DEFAULT
};

void runtime_env_set_destroy_bindings(destroy_bindings_fn_t destroy_bindings) {
    static runtime_env_ops applied;
    applied = *g_runtime_env_ctx.ops;
    applied.destroy_bindings =
        destroy_bindings ?
        destroy_bindings : RUNTIME_ENV_OPS_DEFAULT.destroy_bindings;
    g_runtime_env_ctx.ops = &applied;
}

destroy_bindings_fn_t runtime_env_get_destroy_bindings(void) {
    return g_runtime_env_ctx.ops->destroy_bindings;
}

static runtime_env *g_runtime_env_toplevel = NULL;

runtime_env *runtime_env_make_toplevel(void) {
    g_runtime_env_toplevel = RUNTIME_ENV_MALLOC(sizeof(runtime_env));
    if (!g_runtime_env_toplevel)
        return NULL;

    g_runtime_env_toplevel->bindings = g_runtime_env_ctx.ops->create_bindings();
    if (!g_runtime_env_toplevel->bindings) {
        RUNTIME_ENV_FREE(g_runtime_env_toplevel);
        g_runtime_env_toplevel = NULL;
        return NULL;
    }

    g_runtime_env_toplevel->refcount = 1;
    g_runtime_env_toplevel->is_root = true;
    g_runtime_env_toplevel->parent = NULL;

    return g_runtime_env_toplevel;
}

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

runtime_env *runtime_env_unwind(runtime_env *e) {
	if (!e)
		return NULL;

	hashtable_destroy(e->bindings);

	runtime_env *ret = e->parent;
	RUNTIME_ENV_FREE(e);

	return ret;
}

void runtime_env_release(runtime_env *e) {
	if (!e || e->refcount <= 0 || e->is_root)
        return;

	if (--e->refcount == 0) {
        g_runtime_env_ctx.ops->destroy_bindings(e->bindings);
        RUNTIME_ENV_FREE(e);
	}
}

void runtime_env_value_destroy(runtime_env_value *value) {
	if (!value)
		return;
    switch (value->type) {
    case RUNTIME_VALUE_NUMBER:
        RUNTIME_ENV_FREE(value);
        break;
    case RUNTIME_VALUE_STRING:
		RUNTIME_ENV_FREE(value->as.s);
        RUNTIME_ENV_FREE(value);
        break;
    case RUNTIME_VALUE_SYMBOL:
        RUNTIME_ENV_FREE(value);
        break;
    case RUNTIME_VALUE_ERROR:
		RUNTIME_ENV_FREE(value->as.err.msg);
        RUNTIME_ENV_FREE(value);
        break;
    case RUNTIME_VALUE_FUNCTION:
        runtime_env_release(value->as.fn.closure);
		RUNTIME_ENV_FREE(value);
        break;
    default:
        // do nothing
    }
}

runtime_env *runtime_env_wind(runtime_env *parent) {
    if (!parent)
        return runtime_env_make_toplevel();

	runtime_env *ret = RUNTIME_ENV_MALLOC(sizeof(runtime_env));
    if (!ret)
        return NULL;

    ret->bindings = g_runtime_env_ctx.ops->create_bindings();
    if (!ret->bindings) {
        RUNTIME_ENV_FREE(ret);
        return NULL;
    }

    ret->refcount = 1;
    ret->is_root = false;
    ret->parent = parent;

	return ret;
}
