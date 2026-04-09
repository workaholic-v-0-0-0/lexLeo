// src/cli/include/internal/cli_internal.h

#ifndef LEXLEO_CLI_INTERNAL_H
#define LEXLEO_CLI_INTERNAL_H

#include "runtime_session.h"
#include "interpreter.h"

struct ast *cli_read_ast (const struct interpreter_ctx *ctx);

bool cli_print(
	const struct interpreter_ctx *ctx,
	const struct runtime_env_value *value);

bool cli_store_symbol(struct symbol *sym, void *session); // wrapper

#endif //LEXLEO_CLI_INTERNAL_H