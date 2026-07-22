/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_cr_api.h
 * @ingroup fs_stream_cr_api
 * @brief Composition Root API for wiring the `fs_stream` adapter into the
 * `stream` factory.
 *
 * @details
 * This header exposes the CR-facing entry points used to build `fs_stream`
 * configuration and environment values, provide the adapter entry point used
 * by `stream_file_creator`, and build a descriptor suitable for registration
 * in a `stream_factory`.
 */

#ifndef LEXLEO_FS_STREAM_CR_API_H
#define LEXLEO_FS_STREAM_CR_API_H

#include "fs_stream/cr/fs_stream_types.h"

#include "stream/adapters/stream_adapters_api.h"

#include "osal/mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return a default configuration for the `fs_stream` adapter.
 *
 * @details
 * This helper provides a value-initialized baseline configuration that the
 * Composition Root may override.
 *
 * @return A well-formed default `fs_stream_cfg_t`.
 */
fs_stream_cfg_t fs_stream_default_cfg(void);

/**
 * @brief Build a default environment for the `fs_stream` adapter.
 *
 * @param[in] file_ops Borrowed OSAL file operations table.
 * @param[in] adapter_mem_ops Borrowed OSAL memory operations table.
 * @param[in] port_mem_ops Borrowed memory operations used by the `stream` port
 * handle.
 *
 * @return A well-formed `fs_stream_env_t` initialized from the provided
 * dependencies.
 *
 * See contract:
 * - @ref specifications_fs_stream_default_env
 */
fs_stream_env_t fs_stream_default_env(
	const osal_file_ops_t *file_ops,
	const osal_mem_ops_t *adapter_mem_ops,
	const osal_mem_ops_t *port_mem_ops);

/**
 * @brief Adapter entry point creating a file-backed stream instance.
 *
 * @details
 * Creates a `stream_t` backed by an OSAL file according to the provided
 * creation arguments.
 *
 * @param[out] out Receives the created stream handle.
 * @param[in] args File creation arguments.
 * @param[in] cfg Adapter configuration.
 * @param[in] env Adapter environment.
 *
 * See contract:
 * - @ref specifications_fs_stream_create_stream
 */
stream_status_t fs_stream_create_stream(
	stream_t **out,
	const stream_file_creator_args_t *args,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env);

/**
 * @brief Build an adapter descriptor for registering `fs_stream` in a factory.
 *
 * @details
 * The Composition Root uses this function to build a `stream_adapter_desc_t`
 * suitable for registration into the `stream` factory, for example via
 * `stream_factory_add_adapter()`.
 *
 * @param[out] out Descriptor to initialize.
 * @param[in] key Adapter key used by the factory to select this adapter.
 * @param[in] cfg Adapter configuration to bind into the descriptor.
 * @param[in] env Adapter environment to bind into the descriptor.
 * @param[in] mem Memory operations used for descriptor-owned user data.
 *
 * See contract:
 * - @ref specifications_fs_stream_create_desc
 */
stream_status_t fs_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env,
	const osal_mem_ops_t *mem);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_FS_STREAM_CR_API_H */
