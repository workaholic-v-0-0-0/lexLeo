/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stdio_stream_cr_api.h
 * @ingroup stdio_stream_cr_api
 * @brief Composition Root API for wiring the `stdio_stream` adapter into the
 * `stream` factory.
 *
 * @details
 * This header exposes the CR-facing entry points used to:
 * - build default `stdio_stream` configuration and environment values,
 * - create `stream_t` handles backed by the process standard I/O streams
 *   (`stdin`, `stdout`, and `stderr`),
 * - build an adapter descriptor suitable for registration in the `stream`
 *   factory.
 */

#ifndef LEXLEO_STDIO_STREAM_CR_API_H
#define LEXLEO_STDIO_STREAM_CR_API_H

#include "stdio_stream/cr/stdio_stream_types.h"

#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_key_type.h"
#include "stream/adapters/stream_install.h"
#include "stream/adapters/stream_adapters_api.h"

#include "osal/stdio/osal_stdio_ops.h"

#include "osal/mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return a default configuration for the `stdio_stream` adapter.
 *
 * @details
 * This helper provides a value-initialized baseline configuration that the
 * Composition Root may override.
 *
 * @return
 * A well-formed default `stdio_stream_cfg_t`.
 */
stdio_stream_cfg_t stdio_stream_default_cfg(void);

/**
 * @brief Build a default environment for the `stdio_stream` adapter.
 *
 * @details
 * This helper packages borrowed dependencies into a value-initialized
 * `stdio_stream_env_t`.
 *
 * The returned environment does not take ownership of any pointer passed as
 * argument.
 *
 * @param stdio_ops
 * Borrowed OSAL stdio operations table used by the adapter backend to access
 * the standard streams and the stdio primitives required to implement the
 * `stream` port contract.
 * @param mem Borrowed memory operations table used by the adapter backend.
 * @param[in] port_env
 * Borrowed `stream` port environment.
 *
 * @return
 * A well-formed `stdio_stream_env_t` aggregating the provided dependencies.
 */
stdio_stream_env_t stdio_stream_default_env(
	const osal_stdio_ops_t *stdio_ops,
	const osal_mem_ops_t *mem,
	const stream_env_t *port_env);

/**
 * @brief Create a standard-I/O-backed stream instance.
 *
 * @details
 * This function allocates and initializes a public `stream_t` whose backend
 * wraps `stdin`, `stdout`, or `stderr` according to `args->kind`.
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
 * Creation arguments selecting which standard I/O stream is wrapped by the
 * created adapter instance.
 * Must not be `NULL`.
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
 */
stream_status_t stdio_stream_create_stream(
	stream_t **out,
	const stdio_stream_args_t *args,
	const stdio_stream_cfg_t *cfg,
	const stdio_stream_env_t *env);

/**
 * @brief Build a `stream` adapter descriptor for the `stdio_stream` adapter.
 *
 * @param out Output location that receives the constructed adapter
 *        descriptor on success.
 * @param key Borrowed adapter registration key.
 * @param cfg Borrowed adapter configuration copied into descriptor-owned
 *        constructor data.
 * @param env Borrowed adapter environment copied into descriptor-owned
 *        constructor data.
 * @param mem Borrowed memory operations table used to allocate the
 *        descriptor-owned constructor data.
 *
 * @retval STREAM_STATUS_OK The descriptor was created successfully.
 * @retval STREAM_STATUS_INVALID One or more arguments are invalid.
 * @retval STREAM_STATUS_OOM Memory allocation failed.
 *
 * @details
 * The descriptor produced by this function is intended to be registered into
 * a `stream` factory. The constructor user data allocated for that descriptor
 * is owned by the descriptor lifecycle and must therefore use the factory
 * memory operations.
 */
stream_status_t stdio_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const stdio_stream_cfg_t *cfg,
	const stdio_stream_env_t *env,
	const osal_mem_ops_t *mem);

/**
 * @brief Factory-compatible constructor callback for the
 * `stdio_stream` adapter.
 *
 * @param ud Borrowed adapter-specific constructor user data.
 * @param args Borrowed creation arguments expected by the adapter factory
 *        contract.
 * @param out Output location that receives the created `stream_t *` on
 *        success.
 *
 * @retval STREAM_STATUS_OK The stream was created successfully.
 * @retval STREAM_STATUS_INVALID One or more arguments are invalid.
 * @retval STREAM_STATUS_OOM Memory allocation failed.
 *
 * @details
 * This callback is intended to be stored in a `stream_adapter_desc_t`
 * and invoked through the `stream` factory contract.
 */
stream_status_t stdio_stream_ctor(
	const void *ud,
	const void *args,
	stream_t **out);

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_STDIO_STREAM_CR_API_H
