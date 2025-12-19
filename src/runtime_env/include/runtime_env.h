// src/runtime_env/include/runtime_env.h

#ifndef LEXLEO_RUNTIME_ENV_H
#define LEXLEO_RUNTIME_ENV_H

#include "internal/runtime_env_memory_allocator.h"
#include "internal/runtime_env_string_utils.h"

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    RUNTIME_VALUE_NUMBER,
    RUNTIME_VALUE_STRING,
    RUNTIME_VALUE_SYMBOL,
    RUNTIME_VALUE_ERROR,
    RUNTIME_VALUE_FUNCTION,
    RUNTIME_VALUE_QUOTED,
    RUNTIME_VALUE_TYPE_NB_TYPES,
} runtime_env_value_type;

typedef enum {
    RUNTIME_ENV_ERROR_NOT_A_FUNCTION,
    RUNTIME_ENV_ERROR_UNRETRIEVABLE,
    RUNTIME_ERROR_TYPE_NB_TYPES,
} runtime_env_error_code;

struct ast; // borrowed
struct symbol; // borrowed; lifetime managed by symtab
struct runtime_env; // opaque
typedef struct runtime_env runtime_env;

typedef struct runtime_env_value {
    size_t refcount;
    runtime_env_value_type type;
    union {
        int i;
        char *s; //owned
        const struct symbol *sym; // borrowed from symtab module
        struct {
            runtime_env_error_code code;
            char *msg; // owned
        } err; // owned container
        struct {
            const struct ast *function_node; // borrowed from ast module
            struct runtime_env *closure; // retained/released
        } fn; // owned container
        const struct ast *quoted; // borrowed from ast_pool of runtime_session
    } as; // owned container
} runtime_env_value;

runtime_env_value *runtime_env_make_number(int i);
runtime_env_value *runtime_env_make_string(const char *s); // with string duplication
runtime_env_value *runtime_env_make_symbol(const struct symbol *sym); // borrowed
runtime_env_value *runtime_env_make_error(int code, const char *msg);
runtime_env_value *runtime_env_make_function(const struct ast *function_node, runtime_env *closure);
runtime_env_value *runtime_env_make_quoted(const struct ast *quoted);

// Note: After runtime_env_make_*, runtime_env_value must always be destroyed with runtime_env_value_destroy
void runtime_env_value_retain(const runtime_env_value *v);
void runtime_env_value_release(const runtime_env_value *v);

runtime_env *runtime_env_wind(runtime_env *parent);
runtime_env *runtime_env_unwind(runtime_env *e);

/**
 * Associates a symbol with a runtime value in the given environment frame.
 *
 * Ownership and reference counting rules:
 *  - On success, the environment RETAINS the value (i.e., increments its refcount).
 *  - The caller keeps its own ownership and is still responsible for calling
 *    runtime_env_value_release() when done with the value.
 *  - If the operation fails (e.g. OOM, hash error), the refcount is unchanged.
 *
 * @param e    Target environment (must not be NULL)
 * @param key  Symbol to bind (must not be NULL)
 * @param value Runtime value to bind (must not be NULL)
 * @return true on success, false on failure
 */
bool runtime_env_set_local(runtime_env *e, const struct symbol *key, const runtime_env_value *value);

/**
 * Retrieves the value bound to the given symbol in the current environment frame only.
 *
 * Ownership and reference counting rules:
 *  - The returned pointer is BORROWED (no retain).
 *  - The caller must NOT modify or release it directly.
 *  - If the caller needs to keep the value beyond the lifetime of the environment,
 *    it MUST call runtime_env_value_retain() before storing or returning it.
 *
 * @param e   Environment to look up (must not be NULL)
 * @param key Symbol to look up (must not be NULL)
 * @return Borrowed const pointer to the bound value, or NULL if not found
 */
runtime_env_value *runtime_env_get_local(const runtime_env *e, const struct symbol *key);

/**
 * Retrieves the value bound to the given symbol, searching the current environment
 * and its parents recursively.
 *
 * Ownership and reference counting rules:
 *  - The returned pointer is BORROWED (no retain).
 *  - The caller must NOT modify or release it directly.
 *  - If the caller needs to keep the value beyond the lifetime of the environment,
 *    it MUST call runtime_env_value_retain() before storing or returning it.
 *
 * @param e   Environment chain to search (must not be NULL)
 * @param key Symbol to look up (must not be NULL)
 * @return Borrowed const pointer to the bound value, or NULL if not found
 */
runtime_env_value *runtime_env_get(const runtime_env *e, const struct symbol *key);

#endif //LEXLEO_RUNTIME_ENV_H
