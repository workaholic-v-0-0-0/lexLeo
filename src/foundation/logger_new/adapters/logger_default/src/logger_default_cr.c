/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_default_cr.c
 * @ingroup logger_default_cr_api
 * @brief Composition Root implementation for the `logger_default` adapter.
 *
 * @details
 * Implements construction and initialization of `logger_default_t` backends,
 * the adapter dispatch table, and the private backend operations implementing
 * the `logger` port.
 */

#include "logger_default/cr/logger_default_cr_api.h"

#include "internal/logger_default_handle.h"
#include "internal/logger_default_utc_timestamp.h"

#include "stream/borrowers/stream_borrowers_api.h"

#include "osal/str/osal_str.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_inttypes.h"
#include "policy/lexleo_assert.h"

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
 * Borrowed target stream receiving the timestamp prefix.
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
	const osal_time_ops_t *time_ops
) {
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
		osal_snprintf(
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
 * A required stream write failed or was partial, timestamp formatting failed,
 * or the target stream could not be flushed.
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
		   logger_default->mem_ops
		&& logger_default->mem_ops->free
	);

	logger_default->mem_ops->free(logger_default);

	return LOGGER_STATUS_OK;
}

static const logger_vtbl_t g_logger_default_vtbl = {
	.log = logger_default_log,
	.destroy = logger_default_destroy
};

const logger_vtbl_t *logger_default_vtbl(void)
{
	return &g_logger_default_vtbl;
}

logger_default_cfg_t logger_default_default_cfg(void)
{
	return (logger_default_cfg_t) { .append_newline = true };
}

logger_default_env_t logger_default_default_env(
	stream_t *stream,
	const osal_time_ops_t *time_ops,
	const osal_mem_ops_t *mem_ops
) {
	LEXLEO_ASSERT(stream && time_ops && mem_ops);

	return (logger_default_env_t){
		.stream = stream,
		.time_ops = time_ops,
		.mem_ops = mem_ops
	};
}

logger_default_status_t logger_default_create(
	logger_default_t **out,
	const logger_default_env_t *env
) {
	LEXLEO_ASSERT(
		   out
		&& env
		&& env->mem_ops
		&& env->mem_ops->calloc
	);

	logger_default_t *tmp = env->mem_ops->calloc(1, sizeof(*tmp));
	if (!tmp) {
		return LOGGER_DEFAULT_STATUS_OOM;
	}

	tmp->stream = env->stream;
	tmp->time_ops = env->time_ops;
	tmp->mem_ops = env->mem_ops;

	*out = tmp;
	return LOGGER_DEFAULT_STATUS_OK;
}

logger_default_status_t logger_default_complete_default_init(
	logger_default_t *logger_default,
	const logger_default_cfg_t *cfg
) {
	LEXLEO_ASSERT(logger_default && cfg);

	logger_default->append_newline = cfg->append_newline;

	return LOGGER_DEFAULT_STATUS_OK;
}
