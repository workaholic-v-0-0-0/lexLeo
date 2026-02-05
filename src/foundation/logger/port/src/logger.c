/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/logger/port/src/
 * logger.c
 */

#include "logger.h"
#include "internal/logger_ctx.h"
#include "internal/logger_internal.h"
#include "mem/osal_mem_ops.h"
#include "lexleo_assert.h"

logger_ctx_t logger_default_ctx(const osal_mem_ops_t *mem_ops) {
	logger_ctx_t ctx = {0};
	ctx.deps.mem = mem_ops ? mem_ops : osal_mem_default_ops();
	return ctx;
}

logger_status_t logger_create(
	logger_t **out,
	const logger_vtbl_t *vtbl,
	void *backend,
	const logger_ctx_t *ctx )
{
	if (out) *out = NULL;
	if (
		   !out
		|| !vtbl
		|| !vtbl->log
		|| !vtbl->destroy
		|| !ctx
		|| !ctx->deps.mem
		|| !ctx->deps.mem->malloc
		|| !ctx->deps.mem->free )
		return LOGGER_STATUS_INVALID;
	if (!backend)
		return LOGGER_STATUS_NO_BACKEND;

	logger_t *l = ctx->deps.mem->malloc(sizeof(*l));
	if (!l) return LOGGER_STATUS_OOM;

	l->vtbl = *vtbl;
	l->backend = backend;
	l->mem = ctx->deps.mem;

	*out = l;
	return LOGGER_STATUS_OK;
}

void logger_destroy(logger_t **l) {
	if (!l || !*l) return;
	logger_t *tmp = *l;
	*l = NULL;
	if (tmp->vtbl.destroy && tmp->backend) tmp->vtbl.destroy(tmp->backend);
	LEXLEO_ASSERT(tmp->mem && tmp->mem->free);
	tmp->mem->free(tmp);
}

logger_status_t logger_log(logger_t *l, const char *message) {
	if (!l || !message || !*message)
		return LOGGER_STATUS_INVALID;

	if (!l->backend)
		return LOGGER_STATUS_NO_BACKEND;

	if (!l->vtbl.log)
		return LOGGER_STATUS_INVALID;

	return l->vtbl.log(l->backend, message);
}
