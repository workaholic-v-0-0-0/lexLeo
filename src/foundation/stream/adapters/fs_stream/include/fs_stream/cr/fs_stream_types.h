/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_types.h
 * @ingroup fs_stream_cr_api
 * @brief Public types used by the `fs_stream` Composition Root API.
 *
 * @details
 * This header exposes the configuration and dependency-injection types used
 * to construct and register the `fs_stream` adapter from a Composition Root.
 */

#ifndef LEXLEO_FS_STREAM_TYPES_H
#define LEXLEO_FS_STREAM_TYPES_H

#include "stream/adapters/stream_adapters_api.h"

#include "osal/file/osal_file_ops.h"
#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct fs_stream_cfg_t
 * @ingroup fs_stream_cr_api
 * @brief Configuration values for the `fs_stream` adapter registration.
 *
 * @details
 * This structure groups the CR-provided configuration values used when
 * constructing `fs_stream`-related objects.
 */
typedef struct fs_stream_cfg_t {
	/**
	 * @brief Reserved configuration field.
	 *
	 * @details Reserved for future extensions.
	 */
	int reserved;
} fs_stream_cfg_t;

/**
 * @struct fs_stream_env_t
 * @ingroup fs_stream_cr_api
 * @brief Borrowed dependencies required to construct the `fs_stream` adapter.
 */
typedef struct fs_stream_env_t {

	/** Borrowed OSAL file operations table used by the adapter backend. */
	const osal_file_ops_t *file_ops;

	/** Borrowed memory operations used by the adapter backend. */
	const osal_mem_ops_t *adapter_mem_ops;

	/** Borrowed memory operations used by the `stream` port handle. */
	const osal_mem_ops_t *port_mem_ops;

} fs_stream_env_t;

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_FS_STREAM_TYPES_H
