/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/logger/port/include/internal/
 * logger_ctx.h
 */

#ifndef LEXLEO_LOGGER_CTX_H
#define LEXLEO_LOGGER_CTX_H

#include "logger_types.h"
#include "mem/osal_mem_ops.h"

typedef struct logger_deps_t {
	const osal_mem_ops_t *mem;
} logger_deps_t;

typedef struct logger_vtbl_t {
	logger_status_t (*log)(void *backend, const char *message);
	void (*destroy)(void *backend);
} logger_vtbl_t;

typedef struct logger_ctx_t {
	logger_deps_t deps;
} logger_ctx_t;

logger_ctx_t logger_default_ctx(const osal_mem_ops_t *mem_ops);

logger_status_t logger_create(
	logger_t **out,
	const logger_vtbl_t *vtbl,
	void *backend,
	const logger_ctx_t *ctx );

void logger_destroy(logger_t **l);

#endif //LEXLEO_LOGGER_CTX_H
