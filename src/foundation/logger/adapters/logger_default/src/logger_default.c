/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_default.c
 * @ingroup logger_default_internal_group
 * @brief Private implementation of the `logger_default` adapter.
 *
 * @details
 * This file implements:
 * - default configuration and environment helpers,
 * - private timestamp writing support,
 * - the private backend vtable used to wire `logger_default` into the
 *   `logger` port,
 * - direct construction of a `logger_default`-backed `logger_t`.
 */

#include "logger/adapters/logger_adapters_api.h"
#include "logger_default/cr/logger_default_cr_api.h"

#include "stream/borrowers/stream_borrowers_api.h"

#include "osal/time/osal_time_ops.h"
#include "osal/mem/osal_mem_ops.h"
#include "osal/str/osal_str.h"

#include "policy/lexleo_assert.h"
#include "policy/lexleo_cstd_inttypes.h"
#include "policy/lexleo_cstd_io.h"

#include "internal/logger_default_handle.h"
#include "internal/logger_default_utc_timestamp.h"

logger_default_cfg_t logger_default_default_cfg(void)
{
	return (logger_default_cfg_t) {
		.append_newline = true
	};
}

logger_default_env_t logger_default_default_env(
	stream_t *stream,
	const osal_time_ops_t *time_ops,
	const osal_mem_ops_t *adapter_mem_ops,
	const osal_mem_ops_t *port_mem_ops
) {
	return (logger_default_env_t){
		.stream = stream,
		.time_ops = time_ops,
		.adapter_mem_ops = adapter_mem_ops,
		.port_mem_ops = port_mem_ops
	};
}

/**
 * @brief Write the `logger_default` timestamp prefix to the target stream.
 *
 * @details
 * This helper obtains the current epoch time from the injected time service,
 * converts it to a decomposed UTC+0 timestamp, formats it as text, and writes
 * the resulting prefix to `stream`.
 *
 * If the injected time service fails, this helper writes the fallback prefix
 * `"[timestamp error] "` instead.
 *
 * @param[in] stream
 * Target stream receiving the timestamp prefix.
 * Must not be `NULL`.
 *
 * @param[in] time_ops
 * Borrowed time operations used to obtain the current epoch time.
 * Must not be `NULL`.
 *
 * @retval LOGGER_STATUS_OK
 * Timestamp prefix successfully written, or fallback timestamp-error prefix
 * successfully written.
 *
 * @retval LOGGER_STATUS_IO_ERROR
 * A stream write failed or produced a partial write, or timestamp formatting
 * failed.
 */
static logger_status_t logger_default_write_timestamp(
	stream_t *stream,
	const osal_time_ops_t *time_ops)
{
	static const char TIMESTAMP_ERROR[] = "[timestamp error] ";

	osal_time_t epoch_time;
	osal_time_status_t now_st = time_ops->now(&epoch_time);
	if (now_st != OSAL_TIME_STATUS_OK) {
		stream_status_t write_st = STREAM_STATUS_OK;
		size_t n =
			stream_write(
				stream,
				TIMESTAMP_ERROR,
				sizeof(TIMESTAMP_ERROR) - 1u,
				&write_st);

		if (write_st != STREAM_STATUS_OK ||
		    n != sizeof(TIMESTAMP_ERROR) - 1u) {
			return LOGGER_STATUS_IO_ERROR;
		}

		return LOGGER_STATUS_OK;
	}

	logger_default_utc_timestamp_t date;
	if (!logger_default_epoch_time_to_date(&date, &epoch_time)) {
		stream_status_t write_st = STREAM_STATUS_OK;
		size_t n =
			stream_write(
				stream,
				TIMESTAMP_ERROR,
				sizeof(TIMESTAMP_ERROR) - 1u,
				&write_st);

		if (write_st != STREAM_STATUS_OK ||
		    n != sizeof(TIMESTAMP_ERROR) - 1u) {
			return LOGGER_STATUS_IO_ERROR;
		}

		return LOGGER_STATUS_OK;
	}

	char buf[64];
	int len =
		snprintf(
			buf,
			sizeof(buf),
			"[%04" PRId32 "-%02" PRId32 "-%02" PRId32
			" %02" PRId32 ":%02" PRId32 ":%02" PRId32
			" UTC+0] ",
			date.year,
			date.month,
			date.day,
			date.hour,
			date.minute,
			date.second
		);

	if (len < 0 || (size_t)len >= sizeof(buf)) {
		return LOGGER_STATUS_IO_ERROR;
	}

	stream_status_t write_st = STREAM_STATUS_OK;
	size_t n = stream_write(stream, buf, (size_t)len, &write_st);

	if (write_st != STREAM_STATUS_OK || n != (size_t)len) {
		return LOGGER_STATUS_IO_ERROR;
	}

	return LOGGER_STATUS_OK;
}

