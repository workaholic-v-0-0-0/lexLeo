/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/logger/adapters/logger_default/src/
 * logger_default.c
 */

#include "internal/logger_default_handle.h"
#include "stream/borrowers/stream.h"
#include "logger/adapters/logger_adapters_api.h"
#include "logger_default/cr/logger_default_cr_api.h"
//#include "policy/lexleo_assert.h"

logger_default_cfg_t logger_default_default_cfg(void) {
	return (logger_default_cfg_t) { .append_newline = true };
}

logger_default_env_t logger_default_default_env(
	stream_t *stream,
	const osal_mem_ops_t *adapter_mem,
	const logger_env_t *port_env)
{
	return (logger_default_env_t){
		.stream = stream,
		.adapter_mem = adapter_mem,
		.port_env = *port_env
	};
}

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

logger_default_cfg_t test;


// draft legacy
/*

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

*/