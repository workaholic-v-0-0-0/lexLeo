/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream_cr_api.h
 * @ingroup dynamic_buffer_stream_cr_api
 * @brief Composition Root API for the `dynamic_buffer_stream` adapter.
 *
 * @details
 * This header exposes the CR-facing entry points used to build
 * `dynamic_buffer_stream` configuration and environment values, provide the
 * adapter entry point, and build a descriptor suitable for registration in a
 * `stream_factory`.
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_CR_API_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_CR_API_H

#include "dynamic_buffer_stream/cr/dynamic_buffer_stream_types.h"

#include "stream/adapters/stream_adapters_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return a default configuration for the `dynamic_buffer_stream`
 * adapter.
 *
 * @return A well-formed default `dynamic_buffer_stream_cfg_t`.
 */
dynamic_buffer_stream_cfg_t dynamic_buffer_stream_default_cfg(void);

/**
 * @brief Build a default environment for the `dynamic_buffer_stream` adapter.
 *
 * @param[in] adapter_mem_ops Borrowed memory operations used by the adapter backend.
 * @param[in] port_mem_ops Borrowed memory operations used by the `stream` port handle.
 *
 * @return A well-formed `dynamic_buffer_stream_env_t` initialized from the
 * provided dependencies.
 *
 * See contract:
 * - @ref specifications_dynamic_buffer_stream_default_env
 */
dynamic_buffer_stream_env_t dynamic_buffer_stream_default_env(
	const osal_mem_ops_t *adapter_mem_ops,
	const osal_mem_ops_t *port_mem_ops
);

/**
 * @brief Create a `dynamic_buffer_stream` instance directly.
 *
 * @param[out] out Receives the created stream handle.
 * @param[in] cfg Adapter configuration.
 * @param[in] env Adapter environment.
 *
 * See contract:
 * - @ref specifications_dynamic_buffer_stream_create_stream
 */
stream_status_t dynamic_buffer_stream_create_stream(
	stream_t **out,
	const dynamic_buffer_stream_cfg_t *cfg,
	const dynamic_buffer_stream_env_t *env);

/**
 * @brief Build an adapter descriptor for registering
 * `dynamic_buffer_stream` in a factory.
 *
 * @param[out] out Descriptor to initialize.
 * @param[in] key Adapter key used by the factory to select this adapter.
 * @param[in] cfg Adapter configuration to bind into the descriptor.
 * @param[in] env Adapter environment to bind into the descriptor.
 * @param[in] mem Memory operations used for descriptor-owned user data.
 *
 * See contract:
 * - @ref specifications_dynamic_buffer_stream_create_desc
 */
stream_status_t dynamic_buffer_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const dynamic_buffer_stream_cfg_t *cfg,
	const dynamic_buffer_stream_env_t *env,
	const osal_mem_ops_t *mem);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_DYNAMIC_BUFFER_STREAM_CR_API_H */
