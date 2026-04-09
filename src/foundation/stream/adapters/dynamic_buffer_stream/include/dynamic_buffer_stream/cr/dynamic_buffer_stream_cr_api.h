/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream_cr_api.h
 * @ingroup dynamic_buffer_stream_cr_api
 * @brief Composition Root API for the `dynamic_buffer_stream` adapter.
 *
 * @details
 * This header exposes the CR-facing services used to configure, construct,
 * and register the `dynamic_buffer_stream` adapter.
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_CR_API_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_CR_API_H

#include "dynamic_buffer_stream/cr/dynamic_buffer_stream_types.h"

#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_key_type.h"
#include "stream/adapters/stream_install.h"
#include "stream/adapters/stream_adapters_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return the default configuration for the `dynamic_buffer_stream`
 * adapter.
 *
 * @return Default adapter configuration value.
 */
dynamic_buffer_stream_cfg_t dynamic_buffer_stream_default_cfg(void);

/**
 * @brief Return the default injected environment for the
 * `dynamic_buffer_stream` adapter.
 *
 * @param mem Borrowed memory operations table used by the adapter backend.
 * @param port_env Borrowed `stream` port environment to forward to
 *        `stream_create()`.
 *
 * @return Default adapter environment value.
 */
dynamic_buffer_stream_env_t dynamic_buffer_stream_default_env(
	const osal_mem_ops_t *mem,
	const stream_env_t *port_env);

/**
 * @brief Create a `dynamic_buffer_stream` instance directly.
 *
 * @param out Output location that receives the created `stream_t *` on
 *        success.
 * @param cfg Borrowed adapter configuration.
 * @param env Borrowed adapter environment.
 *
 * @retval STREAM_STATUS_OK The stream was created successfully.
 * @retval STREAM_STATUS_INVALID One or more arguments are invalid.
 * @retval STREAM_STATUS_OOM Memory allocation failed.
 */
stream_status_t dynamic_buffer_stream_create_stream(
	stream_t **out,
	const dynamic_buffer_stream_cfg_t *cfg,
	const dynamic_buffer_stream_env_t *env);

/**
 * @brief Build a `stream` adapter descriptor for the
 * `dynamic_buffer_stream` adapter.
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
stream_status_t dynamic_buffer_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const dynamic_buffer_stream_cfg_t *cfg,
	const dynamic_buffer_stream_env_t *env,
	const osal_mem_ops_t *mem);

/**
 * @brief Factory-compatible constructor callback for the
 * `dynamic_buffer_stream` adapter.
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
 */
stream_status_t dynamic_buffer_stream_ctor(
	const void *ud,
	const void *args,
	stream_t **out);

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_DYNAMIC_BUFFER_STREAM_CR_API_H
