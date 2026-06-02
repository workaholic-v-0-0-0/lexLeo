/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream_types.h
 * @ingroup dynamic_buffer_stream_cr_api
 * @brief Public types used by the `dynamic_buffer_stream` Composition Root
 * API.
 *
 * @details
 * This header exposes the configuration and dependency-injection types used
 * to construct and register the `dynamic_buffer_stream` adapter from a
 * Composition Root.
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_TYPES_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_TYPES_H

#include "stream/adapters/stream_adapters_types.h"

#include "osal/mem/osal_mem_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct dynamic_buffer_stream_cfg_t
 * @ingroup dynamic_buffer_stream_cr_api
 * @brief Configuration values for the `dynamic_buffer_stream` adapter
 * registration.
 *
 * @details
 * This structure groups the CR-provided configuration values used when
 * constructing `dynamic_buffer_stream`-related objects.
 */
typedef struct dynamic_buffer_stream_cfg_t {

	/** Default initial capacity of the internal dynamic buffer. */
	size_t default_cap;

} dynamic_buffer_stream_cfg_t;

/**
 * @struct dynamic_buffer_stream_env_t
 *
 * @brief Borrowed dependencies required to construct the
 * `dynamic_buffer_stream` adapter.
 */
typedef struct dynamic_buffer_stream_env_t {

	/** Borrowed memory operations used by the adapter backend. */
	const osal_mem_ops_t *adapter_mem_ops;

	/** Borrowed memory operations used by the stream port handle. */
	const osal_mem_ops_t *port_mem_ops;

} dynamic_buffer_stream_env_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_DYNAMIC_BUFFER_STREAM_TYPES_H */
