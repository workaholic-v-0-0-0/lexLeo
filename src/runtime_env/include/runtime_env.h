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
    RUNTIME_VALUE_TYPE_NB_TYPES,
} runtime_env_value_type;

typedef enum {
    RUNTIME_ENV_ERROR_NOT_A_FUNCTION,
    RUNTIME_ERROR_TYPE_NB_TYPES,
} runtime_env_error_code;

struct ast; // borrowed
struct symbol; // borrowed; lifetime managed by symtab
struct runtime_env; // opaque
typedef struct runtime_env runtime_env;

typedef struct runtime_env_value {
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
    } as; // owned container
} runtime_env_value;

runtime_env_value *runtime_env_make_number(int i);
runtime_env_value *runtime_env_make_string(const char *s); // with string duplication
runtime_env_value *runtime_env_make_symbol(const struct symbol *sym); // borrowed
runtime_env_value *runtime_env_make_error(int code, const char *msg);
runtime_env_value *runtime_env_make_function(const struct ast *function_node, runtime_env *closure);

// Note: After runtime_env_make_*, runtime_env_value must always be destroyed with runtime_env_value_destroy
void runtime_env_value_destroy(runtime_env_value *value);

runtime_env *runtime_env_wind(runtime_env *parent);
runtime_env *runtime_env_unwind(runtime_env *e);

bool runtime_env_set_local(runtime_env *e, const struct symbol *key, const runtime_env_value *value);
const runtime_env_value *runtime_env_get_local(const runtime_env *e, const struct symbol *key);
const runtime_env_value *runtime_env_get(const runtime_env *e, const struct symbol *key);

#endif //LEXLEO_RUNTIME_ENV_H
