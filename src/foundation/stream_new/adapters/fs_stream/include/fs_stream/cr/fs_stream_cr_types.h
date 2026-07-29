/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_cr_types.h
 * @ingroup fs_stream_cr_api
 * @brief Public types used by the `fs_stream` Composition Root API.
 *
 * @details
 * Declares the configuration and borrowed dependency types used to construct
 * `fs_stream` backends and register the adapter from a Composition Root.
 */

#ifndef LEXLEO_FS_STREAM_TYPES_H
#define LEXLEO_FS_STREAM_TYPES_H

#include "osal/file/osal_file_ops.h"
#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fs_stream_t fs_stream_t;

/**
 * @struct fs_stream_cfg_t
 * @ingroup fs_stream_cr_api
 * @brief Configuration values used to construct an `fs_stream` handle.
 *
 * @details
 * Groups the configuration values provided by the Composition Root when
 * constructing an `fs_stream` handle.
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
 * @brief Borrowed dependencies required to construct the `fs_stream` handle.
 */
typedef struct fs_stream_env_t {

	/** Borrowed file operations table. */
	const osal_file_ops_t *file_ops;

	/** Borrowed memory operations. */
	const osal_mem_ops_t *mem_ops;

} fs_stream_env_t;

/**
 * @enum fs_stream_status_t
 * @ingroup fs_stream_cr_api
 * @brief Status values returned by the `fs_stream` API.
 */
typedef enum {
	FS_STREAM_STATUS_OK = 0,
	FS_STREAM_STATUS_EOF,
	FS_STREAM_STATUS_INVALID,
	FS_STREAM_STATUS_NOT_FOUND,
	FS_STREAM_STATUS_IO_ERROR,
	FS_STREAM_STATUS_OOM
} fs_stream_status_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_FS_STREAM_TYPES_H */
