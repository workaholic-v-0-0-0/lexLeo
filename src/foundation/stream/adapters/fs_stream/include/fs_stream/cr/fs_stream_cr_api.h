/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_cr_api.h
 * @ingroup fs_stream_cr_api
 * @brief Composition Root API for wiring the fs_stream adapter into the stream factory.
 *
 * @details
 * This header exposes the CR-facing entry points used to:
 * - build default fs_stream configuration and environment values,
 * - create a file-backed stream instance,
 * - build an adapter descriptor suitable for registration in the stream factory.
 */

#ifndef LEXLEO_FS_STREAM_CR_API_H
#define LEXLEO_FS_STREAM_CR_API_H

#include "fs_stream/cr/fs_stream_types.h"
#include "stream/adapters/stream_adapters_api.h"
#include "stream/adapters/stream_install.h"
#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_key_type.h"
#include "osal/mem/osal_mem_ops.h"

/**
 * @ingroup fs_stream_cr_api
 * @brief Return a default configuration for fs_stream.
 *
 * This function is CR-friendly: it provides a sane baseline configuration
 * that the Composition Root may override.
 *
 * @return A value-initialized configuration structure.
 */
fs_stream_cfg_t fs_stream_default_cfg(void);

/**
 * @ingroup fs_stream_cr_api
 * @brief Build a default environment for fs_stream from injected dependencies.
 *
 * The returned environment is a value object aggregating borrowed dependencies.
 * It does not take ownership of any pointer passed as argument.
 *
 * @param[in] file_env Borrowed OSAL file environment (may be NULL depending on OSAL design).
 * @param[in] file_ops Borrowed OSAL file operations vtable (must not be NULL).
 * @param[in] port_env Borrowed stream port environment (must not be NULL).
 *
 * @return A value-initialized environment structure.
 */
fs_stream_env_t fs_stream_default_env(
	const osal_file_env_t *file_env,
	const osal_file_ops_t *file_ops,
	const stream_env_t *port_env );

/**
 * @ingroup fs_stream_cr_api
 * @brief Create a file-backed stream instance.
 *
 * This constructor allocates and initializes a @ref stream_t whose backend
 * is an OSAL file opened according to the parameters provided in
 * @ref fs_stream_args_t.
 *
 * The underlying OSAL file is opened using the OSAL file operations
 * provided through the adapter environment.
 *
 * Ownership:
 * - On success, a newly allocated stream is returned in `*out`.
 * - The caller becomes responsible for destroying it via @ref stream_destroy.
 * - On failure, `*out` is left unchanged.
 *
 * @param[out] out
 * Pointer to the output stream instance. Must not be NULL.
 *
 * @param[in] args
 * Creation arguments describing the file to open:
 * - `path` : UTF-8 path of the file (must not be NULL and must not be an empty string)
 * - `flags` : OSAL file open flags
 * - `autoclose` : whether the OSAL file must be closed when the stream is destroyed
 *
 * @param[in] cfg
 * Adapter configuration (must not be NULL).
 *
 * @param[in] env
 * Adapter environment containing injected dependencies
 * (e.g. OSAL file operations and memory allocator).
 *
 * @retval STREAM_STATUS_OK
 * Stream successfully created.
 *
 * @retval STREAM_STATUS_INVALID
 * One or more input pointers are NULL or arguments are invalid.
 *
 * @retval STREAM_STATUS_OOM
 * Memory allocation failed.
 *
 * @retval STREAM_STATUS_IO_ERROR
 * Underlying OSAL file operation failed (e.g. open failure).
 */
stream_status_t fs_stream_create_stream(
	stream_t **out,
	const fs_stream_args_t *args,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env );

/**
 * @ingroup fs_stream_cr_api
 * @brief Build a stream adapter descriptor for registering fs_stream in a factory.
 *
 * The Composition Root uses this function to build a @ref stream_adapter_desc_t
 * suitable for registration into the stream factory (e.g. via
 * @ref stream_factory_add_adapter).
 *
 * Memory policy:
 * - `mem` must be the allocator that will later be used to destroy the
 *   descriptor-owned user data through `ud_dtor`.
 *
 * @param[out] out
 * Descriptor to initialize. Must not be NULL.
 * On failure, if non-NULL, it is reset to an empty descriptor.
 *
 * @param[in] key
 * Adapter key used by the factory to select this adapter.
 * Must not be NULL and must not be an empty string.
 *
 * @param[in] cfg
 * Adapter configuration to bind into the descriptor.
 * Must not be NULL.
 *
 * @param[in] env
 * Adapter environment to bind into the descriptor.
 * Must not be NULL.
 *
 * @param[in] mem
 * Memory operations used to allocate descriptor-owned user data.
 * Must not be NULL.
 *
 * @retval STREAM_STATUS_OK
 * Descriptor successfully created.
 *
 * @retval STREAM_STATUS_INVALID
 * One or more input arguments are invalid, or `key` is an empty string.
 *
 * @retval STREAM_STATUS_OOM
 * Allocation of descriptor-owned user data failed.
 */
stream_status_t fs_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env,
	const osal_mem_ops_t *mem );

#endif // LEXLEO_FS_STREAM_CR_API_H
