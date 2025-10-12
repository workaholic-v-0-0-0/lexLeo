// src/interpreter/include/interpreter.h

#ifndef LEXLEO_INTERPRETER_H
#define LEXLEO_INTERPRETER_H

#include <stddef.h>
#include <stdbool.h>

struct ast;
struct runtime_env;
struct runtime_env_value;

typedef enum {
    INTERPRETER_STATUS_OK = 0,
    INTERPRETER_STATUS_ERROR,
    INTERPRETER_STATUS_OOM,
    INTERPRETER_STATUS_BINDING_ERROR,
} interpreter_status;

interpreter_status interpreter_eval(
    struct runtime_env *env,
    const struct ast *root,
    struct runtime_env_value **out );

#endif //LEXLEO_INTERPRETER_H
