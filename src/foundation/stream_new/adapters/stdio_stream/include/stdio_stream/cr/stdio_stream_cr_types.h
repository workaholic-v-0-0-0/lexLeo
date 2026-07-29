/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stdio_stream_cr_types.h
 * @ingroup stdio_stream_cr_api
 * @brief Public types used by the `stdio_stream` Composition Root API.
 *
 * @details
 * Declares the configuration and external borrowed dependency types used to
 * construct and register the `stdio_stream` adapter from a Composition Root.
 */

#ifndef LEXLEO_STDIO_STREAM_TYPES_H
#define LEXLEO_STDIO_STREAM_TYPES_H

#include "stream/adapters/stream_adapters_types.h"

#include "osal/stdio/osal_stdio_ops.h"

#include "osal/mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct stdio_stream_t stdio_stream_t;

/**
 * @struct stdio_stream_cfg_t
 * @ingroup stdio_stream_cr_api
 * @brief Configuration values for the `stdio_stream` adapter registration.
 *
 * @details
 * This structure groups the CR-provided configuration values used when
 * constructing `stdio_stream`-related objects.
 */
typedef struct stdio_stream_cfg_t {
	/**
	 * @brief Reserved configuration field.
	 *
	 * @details Reserved for future extensions.
	 */
	int reserved;
} stdio_stream_cfg_t;

/**
 * @struct stdio_stream_env_t
 * @ingroup stdio_stream_cr_api
 * @brief External borrowed dependencies required to construct the
 * `stdio_stream` backend.
 */
typedef struct stdio_stream_env_t {

	/** Borrowed standard stream operations table. */
	const osal_stdio_ops_t *stdio_ops;

	/** Borrowed memory operations. */
	const osal_mem_ops_t *mem_ops;

} stdio_stream_env_t;

/**
 * @enum stdio_stream_status_t
 * @ingroup stdio_stream_cr_api
 * @brief Status values returned by the `stdio_stream` API.
 */
typedef enum {
	STDIO_STREAM_STATUS_OK = 0,
	STDIO_STREAM_STATUS_OOM
} stdio_stream_status_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STDIO_STREAM_TYPES_H */
