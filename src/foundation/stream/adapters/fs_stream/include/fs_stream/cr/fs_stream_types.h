/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_types.h
 * @ingroup fs_stream_cr_api
 * @brief Types used by the fs_stream Composition Root API.
 */

#ifndef LEXLEO_FS_STREAM_TYPES_H
#define LEXLEO_FS_STREAM_TYPES_H

#include "stream/adapters/stream_env.h"
#include "osal/file/osal_file_ops.h"
#include "osal/file/osal_file_env.h"
#include "policy/lexleo_cstd_types.h"

/**
 * @struct fs_stream_cfg_t
 * @ingroup fs_stream_cr_api
 * @brief fs_stream configuration.
 */
typedef struct fs_stream_cfg_t {
	int reserved; /**< Reserved for future extensions. */
} fs_stream_cfg_t;

/**
 * @struct fs_stream_env_t
 * @ingroup fs_stream_cr_api
 * @brief Injected dependencies for the fs_stream adapter.
 *
 * Borrowed dependencies provided by the Composition Root.
 *
 * - file_env.mem is used for backend allocation
 * - file_ops provides OSAL file operations
 * - port_env is forwarded to stream_create()
 */
typedef struct fs_stream_env_t {
	osal_file_env_t file_env;
	const osal_file_ops_t *file_ops;
	stream_env_t port_env;
} fs_stream_env_t;

/**
 * @struct fs_stream_args_t
 * @ingroup fs_stream_cr_api
 * @brief Arguments provided at stream creation time.
 */
typedef struct fs_stream_args_t {
    const char *path; /**< UTF-8 path. */
    uint32_t flags;   /**< OSAL file open flags. */
    bool autoclose;   /**< Close OSAL file on stream close. */
} fs_stream_args_t;

#endif //LEXLEO_FS_STREAM_TYPES_H
