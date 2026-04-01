/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream_types.h
 * @ingroup dynamic_buffer_stream_cr_api
 * @brief Types used by the `dynamic_buffer_stream` Composition Root API.
 *
 * @details
 * This header exposes the public configuration and environment types used by
 * the `dynamic_buffer_stream` adapter CR-facing services.
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_TYPES_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_TYPES_H

#include "stream/adapters/stream_env.h"
#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_cstd_types.h"

/**
 * @struct dynamic_buffer_stream_cfg_t
 * @ingroup dynamic_buffer_stream_cr_api
 * @brief Configuration type for the `dynamic_buffer_stream` adapter.
 *
 * @details
 * This structure carries the CR-provided configuration values used when
 * constructing `dynamic_buffer_stream`-related objects.
 */
typedef struct dynamic_buffer_stream_cfg_t {
	/**
	 * @brief Default initial capacity of the internal dynamic buffer.
	 *
	 * @details
	 * This capacity is expressed in bytes and is used when allocating the
	 * adapter-managed in-memory buffer during stream creation.
	 */
	size_t default_cap;
} dynamic_buffer_stream_cfg_t;

/**
 * @struct dynamic_buffer_stream_env_t
 * @ingroup dynamic_buffer_stream_cr_api
 * @brief Injected dependencies for the `dynamic_buffer_stream` adapter.
 *
 * @details
 * This structure aggregates the borrowed runtime dependencies provided by the
 * Composition Root and required by `dynamic_buffer_stream` construction
 * services.
 */
typedef struct dynamic_buffer_stream_env_t {
	/**
	 * @brief Borrowed memory operations table for adapter-owned allocations.
	 *
	 * @details
	 * This table is used for backend allocation, buffer allocation, buffer
	 * growth, and backend destruction.
	 */
	const osal_mem_ops_t *mem;

	/**
	 * @brief Borrowed `stream` port environment.
	 *
	 * @details
	 * This environment is forwarded to `stream_create()` when constructing the
	 * public `stream_t` handle.
	 */
	stream_env_t port_env;
} dynamic_buffer_stream_env_t;

#endif // LEXLEO_DYNAMIC_BUFFER_STREAM_TYPES_H
