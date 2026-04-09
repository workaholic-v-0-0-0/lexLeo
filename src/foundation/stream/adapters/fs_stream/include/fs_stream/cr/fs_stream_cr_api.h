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
 * This header exposes the CR-facing entry points used to:
 * - build default `fs_stream` configuration and environment values,
 * - create a file-backed stream instance,
 * - build an adapter descriptor suitable for registration in the `stream`
 *   factory.
 */

#ifndef LEXLEO_FS_STREAM_CR_API_H
#define LEXLEO_FS_STREAM_CR_API_H

#include "fs_stream/cr/fs_stream_types.h"

#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_key_type.h"
#include "stream/adapters/stream_install.h"
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
 * @return
 * A well-formed default `fs_stream_cfg_t`.
 */
fs_stream_cfg_t fs_stream_default_cfg(void);

/**
 * @brief Build a default environment for the `fs_stream` adapter.
 *
 * @details
 * This helper packages borrowed dependencies into a value-initialized
 * `fs_stream_env_t`.
 *
 * The returned environment does not take ownership of any pointer passed as
 * argument.
 *
 * @param[in] file_env
 * Borrowed OSAL file environment.
 *
 * @param[in] file_ops
 * Borrowed OSAL file operations table.
 *
 * @param[in] port_env
 * Borrowed `stream` port environment.
 *
 * @return
 * A well-formed `fs_stream_env_t` aggregating the provided dependencies.
 */
fs_stream_env_t fs_stream_default_env(
	const osal_file_env_t *file_env,
	const osal_file_ops_t *file_ops,
	const stream_env_t *port_env);

/**
 * @brief Create a file-backed stream instance.
 *
 * @details
 * This function allocates and initializes a public `stream_t` whose backend
 * is an OSAL file opened according to the parameters provided in
 * `fs_stream_args_t`.
 *
 * The underlying OSAL file is opened through the file operations injected in
 * the adapter environment.
 *
 * Ownership:
 * - On success, a newly allocated stream is returned in `*out`.
 * - The caller becomes responsible for destroying it via `stream_destroy()`.
 * - On failure, `*out` is left unchanged.
 *
 * @param[out] out
 * Receives the created stream handle.
 * Must not be `NULL`.
 *
 * @param[in] args
 * Creation arguments describing the file to open.
 * Must not be `NULL`.
 *
 * Contract:
 * - `args->path` must not be `NULL` and must not be an empty string.
 * - `args->flags` must not be zero.
 *
 * @param[in] cfg
 * Adapter configuration.
 * Must not be `NULL`.
 *
 * @param[in] env
 * Adapter environment containing injected dependencies.
 * Must not be `NULL`.
 *
 * @retval STREAM_STATUS_OK
 * Stream successfully created.
 *
 * @retval STREAM_STATUS_INVALID
 * One or more arguments are invalid.
 *
 * @retval STREAM_STATUS_OOM
 * Memory allocation failed.
 *
 * @retval STREAM_STATUS_IO_ERROR
 * The underlying OSAL file operation failed.
 */
stream_status_t fs_stream_create_stream(
	stream_t **out,
	const fs_stream_args_t *args,
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
 * Memory policy:
 * - `mem` must be the allocator that will later be used to destroy the
 *   descriptor-owned user data through `ud_dtor`.
 *
 * @param[out] out
 * Descriptor to initialize.
 * Must not be `NULL`.
 *
 * On failure, if `out != NULL`, `*out` is reset to an empty descriptor.
 *
 * @param[in] key
 * Adapter key used by the factory to select this adapter.
 * Must not be `NULL` and must not be an empty string.
 *
 * @param[in] cfg
 * Adapter configuration to bind into the descriptor.
 * Must not be `NULL`.
 *
 * @param[in] env
 * Adapter environment to bind into the descriptor.
 * Must not be `NULL`.
 *
 * @param[in] mem
 * Memory operations used to allocate descriptor-owned user data.
 * Must not be `NULL`.
 *
 * @retval STREAM_STATUS_OK
 * Descriptor successfully created.
 *
 * @retval STREAM_STATUS_INVALID
 * One or more arguments are invalid.
 *
 * @retval STREAM_STATUS_OOM
 * Allocation of descriptor-owned user data failed.
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

#endif // LEXLEO_FS_STREAM_CR_API_H
