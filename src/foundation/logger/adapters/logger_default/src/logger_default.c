/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/logger/adapters/logger_default/src/
 * logger_default.c
 */

#include "internal/logger_ctx.h" // has to change
#include "wiring/logger_default_ctx.h"
#include "wiring/logger_default.h"
#include "internal/logger_default_state.h"
#include "internal/logger_default_backend.h"
#include "mem/osal_mem_ops.h"
#include "stream/borrowers/stream.h"
// #include "fs_stream.h" // commented only so that it can build
#include "lexleo_assert.h"

static logger_status_t logger_default_log(void *backend, const char *message) {
	return LOGGER_STATUS_OK; // placeholder
}

static void logger_default_destroy(void *backend) {
	// placeholder
}

static const logger_vtbl_t DEFAULT_VTBL = {
	.log = logger_default_log,
	.destroy = logger_default_destroy,
};

logger_default_ctx_t logger_default_default_ctx(
	const osal_mem_ops_t *mem_ops )
{
	logger_default_ctx_t ctx;
	ctx.deps.mem = mem_ops ? mem_ops : osal_mem_default_ops();
	return ctx;
}

static logger_status_t create_backend(
	logger_default_t **out,
	const logger_default_ctx_t *ctx )
{
	if (out) *out = NULL;
	if (
			   !out
			|| !ctx
			|| !ctx->deps.mem
			|| !ctx->deps.mem->calloc
			|| !ctx->deps.mem->free )
		return LOGGER_STATUS_INVALID;

	logger_default_t *backend = ctx->deps.mem->calloc(1, sizeof(*backend));
	if (!backend)
		return LOGGER_STATUS_OOM;
	backend->mem = ctx->deps.mem;
	// bbb todo

	return LOGGER_STATUS_OK; // placeholder
}

logger_status_t logger_default_create_logger(
	logger_t **out,
	const logger_default_ctx_t *ctx )
{
	return LOGGER_STATUS_OK; // placeholder
}
