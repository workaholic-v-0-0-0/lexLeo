/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stdio_stream_types.h
 * @ingroup stdio_stream_cr_api
 * @brief Types used by the `stdio_stream` Composition Root API.
 *
 * @details
 * This header exposes the public configuration, environment, and creation
 * argument types used by the `stdio_stream` adapter CR-facing services.
 */

#ifndef LEXLEO_STDIO_STREAM_TYPES_H
#define LEXLEO_STDIO_STREAM_TYPES_H

#include "stream/adapters/stream_env.h"

#include "osal/stdio/osal_stdio_ops.h"

#include "osal/mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct stdio_stream_cfg_t
 * @ingroup stdio_stream_cr_api
 * @brief Configuration type for the `stdio_stream` adapter.
 *
 * @details
 * This structure carries the CR-provided configuration values used when
 * constructing `stdio_stream`-related objects.
 */
typedef struct stdio_stream_cfg_t {
	/**
	 * @brief Reserved configuration field.
	 *
	 * @details
	 * Reserved for future extensions.
	 */
	int reserved;
} stdio_stream_cfg_t;

/**
 * @struct stdio_stream_env_t
 * @ingroup stdio_stream_cr_api
 * @brief Injected dependencies for the `stdio_stream` adapter.
 *
 * @details
 * This structure aggregates the borrowed runtime dependencies provided by the
 * Composition Root and required by `stdio_stream` construction services.
 */
typedef struct stdio_stream_env_t {
	/**
	 * @brief Borrowed OSAL stdio operations table.
	 *
	 * @details
	 * This table is used to access the borrowed standard streams and the stdio
	 * primitives required by the adapter backend to implement the borrower-side
	 * `stream` operations.
	 */
	const osal_stdio_ops_t *stdio_ops;

	/**
	 * @brief Borrowed memory operations table for adapter-owned allocations.
	 *
	 * @details
	 * This table is used for backend allocation.
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
} stdio_stream_env_t;

/**
 * @enum stdio_stream_kind_t
 * @ingroup stdio_stream_cr_api
 * @brief Kind of standard I/O stream requested from the `stdio_stream` adapter.
 *
 * @details
 * This enumeration identifies which standard C I/O stream must be wrapped by
 * a `stdio_stream` instance at creation time.
 */
typedef enum stdio_stream_kind_t {
	/**
	 * @brief Standard input stream.
	 */
	STDIO_STREAM_KIND_STDIN = 0,

	/**
	 * @brief Standard output stream.
	 */
	STDIO_STREAM_KIND_STDOUT,

	/**
	 * @brief Standard error stream.
	 */
	STDIO_STREAM_KIND_STDERR,

	/**
	 * @brief Number of supported standard I/O stream kinds.
	 *
	 * @details
	 * This value is not a valid runtime `stdio_stream_kind_t`.
	 * It is intended only for internal bounds checking and iteration.
	 */
	STDIO_STREAM_KIND_COUNT
} stdio_stream_kind_t;

/**
 * @struct stdio_stream_args_t
 * @ingroup stdio_stream_cr_api
 * @brief Creation arguments for the `stdio_stream` adapter.
 *
 * @details
 * This structure carries the per-instance creation arguments used to select
 * which standard C I/O stream is wrapped by the created `stream_t`.
 */
typedef struct stdio_stream_args_t {
	/**
	 * @brief Requested standard I/O stream kind.
	 *
	 * @details
	 * This field selects whether the created adapter instance wraps:
	 * - `stdin`,
	 * - `stdout`,
	 * - or `stderr`.
	 */
	stdio_stream_kind_t kind;
} stdio_stream_args_t;

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_STDIO_STREAM_TYPES_H
