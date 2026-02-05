/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/fs_stream/include/fs_stream/cr/
 * fs_stream_cr_api.h
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
 * @defgroup fs_stream_cr_api fs_stream CR API
 * @ingroup stream
 * @brief Composition Root (CR) API for the fs_stream adapter.
 *
 * This API is intended to be used by the Composition Root only:
 * it wires the fs_stream adapter (file-backed stream) into the stream port
 * factory via a @ref stream_adapter_desc_t.
 *
 * Design notes:
 * - The adapter is created through the stream port factory using a constructor
 *   callback (@ref fs_stream_ctor).
 * - Runtime dependencies are injected via @ref fs_stream_env_t.
 * - Memory ownership rules are enforced through the factory memory ops:
 *   @ref fs_stream_create_desc requires the factory's @ref osal_mem_ops_t.
 *
 * @note This header is part of the CR surface. Runtime modules should not
 *       include it directly.
 * @{
 */

/**
 * @brief Return a default configuration for fs_stream.
 *
 * This function is CR-friendly: it provides a sane baseline configuration
 * that the Composition Root may override.
 *
 * @return A value-initialized configuration structure.
 */
fs_stream_cfg_t fs_stream_default_cfg(void);

/**
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
 * @brief Create a file-backed stream instance.
 *
 * This is the main constructor used by the adapter implementation.
 * It allocates and initializes a @ref stream_t whose backend is an OSAL file.
 *
 * Ownership:
 * - On success, `*out` is set to a newly created stream owned by the caller.
 * - On failure, `*out` is left untouched (recommended contract).
 *
 * @param[out] out Output stream instance. Must not be NULL.
 * @param[in]  args Adapter-specific arguments (path, flags, autoclose, etc.). Must not be NULL.
 * @param[in]  cfg  Adapter configuration. Must not be NULL.
 * @param[in]  env  Adapter environment (injected dependencies). Must not be NULL.
 *
 * @retval STREAM_STATUS_OK on success.
 * @retval STREAM_STATUS_INVALID_ARG if an input pointer is NULL or arguments are invalid.
 * @retval STREAM_STATUS_OOM if allocation fails.
 * @retval STREAM_STATUS_IO_ERROR (or similar) if OSAL file operations fail.
 */
stream_status_t fs_stream_create_stream(
	stream_t **out,
	const fs_stream_args_t *args,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env );

/**
 * @brief Factory constructor callback for fs_stream.
 *
 * This function matches the @ref stream_ctor_fn_t signature expected by the
 * stream port factory. It typically unpacks user data and forwards to
 * @ref fs_stream_create_stream.
 *
 * Contract:
 * - `ud` is an opaque pointer provided by the factory descriptor.
 * - `args` is the runtime argument blob passed to the factory (expected to be
 *   a pointer to @ref fs_stream_args_t).
 *
 * @param[in]  ud   Opaque user data provided by the descriptor (may be NULL).
 * @param[in]  args Runtime args blob (expected: `const fs_stream_args_t*`). Must not be NULL.
 * @param[out] out  Output stream instance. Must not be NULL.
 *
 * @return A @ref stream_status_t value.
 */
stream_status_t fs_stream_ctor(
	void *ud,
	const void *args,
	stream_t **out );

/**
 * @brief Build a stream adapter descriptor for registering fs_stream in a factory.
 *
 * The Composition Root uses this to produce a @ref stream_adapter_desc_t to be
 * registered with @ref stream_factory_add_adapter (or equivalent).
 *
 * Memory policy:
 * - `mem` MUST be the factory's memory ops (i.e., the same allocator the factory
 *   will later use to destroy descriptor-owned user data through `ud_dtor`).
 *
 * @param[out] out Descriptor to fill. Must not be NULL.
 * @param[in]  key Adapter key used by the factory to select this adapter. Must not be NULL.
 * @param[in]  cfg Adapter configuration to bind into the descriptor. Must not be NULL.
 * @param[in]  env Adapter environment to bind into the descriptor. Must not be NULL.
 * @param[in]  mem Factory memory ops. Must not be NULL.
 *
 * @retval STREAM_STATUS_OK on success.
 * @retval STREAM_STATUS_INVALID_ARG if an input pointer is NULL.
 * @retval STREAM_STATUS_OOM if descriptor user data allocation fails.
 */
stream_status_t fs_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env,
	const osal_mem_ops_t *mem ); // MUST BE THE FACTORY'S

/** @} */ /* fs_stream_cr_api */

#endif // LEXLEO_FS_STREAM_CR_API_H
