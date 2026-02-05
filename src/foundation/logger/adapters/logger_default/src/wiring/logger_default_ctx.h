/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/logger/adapters/logger_default/src/wiring/
 * logger_default_ctx.h
 */

#ifndef LEXLEO_LOGGER_DEFAULT_CTX_H
#define LEXLEO_LOGGER_DEFAULT_CTX_H

#include "mem/osal_mem_ops.h"

typedef struct logger_default_deps_t {
	const osal_mem_ops_t *mem;
} logger_default_deps_t;

typedef struct logger_default_ctx_t {
	logger_default_deps_t deps;
} logger_default_ctx_t;

logger_default_ctx_t logger_default_default_ctx(const osal_mem_ops_t *mem_ops);

#endif //LEXLEO_LOGGER_DEFAULT_CTX_H
