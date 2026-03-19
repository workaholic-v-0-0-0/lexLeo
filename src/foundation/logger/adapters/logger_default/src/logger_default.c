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
#include "policy/lexleo_assert.h"
#include "policy/lexleo_cstring.h"

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

static logger_status_t logger_default_log(void *backend, const char *message)
{
	if (!message)
		return LOGGER_STATUS_INVALID;

	LEXLEO_ASSERT(backend);

	logger_default_t *logger_default = (logger_default_t *)backend;

	stream_status_t st = STREAM_STATUS_OK;
	size_t len = strlen(message);

	size_t n =
		stream_write(
			logger_default->stream,
			message,
			len,
			&st
		);
	if (st != STREAM_STATUS_OK || n != len) {
		return LOGGER_STATUS_IO_ERROR;
	}

	if (logger_default->append_newline) {
		n = stream_write(logger_default->stream, "\n", 1, &st);

		if (st != STREAM_STATUS_OK || n != 1) {
			return LOGGER_STATUS_IO_ERROR;
		}
	}

	return LOGGER_STATUS_OK;
}

static void logger_default_destroy(void *backend) {
	if (!backend) return;

	logger_default_t *logger_default = (logger_default_t *)backend;
	LEXLEO_ASSERT(logger_default->mem && logger_default->mem->free);

	logger_default->mem->free(logger_default);
}

static const logger_vtbl_t DEFAULT_VTBL = {
	.log = logger_default_log,
	.destroy = logger_default_destroy,
};

logger_status_t logger_default_create_logger(
	logger_t **out,
	const logger_default_cfg_t *cfg,
	const logger_default_env_t *env)
{
	if (!out || !cfg || !env)
		return LOGGER_STATUS_INVALID;

	LEXLEO_ASSERT(
		   env->adapter_mem
		&& env->adapter_mem->malloc
		&& env->adapter_mem->free
		&& env->stream
	);

	logger_default_t *backend = env->adapter_mem->malloc(sizeof(*backend));
	if (!backend)
		return LOGGER_STATUS_OOM;

	backend->stream = env->stream;
	backend->append_newline = cfg->append_newline;
	backend->mem = env->adapter_mem;

	logger_t *tmp = NULL;

	logger_status_t st =
		logger_create(&tmp, &DEFAULT_VTBL, (void *)backend, &env->port_env);

	if (st != LOGGER_STATUS_OK) {
		env->adapter_mem->free(backend);
		return st;
	}

	*out = tmp;

	return LOGGER_STATUS_OK;
}
