/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_default_types.h
 * @ingroup logger_default_cr_api
 * @brief Types used by the `logger_default` Composition Root API.
 *
 * @details
 * This header exposes the public configuration and environment types used by
 * the `logger_default` adapter CR-facing services.
 */

#ifndef LEXLEO_LOGGER_DEFAULT_TYPES_H
#define LEXLEO_LOGGER_DEFAULT_TYPES_H

#include "stream/borrowers/stream_borrowers_types.h"

#include "osal/time/osal_time_ops.h"
#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct logger_default_cfg_t
 * @ingroup logger_default_cr_api
 * @brief Configuration type for the `logger_default` adapter.
 *
 * @details
 * This structure carries the CR-provided configuration values used when
 * constructing `logger_default`-related objects.
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
 * @brief Injected dependencies for the `logger_default` adapter.
 *
 * @details
 * This structure aggregates the borrowed runtime dependencies provided by the
 * Composition Root and required by `logger_default` construction services.
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

	/** Borrowed memory operations used by the adapter backend. */
	const osal_mem_ops_t *adapter_mem_ops;

	/** Borrowed memory operations used by the `logger` port handle. */
	const osal_mem_ops_t *port_mem_ops;

} logger_default_env_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_DEFAULT_TYPES_H */
