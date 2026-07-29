/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_cr_api.h
 * @ingroup fs_stream_cr_api
 * @brief Composition Root API for the `fs_stream` adapter.
 *
 * @details
 * Exposes the CR-facing entry points used to build `fs_stream`
 * configuration and environment values, construct and complete
 * `fs_stream` backends, and build keyed backend provider entries suitable
 * for registration in a stream factory.
 */

#ifndef LEXLEO_FS_STREAM_CR_API_H
#define LEXLEO_FS_STREAM_CR_API_H

#include "stream/adapters/stream_adapters_regular_file_contract.h"

#include "fs_stream/cr/fs_stream_cr_types.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/file/osal_file_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return the `stream` port dispatch table implemented by the
 * `fs_stream` adapter.
 *
 * @return
 * Borrowed pointer to the `fs_stream` adapter `stream_vtbl_t`.
 *
 * See contract:
 * - @ref specifications_fs_stream_vtbl "fs_stream_vtbl() specifications".
 */
const stream_vtbl_t *fs_stream_vtbl(void);

/**
 * @brief Return a default configuration for the `fs_stream` adapter.
 *
 * @details
 * This helper builds an environment from the borrowed dependencies required
 * by the `fs_stream` adapter.
 *
 * @return A well-formed default `fs_stream_cfg_t`.
 *
 * See contract:
 * - @ref specifications_fs_stream_default_cfg
 */
fs_stream_cfg_t fs_stream_default_cfg(void);

/**
 * @brief Return a default `fs_stream` environment.
 *
 * @param[in] file_ops Borrowed file operations.
 * @param[in] mem_ops Borrowed memory operations.
 *
 * @return
 * A well-formed `fs_stream_env_t`.
 *
 * See contract:
 * - @ref specifications_fs_stream_default_env
 */
fs_stream_env_t fs_stream_default_env(
	const osal_file_ops_t *file_ops,
	const osal_mem_ops_t *mem_ops);

/**
 * @brief Create an `fs_stream` backend.
 *
 * @param[out] out Receives the created backend.
 * @param[in] env Borrowed dependencies required to construct the backend.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_fs_stream_create
 */
fs_stream_status_t fs_stream_create(
	fs_stream_t **out,
	const fs_stream_env_t *env);

/**
 * @brief Complete the default initialization of an `fs_stream` backend.
 *
 * @param[in,out] fs_stream Backend to complete.
 * @param[in] cfg Backend configuration.
 * @param[in] args File creation arguments.
 *
 * @return Initialization status.
 *
 * See contract:
 * - @ref specifications_fs_stream_complete_default_init
 */
fs_stream_status_t fs_stream_complete_default_init(
	fs_stream_t *fs_stream,
	const fs_stream_cfg_t *cfg,
	const stream_regular_file_creator_args_t *args);

/**
 * @brief Create an adapter provider for the `fs_stream` adapter.
 *
 * @param[out] out Receives the created adapter provider.
 *
 * @param[in] cfg
 * Adapter configuration copied into the provider-owned constructor user data.
 *
 * @param[in] env
 * Borrowed dependencies copied into the provider-owned constructor user data.
 *
 * @return Adapter provider creation status.
 *
 * See contract:
 * - @ref specifications_fs_stream_create_adapter_provider
 *   "fs_stream_create_adapter_provider() specifications".
 */
fs_stream_status_t fs_stream_create_adapter_provider(
	stream_adapter_provider_t **out,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_FS_STREAM_CR_API_H */
