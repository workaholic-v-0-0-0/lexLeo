/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stdio_stream_types.h
 * @ingroup stdio_stream_cr_api
 * @brief Public types used by the `stdio_stream` Composition Root API.
 *
 * @details
 * This header exposes the configuration and dependency-injection types used
 * to construct and register the `stdio_stream` adapter from a Composition
 * Root.
 */

#ifndef LEXLEO_STDIO_STREAM_TYPES_H
#define LEXLEO_STDIO_STREAM_TYPES_H

#include "stream/adapters/stream_adapters_types.h"

#include "osal/stdio/osal_stdio_ops.h"

#include "osal/mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief Borrowed dependencies required to construct the `stdio_stream`
 * adapter.
 *
 * @details
 * The Composition Root provides these dependencies when building the adapter
 * descriptor registered in a `stream_factory`. Pointer fields remain borrowed
 * and must outlive the registered adapter descriptor.
 */
typedef struct stdio_stream_env_t {

	/** Borrowed OSAL stdio operations. */
	const osal_stdio_ops_t *stdio_ops;

	/** Borrowed memory operations used by the adapter backend. */
	const osal_mem_ops_t *adapter_mem_ops;

	/** Borrowed memory operations used by the `stream` port handle. */
	const osal_mem_ops_t *port_mem_ops;

} stdio_stream_env_t;

/**
 * @enum stdio_stream_kind_t
 * @ingroup stdio_stream_cr_api
 * @brief Standard I/O stream kind selected by `stdio_stream`.
 */
typedef enum stdio_stream_kind_t {

	/** Standard input stream. */
	STDIO_STREAM_KIND_STDIN = 0,

	/** Standard output stream. */
	STDIO_STREAM_KIND_STDOUT,

	/** Standard error stream. */
	STDIO_STREAM_KIND_STDERR,

	/** Number of supported standard I/O stream kinds. */
	STDIO_STREAM_KIND_COUNT

} stdio_stream_kind_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STDIO_STREAM_TYPES_H */
