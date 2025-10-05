// src/runtime_env/src/runtime_env.c

#include "runtime_env.h"
#include "internal/runtime_env_internal.h"
#include "internal/runtime_env_ctx.h"

#include "hashtable.h"

#include <stdbool.h>

void runtime_env_value_destroy_adapter(void *value) {
    runtime_env_value_destroy((runtime_env_value *)value);
}

static const runtime_env_ops RUNTIME_ENV_OPS_DEFAULT = {
    .create_bindings = hashtable_create,
    .destroy_bindings = hashtable_destroy,
    .hashtable_key_is_in_use = hashtable_key_is_in_use,
    .hashtable_get = hashtable_get,
    .hashtable_add = hashtable_add,
    .hashtable_reset_value = hashtable_reset_value,
    .hashtable_remove = hashtable_remove,
};

static runtime_env_ctx g_runtime_env_ctx = {
    .ops = &RUNTIME_ENV_OPS_DEFAULT
};

static runtime_env *g_runtime_env_toplevel = NULL;

runtime_env *runtime_env_make_toplevel(void) {
    g_runtime_env_toplevel = RUNTIME_ENV_MALLOC(sizeof(runtime_env));
    if (!g_runtime_env_toplevel)
        return NULL;

    g_runtime_env_toplevel->bindings =
        g_runtime_env_ctx.ops->create_bindings(
            RUNTIME_ENV_SIZE,
            RUNTIME_ENV_KEY_TYPE,
            runtime_env_value_destroy_adapter
        );
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

    runtime_env_retain(closure);

    return ret;
}

runtime_env *runtime_env_unwind(runtime_env *e) {
	if (!e)
		return NULL;

	g_runtime_env_ctx.ops->destroy_bindings(e->bindings);

	runtime_env *ret = e->parent;
	RUNTIME_ENV_FREE(e);

	return ret;
}

void runtime_env_retain(runtime_env *e) {
    if (e) e->refcount++;
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

    ret->bindings =
        g_runtime_env_ctx.ops->create_bindings(
            RUNTIME_ENV_SIZE,
            RUNTIME_ENV_KEY_TYPE,
            runtime_env_value_destroy_adapter
        );
    if (!ret->bindings) {
        RUNTIME_ENV_FREE(ret);
        return NULL;
    }

    ret->refcount = 1;
    ret->is_root = false;
    ret->parent = parent;

	return ret;
}

runtime_env_value *runtime_env_value_clone(const runtime_env_value *value) {
    if (!value)
        return NULL;

	switch (value->type) {
	case RUNTIME_VALUE_NUMBER:
		return runtime_env_make_number(value->as.i);
	case RUNTIME_VALUE_STRING:
		return runtime_env_make_string(value->as.s);
	case RUNTIME_VALUE_SYMBOL:
		return runtime_env_make_symbol(value->as.sym);
	case RUNTIME_VALUE_ERROR:
		return runtime_env_make_error(value->as.err.code, value->as.err.msg);
	case RUNTIME_VALUE_FUNCTION:
		return runtime_env_make_function(value->as.fn.function_node, value->as.fn.closure);
	default:
		return NULL;
	}
}

bool runtime_env_set_local(
        runtime_env *e,
        const struct symbol *key,
        const runtime_env_value *value ) {
    if (!e || !key || !value)
        return false;

	runtime_env_value *clone = runtime_env_value_clone(value);
	if (!clone)
		return false;

    const bool in_use =
		g_runtime_env_ctx.ops->hashtable_key_is_in_use(e->bindings, key);

	bool ok =
		in_use ?
			(
				g_runtime_env_ctx.ops->hashtable_reset_value(
					e->bindings,
					key,
					clone )
				==
				0 )
			:
			(
				g_runtime_env_ctx.ops->hashtable_add(
					e->bindings,
					key,
					clone )
				==
				0 )
			;

	if (!ok) {
		runtime_env_value_destroy(clone);
		return false;
	}

    return true;
}



// setters and getters

