/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream_cr_api.h
 * @ingroup dynamic_buffer_stream_cr_api
 * @brief Composition Root API for the `dynamic_buffer_stream` adapter.
 *
 * @details
 * Exposes the CR-facing entry points used to build `dynamic_buffer_stream`
 * configuration and environment values, construct and complete
 * `dynamic_buffer_stream` backends, and build keyed backend provider entries
 * suitable for registration in a stream factory.
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_CR_API_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_CR_API_H

#include "stream/adapters/stream_adapters_types.h"

#include "dynamic_buffer_stream/cr/dynamic_buffer_stream_cr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return the `stream` port dispatch table implemented by the
 * `dynamic_buffer_stream` adapter.
 *
 * @return
 * Borrowed pointer to the `dynamic_buffer_stream` adapter `stream_vtbl_t`.
 *
 * See contract:
 * - @ref specifications_dynamic_buffer_stream_vtbl
 * "dynamic_buffer_stream_vtbl() specifications".
 */
const stream_vtbl_t *dynamic_buffer_stream_vtbl(void);

/**
 * @brief Return a default configuration for the `dynamic_buffer_stream`
 * adapter.
 *
 * @details
 * This helper builds the default configuration used to complete the
 * initialization of a `dynamic_buffer_stream` backend.
 *
 * @return A well-formed default `dynamic_buffer_stream_cfg_t`.
 *
 * See contract:
 * - @ref specifications_dynamic_buffer_stream_default_cfg
 */
dynamic_buffer_stream_cfg_t dynamic_buffer_stream_default_cfg(void);

/**
 * @brief Return a default `dynamic_buffer_stream` environment.
 *
 * @param[in] mem_ops Borrowed memory operations.
 *
 * @return A well-formed `dynamic_buffer_stream_env_t`.
 *
 * See contract:
 * - @ref specifications_dynamic_buffer_stream_default_env
 */
dynamic_buffer_stream_env_t dynamic_buffer_stream_default_env(
	const osal_mem_ops_t *mem_ops);

/**
 * @brief Create a `dynamic_buffer_stream` backend.
 *
 * @details
 * Initializes the backend's external borrowed dependencies from `env`.
 * Remaining initialization is completed separately through
 * `dynamic_buffer_stream_complete_default_init()`.
 *
 * @param[out] out Receives the created backend.
 * @param[in] env External borrowed dependencies required to construct the backend.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_dynamic_buffer_stream_create
 */
dynamic_buffer_stream_status_t dynamic_buffer_stream_create(
	dynamic_buffer_stream_t **out,
	const dynamic_buffer_stream_env_t *env);

/**
 * @brief Complete the default initialization of a
 * `dynamic_buffer_stream` backend.
 *
 * @param[in,out] dynamic_buffer_stream Backend to complete.
 * @param[in] cfg Backend configuration.
 *
 * @return Initialization status.
 *
 * See contract:
 * - @ref specifications_dynamic_buffer_stream_complete_default_init
 */
dynamic_buffer_stream_status_t dynamic_buffer_stream_complete_default_init(
	dynamic_buffer_stream_t *dynamic_buffer_stream,
	const dynamic_buffer_stream_cfg_t *cfg);

/**
 * @brief Create an adapter provider for the `dynamic_buffer_stream` adapter.
 *
 * @param[out] out Receives the created adapter provider.
 *
 * @param[in] cfg
 * Adapter configuration copied into the provider-owned constructor user data.
 *
 * @param[in] env
 * External borrowed dependencies copied into the provider-owned constructor
 * user data.
 *
 * @return Adapter provider creation status.
 *
 * See contract:
 * - @ref specifications_dynamic_buffer_stream_create_adapter_provider
 *   "dynamic_buffer_stream_create_adapter_provider() specifications".
 */
dynamic_buffer_stream_status_t dynamic_buffer_stream_create_adapter_provider(
	stream_adapter_provider_t **out,
	const dynamic_buffer_stream_cfg_t *cfg,
	const dynamic_buffer_stream_env_t *env);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_DYNAMIC_BUFFER_STREAM_CR_API_H */
