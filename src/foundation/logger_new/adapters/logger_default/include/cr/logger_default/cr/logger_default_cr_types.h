/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_default_cr_types.h
 * @ingroup logger_default_cr_api
 * @brief Public types used by the `logger_default` Composition Root API.
 *
 * @details
 * Declares the configuration, external borrowed dependency, and status types
 * used to construct and complete the initialization of `logger_default`
 * backends from a Composition Root.
 */

#ifndef LEXLEO_LOGGER_DEFAULT_CR_TYPES_H
#define LEXLEO_LOGGER_DEFAULT_CR_TYPES_H

#include "stream/borrowers/stream_borrowers_api.h"

#include "osal/time/osal_time_ops.h"
#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct logger_default_t logger_default_t;

/**
 * @struct logger_default_cfg_t
 * @ingroup logger_default_cr_api
 * @brief Configuration values used to complete the initialization of a
 * `logger_default_t`.
 *
 * @details
 * Groups the configuration values provided by the Composition Root when
 * completing the initialization of a `logger_default` backend.
 */
typedef struct logger_default_cfg_t {
	/**
	 * @brief Whether the adapter appends a trailing newline to emitted messages.
	 */
	bool append_newline;
} logger_default_cfg_t;

/**
 * @struct logger_default_env_t
 * @ingroup logger_default_cr_api
 * @brief External borrowed dependencies required to construct the
 * `logger_default` adapter.
 */
typedef struct logger_default_env_t {
	/**
	 * @brief Borrowed target stream used by the adapter.
	 *
	 * @details
	 * The adapter writes emitted log messages to this stream.
	 * The stream remains owned by the caller and is not destroyed by the
	 * `logger_default` adapter.
	 */
	stream_t *stream;

	/** Borrowed time operations table used by the adapter backend. */
	const osal_time_ops_t *time_ops;

	/** Borrowed memory operations. */
	const osal_mem_ops_t *mem_ops;

} logger_default_env_t;

/**
 * @enum logger_default_status_t
 *
 * @brief Status values returned by the `logger_default` API.
 */
typedef enum logger_default_status_t {
	LOGGER_DEFAULT_STATUS_OK = 0,
	LOGGER_DEFAULT_STATUS_OOM,
	LOGGER_DEFAULT_STATUS_IO_ERROR
} logger_default_status_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_DEFAULT_CR_TYPES_H */
