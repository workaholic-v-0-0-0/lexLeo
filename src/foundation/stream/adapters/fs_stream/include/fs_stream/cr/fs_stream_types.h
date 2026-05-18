/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_types.h
 * @ingroup fs_stream_cr_api
 * @brief Types used by the `fs_stream` Composition Root API.
 *
 * @details
 * This header exposes the public configuration, environment, and creation
 * argument types used by the `fs_stream` adapter CR-facing services.
 */

#ifndef LEXLEO_FS_STREAM_TYPES_H
#define LEXLEO_FS_STREAM_TYPES_H

#include "stream/adapters/stream_env.h"

#include "osal/file/osal_file_ops.h"
#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct fs_stream_cfg_t
 * @ingroup fs_stream_cr_api
 * @brief Configuration type for the `fs_stream` adapter.
 *
 * @details
 * This structure carries the CR-provided configuration values used when
 * constructing `fs_stream`-related objects.
 */
typedef struct fs_stream_cfg_t {
	/**
	 * @brief Reserved configuration field.
	 *
	 * @details
	 * Reserved for future extensions.
	 */
	int reserved;
} fs_stream_cfg_t;

/**
 * @struct fs_stream_env_t
 * @ingroup fs_stream_cr_api
 * @brief Injected dependencies for the `fs_stream` adapter.
 *
 * @details
 * This structure aggregates the borrowed runtime dependencies provided by the
 * Composition Root and required by `fs_stream` construction services.
 */
typedef struct fs_stream_env_t {

	/**
	 * @brief Borrowed OSAL file operations table.
	 *
	 * @details
	 * This table provides the OSAL file operations used by the adapter backend.
	 */
	const osal_file_ops_t *file_ops;

	/**
	 * @brief Borrowed memory operations used for adapter-backend allocation.
	 *
	 * @details
	 * These memory operations are used to allocate and destroy the private
	 * backend object owned by the `fs_stream` adapter.
	 */
	const osal_mem_ops_t *adapter_mem_ops;

	/**
	 * @brief Borrowed `stream` port environment.
	 *
	 * @details
	 * This environment is forwarded to `stream_create()` when constructing the
	 * public `stream_t` handle.
	 */
	stream_env_t port_env;

} fs_stream_env_t;

/**
 * @struct fs_stream_args_t
 * @ingroup fs_stream_cr_api
 * @brief Arguments provided when creating a file-backed stream.
 */
typedef struct fs_stream_args_t {
	/**
	 * @brief UTF-8 path of the target file.
	 */
	const char *path;

	/**
	 * @brief Portable OSAL file open mode.
	 *
	 * @details
	 * This string is forwarded to `osal_file_ops_t::open()`.
	 * Supported values are defined by the `osal_file` contract,
	 * currently `"rb"`, `"wb"`, and `"ab"`.
	 */
	const char *mode;

} fs_stream_args_t;

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_FS_STREAM_TYPES_H