/**
 * @brief Private `log` implementation for the `logger_default` backend.
 *
 * @details
 * This function writes:
 * - a timestamp prefix produced from the injected time service,
 * - the provided message,
 * - an optional trailing newline depending on backend configuration.
 *
 * @param[in] backend
 * Private `logger_default` backend handle.
 * Must not be `NULL`.
 *
 * @param[in] message
 * Message to log.
 *
 * @retval LOGGER_STATUS_OK
 * Message successfully written.
 *
 * @retval LOGGER_STATUS_INVALID
 * `message == NULL`.
 *
 * @retval LOGGER_STATUS_IO_ERROR
 * A required stream write failed or was partial.
 */
static logger_status_t logger_default_log(
	void *backend,
	const char *message
) {
	if (!message) {
		return LOGGER_STATUS_INVALID;
	}

	LEXLEO_ASSERT(backend);

	logger_default_t *logger_default = (logger_default_t *)backend;

	LEXLEO_ASSERT(
		   logger_default->stream
		&& logger_default->time_ops
	);

	logger_status_t write_timestamp_st =
		logger_default_write_timestamp(
			logger_default->stream,
			logger_default->time_ops);
	if (write_timestamp_st != LOGGER_STATUS_OK) {
		return write_timestamp_st;
	}

	stream_status_t st = STREAM_STATUS_OK;

	size_t len = osal_strlen(message);

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
		n = stream_write(logger_default->stream, "\n", 1u, &st);

		if (st != STREAM_STATUS_OK || n != 1u) {
			return LOGGER_STATUS_IO_ERROR;
		}
	}

	stream_status_t flush_st = stream_flush(logger_default->stream);
	if (flush_st != STREAM_STATUS_OK) {
		return LOGGER_STATUS_IO_ERROR;
	}

	return LOGGER_STATUS_OK;
}

static logger_status_t logger_default_destroy(void *backend)
{
	LEXLEO_ASSERT(backend); /* via logger_destroy() contract */

	logger_default_t *logger_default = (logger_default_t *)backend;

	LEXLEO_ASSERT(
		   logger_default->mem
		&& logger_default->mem->free
	);

	logger_default->mem->free(logger_default);

	return LOGGER_STATUS_OK;
}

/**
 * @brief Private backend vtable for `logger_default`.
 */
static const logger_vtbl_t g_logger_default_vtbl = {
	.log = logger_default_log,
	.destroy = logger_default_destroy,
};

static logger_status_t logger_default_create_backend(
	logger_default_t **out,
	const logger_default_cfg_t *cfg,
	const logger_default_env_t *env
) {
	if (
		   !out
		|| !cfg
		|| !env
	) {
		return LOGGER_STATUS_INVALID;
	}

	LEXLEO_ASSERT(
		   env->stream
		&& env->time_ops
		&& env->adapter_mem_ops
		&& env->adapter_mem_ops->calloc
		&& env->adapter_mem_ops->free
	);

	logger_default_t *backend =
		env->adapter_mem_ops->calloc(1, sizeof(*backend));
	if (!backend) {
		return LOGGER_STATUS_OOM;
	}

	backend->stream = env->stream;
	backend->time_ops = env->time_ops;
	backend->append_newline = cfg->append_newline;
	backend->mem = env->adapter_mem_ops;

	*out = backend;
	return LOGGER_STATUS_OK;
}

logger_status_t logger_default_create_logger(
	logger_t **out,
	const logger_default_cfg_t *cfg,
	const logger_default_env_t *env)
{
	if (!out || !cfg || !env) {
		return LOGGER_STATUS_INVALID;
	}

	logger_default_t *backend = NULL;
	logger_status_t st = logger_default_create_backend(&backend, cfg, env);
	if (st != LOGGER_STATUS_OK) {
		return st;
	}

	logger_t *tmp = NULL;

	logger_env_t logger_env =
		logger_default_env(
			&g_logger_default_vtbl,
			env->port_mem_ops
		);
	st = logger_create(&tmp, &logger_env);
	if (st != LOGGER_STATUS_OK) {
		logger_status_t st2 = logger_default_destroy(backend);
		LEXLEO_ASSERT(st2 == LOGGER_STATUS_OK);
		return st;
	}

	st = logger_complete_default_init(tmp, backend);
	LEXLEO_ASSERT(st == LOGGER_STATUS_OK);

	*out = tmp;
	return LOGGER_STATUS_OK;
}
