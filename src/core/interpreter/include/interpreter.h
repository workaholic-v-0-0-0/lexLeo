// src/core/interpreter/include/interpreter.h

#ifndef LEXLEO_INTERPRETER_H
#define LEXLEO_INTERPRETER_H

#include <stddef.h>
#include <stdbool.h>

struct ast;
struct runtime_env;
struct runtime_env_value;

typedef enum {
    INTERPRETER_STATUS_OK = 0,
    INTERPRETER_STATUS_UNSUPPORTED_AST,
    INTERPRETER_STATUS_OOM,
    INTERPRETER_STATUS_BINDING_ERROR,
    INTERPRETER_STATUS_INVALID_AST,
    INTERPRETER_STATUS_TYPE_ERROR,
    INTERPRETER_STATUS_NOT_EVALUABLE,
    INTERPRETER_STATUS_ARITY_ERROR,
	INTERPRETER_STATUS_DUPLICATE_PARAMETER,
    INTERPRETER_STATUS_DIVISION_BY_ZERO,
    INTERPRETER_STATUS_LOOKUP_FAILED,
    INTERPRETER_STATUS_INTERNAL_ERROR,
    INTERPRETER_STATUS_READ_AST_ERROR,
    INTERPRETER_STATUS_WRITE_RUNTIME_VALUE_ERROR,
    INTERPRETER_STATUS_NB_TYPES,
} interpreter_status;

struct interpreter_ctx; // forward

typedef struct ast *(*interpreter_read_ast_fn_t)(const struct interpreter_ctx *ctx);

typedef bool (*interpreter_write_runtime_value_fn_t)(
    const struct interpreter_ctx *ctx,
    const struct runtime_env_value *value);

typedef struct interpreter_ops_t {
	// called when evaluating an AST of type AST_TYPE_READING ;
	// a hook for cli_read_parse_resolve
	interpreter_read_ast_fn_t read_ast_fn;
	// called when evaluating an AST of type AST_TYPE_WRITING ;
	// a hook for cli_print
    interpreter_write_runtime_value_fn_t write_runtime_value_fn;
} interpreter_ops_t;

struct interpreter_ctx *interpreter_ctx_create(
    const interpreter_ops_t *ops,
    // a hook the the relative runtime_session instance
    void *host_ctx );

void interpreter_ctx_destroy(struct interpreter_ctx *ctx);

interpreter_status interpreter_eval(
	struct interpreter_ctx *ctx,
    struct runtime_env *env,
    const struct ast *root,
    struct runtime_env_value **out );

void *interpreter_ctx_get_host_ctx(const struct interpreter_ctx *ctx);

#endif //LEXLEO_INTERPRETER_H
