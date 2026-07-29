/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream_cr_types.h
 * @ingroup dynamic_buffer_stream_cr_api
 * @brief Public types used by the `dynamic_buffer_stream` Composition Root
 * API.
 *
 * @details
 * Declares the configuration and external borrowed dependency types used to
 * construct `dynamic_buffer_stream` backends and register the adapter from a
 * Composition Root.
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_CR_TYPES_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_CR_TYPES_H

#include "osal/mem/osal_mem_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dynamic_buffer_stream_t dynamic_buffer_stream_t;

/**
 * @struct dynamic_buffer_stream_cfg_t
 * @ingroup dynamic_buffer_stream_cr_api
 * @brief Configuration values used to complete the initialization of a
 * `dynamic_buffer_stream_t`.
 *
 * @details
 * Groups the configuration values provided by the Composition Root when
 * completing the initialization of a `dynamic_buffer_stream` backend.
 */
typedef struct dynamic_buffer_stream_cfg_t {

	/** Default initial capacity of the internal dynamic buffer. */
	size_t default_cap;

} dynamic_buffer_stream_cfg_t;

/**
 * @struct dynamic_buffer_stream_env_t
 *
 * @brief External borrowed dependencies required to construct the
 * `dynamic_buffer_stream` adapter.
 */
typedef struct dynamic_buffer_stream_env_t {

	/** Borrowed memory operations. */
	const osal_mem_ops_t *mem_ops;

} dynamic_buffer_stream_env_t;

/**
 * @enum dynamic_buffer_stream_status_t
 *
 * @brief Status values returned by the `dynamic_buffer_stream` API.
 */
typedef enum {
	DYNAMIC_BUFFER_STREAM_STATUS_OK = 0,
	DYNAMIC_BUFFER_STREAM_STATUS_OOM
} dynamic_buffer_stream_status_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_DYNAMIC_BUFFER_STREAM_CR_TYPES_H */
