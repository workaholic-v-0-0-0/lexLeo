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

#include "internal/logger_default_handle.h"
#include "internal/logger_default_utc_timestamp.h"
#include "stream/borrowers/stream.h"
#include "logger/adapters/logger_adapters_api.h"
#include "logger_default/cr/logger_default_cr_api.h"
#include "osal/time/osal_time.h"
#include "policy/lexleo_assert.h"
#include "policy/lexleo_cstring.h"
#include "policy/lexleo_cstd_io.h"
#include "policy/lexleo_cstd_inttypes.h"

/**
 * @brief Return the default configuration for the `logger_default` adapter.
 *
 * @details
 * This helper establishes the default runtime behavior of the adapter.
 *
 * @return
 * A well-formed `logger_default_cfg_t`.
 */
logger_default_cfg_t logger_default_default_cfg(void)
{
	return (logger_default_cfg_t){ .append_newline = true };
}

/**
 * @brief Build a default environment for the `logger_default` adapter.
 *
 * @details
 * This helper packages borrowed runtime dependencies into a
 * `logger_default_env_t`.
 *
 * @param[in] stream
 * Borrowed target stream used by the adapter.
 *
 * @param[in] time_ops
 * Borrowed time operations used for timestamp generation.
 *
 * @param[in] adapter_mem
 * Borrowed memory operations used for adapter-backend allocation.
 *
 * @param[in] port_env
 * Borrowed `logger` port environment.
 *
 * @return
 * A well-formed `logger_default_env_t` aggregating the provided dependencies.
 */
logger_default_env_t logger_default_default_env(
	stream_t *stream,
	const osal_time_ops_t *time_ops,
	const osal_mem_ops_t *adapter_mem,
	const logger_env_t *port_env)
{
	return (logger_default_env_t){
		.stream = stream,
		.time_ops = time_ops,
		.adapter_mem = adapter_mem,
		.port_env = *port_env
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
 * Timestamp, message, or newline write failed.
 */
static logger_status_t logger_default_log(void *backend, const char *message)
{
	if (!message) {
		return LOGGER_STATUS_INVALID;
	}

	LEXLEO_ASSERT(backend);

	logger_default_t *logger_default = (logger_default_t *)backend;

	LEXLEO_ASSERT(logger_default->stream && logger_default->time_ops);

	logger_status_t write_timestamp_st =
		logger_default_write_timestamp(
			logger_default->stream,
			logger_default->time_ops);
	if (write_timestamp_st != LOGGER_STATUS_OK) {
		return write_timestamp_st;
	}

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
		n = stream_write(logger_default->stream, "\n", 1u, &st);

		if (st != STREAM_STATUS_OK || n != 1u) {
			return LOGGER_STATUS_IO_ERROR;
		}
	}

	return LOGGER_STATUS_OK;
}

/**
 * @brief Private `destroy` implementation for the `logger_default` backend.
 *
 * @details
 * This function releases the private backend object using the injected
 * adapter-memory operations table.
 *
 * @param[in] backend
 * Private `logger_default` backend handle.
 * May be `NULL`.
 */
static void logger_default_destroy(void *backend)
{
	if (!backend) {
		return;
	}

	logger_default_t *logger_default = (logger_default_t *)backend;
	LEXLEO_ASSERT(logger_default->mem && logger_default->mem->free);

	logger_default->mem->free(logger_default);
}

/**
 * @brief Private backend vtable for `logger_default`.
 */
static const logger_vtbl_t DEFAULT_VTBL = {
	.log = logger_default_log,
	.destroy = logger_default_destroy,
};

/**
 * @brief Create a logger instance backed by the `logger_default` adapter.
 *
 * @details
 * This function allocates the private `logger_default` backend, initializes it
 * from the injected configuration and environment, and then wires it into the
 * public `logger` port through `logger_create()`.
 *
 * @param[out] out
 * Receives the created logger handle.
 * Must not be `NULL`.
 *
 * @param[in] cfg
 * Adapter configuration.
 * Must not be `NULL`.
 *
 * @param[in] env
 * Adapter environment.
 * Must not be `NULL`.
 *
 * @retval LOGGER_STATUS_OK
 * Logger successfully created.
 *
 * @retval LOGGER_STATUS_INVALID
 * One or more public arguments are invalid.
 *
 * @retval LOGGER_STATUS_OOM
 * Adapter-backend allocation failed.
 */
logger_status_t logger_default_create_logger(
	logger_t **out,
	const logger_default_cfg_t *cfg,
	const logger_default_env_t *env)
{
	if (!out || !cfg || !env) {
		return LOGGER_STATUS_INVALID;
	}

	LEXLEO_ASSERT(
		   env->time_ops
		&& env->time_ops->now
		&& env->adapter_mem
		&& env->adapter_mem->malloc
		&& env->adapter_mem->free
		&& env->stream
	);

	logger_default_t *backend = env->adapter_mem->malloc(sizeof(*backend));
	if (!backend) {
		return LOGGER_STATUS_OOM;
	}

	backend->stream = env->stream;
	backend->time_ops = env->time_ops;
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
