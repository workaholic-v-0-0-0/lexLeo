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

#include "logger/adapters/logger_env.h"
#include "stream/borrowers/stream_types.h"
#include "osal/time/osal_time_ops.h"
#include "osal/mem/osal_mem_ops.h"
#include "policy/lexleo_cstd_types.h"

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

	/**
	 * @brief Borrowed time operations used for timestamp generation.
	 *
	 * @details
	 * These operations are used to obtain the current time when producing
	 * timestamped log records.
	 * The operations table remains owned by the caller and is not modified by
	 * the `logger_default` adapter.
	 */
	const osal_time_ops_t *time_ops;

	/**
	 * @brief Borrowed memory operations used for adapter-backend allocation.
	 *
	 * @details
	 * These memory operations are used to allocate and destroy the private
	 * backend object owned by the `logger_default` adapter.
	 */
	const osal_mem_ops_t *adapter_mem;

	/**
	 * @brief Borrowed `logger` port environment.
	 *
	 * @details
	 * This environment is forwarded to `logger_create()` when constructing the
	 * public `logger_t` handle.
	 */
	logger_env_t port_env;
} logger_default_env_t;

#endif // LEXLEO_LOGGER_DEFAULT_TYPES_H