void runtime_env_set_ops(const runtime_env_ops *overrides) {
    static runtime_env_ops applied;
    applied = *g_runtime_env_ctx.ops;

    if (overrides) {
        if (overrides->create_bindings)
            applied.create_bindings = overrides->create_bindings;
        if (overrides->destroy_bindings)
            applied.destroy_bindings = overrides->destroy_bindings;
        if (overrides->hashtable_key_is_in_use)
            applied.hashtable_key_is_in_use = overrides->hashtable_key_is_in_use;
        if (overrides->hashtable_get)
            applied.hashtable_get = overrides->hashtable_get;
        if (overrides->hashtable_add)
            applied.hashtable_add = overrides->hashtable_add;
        if (overrides->hashtable_reset_value)
            applied.hashtable_reset_value = overrides->hashtable_reset_value;
        if (overrides->hashtable_remove)
            applied.hashtable_remove = overrides->hashtable_remove;
    }
    g_runtime_env_ctx.ops = &applied;
}

void runtime_env_reset_ops(void) {
    g_runtime_env_ctx.ops = &RUNTIME_ENV_OPS_DEFAULT;
}

void runtime_env_set_create_bindings(create_bindings_fn_t fn) {
    runtime_env_set_ops(&(runtime_env_ops){
        .create_bindings = fn ? fn : RUNTIME_ENV_OPS_DEFAULT.create_bindings
    });
}

void runtime_env_set_destroy_bindings(destroy_bindings_fn_t fn) {
    runtime_env_set_ops(&(runtime_env_ops){
        .destroy_bindings = fn ? fn : RUNTIME_ENV_OPS_DEFAULT.destroy_bindings
    });
}

void runtime_env_set_hashtable_key_is_in_use(hashtable_key_is_in_use_fn_t fn) {
    runtime_env_set_ops(&(runtime_env_ops){
        .hashtable_key_is_in_use = fn ? fn : RUNTIME_ENV_OPS_DEFAULT.hashtable_key_is_in_use
    });
}

void runtime_env_set_hashtable_get(hashtable_get_fn_t fn) {
    runtime_env_set_ops(&(runtime_env_ops){
        .hashtable_get = fn ? fn : RUNTIME_ENV_OPS_DEFAULT.hashtable_get
    });
}

void runtime_env_set_hashtable_add(hashtable_add_fn_t fn) {
    runtime_env_set_ops(&(runtime_env_ops){
        .hashtable_add = fn ? fn : RUNTIME_ENV_OPS_DEFAULT.hashtable_add
    });
}

void runtime_env_set_hashtable_reset_value(hashtable_reset_value_fn_t fn) {
    runtime_env_set_ops(&(runtime_env_ops){
        .hashtable_reset_value = fn ? fn : RUNTIME_ENV_OPS_DEFAULT.hashtable_reset_value
    });
}

void runtime_env_set_hashtable_remove(hashtable_remove_fn_t fn) {
    runtime_env_set_ops(&(runtime_env_ops){
        .hashtable_remove = fn ? fn : RUNTIME_ENV_OPS_DEFAULT.hashtable_remove
    });
}

create_bindings_fn_t runtime_env_get_create_bindings(void) {
    return g_runtime_env_ctx.ops->create_bindings;
}

destroy_bindings_fn_t runtime_env_get_destroy_bindings(void) {
    return g_runtime_env_ctx.ops->destroy_bindings;
}

hashtable_key_is_in_use_fn_t runtime_env_get_hashtable_key_is_in_use(void) {
    return g_runtime_env_ctx.ops->hashtable_key_is_in_use;
}

hashtable_get_fn_t runtime_env_get_hashtable_get(void) {
    return g_runtime_env_ctx.ops->hashtable_get;
}

hashtable_add_fn_t runtime_env_get_hashtable_add(void) {
    return g_runtime_env_ctx.ops->hashtable_add;
}

hashtable_reset_value_fn_t runtime_env_get_hashtable_reset_value(void) {
    return g_runtime_env_ctx.ops->hashtable_reset_value;
}

hashtable_remove_fn_t runtime_env_get_hashtable_remove(void) {
    return g_runtime_env_ctx.ops->hashtable_remove;
}
