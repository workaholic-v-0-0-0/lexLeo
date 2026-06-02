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
 * This header exposes the CR-facing services used to configure, create, and
 * register the `stdio_stream` adapter.
 */

#ifndef LEXLEO_STDIO_STREAM_CR_API_H
#define LEXLEO_STDIO_STREAM_CR_API_H

#include "stdio_stream/cr/stdio_stream_types.h"

#include "stream/adapters/stream_adapters_api.h"

#include "osal/stdio/osal_stdio_ops.h"

#include "osal/mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return a default configuration for the `stdio_stream` adapter.
 *
 * @return A well-formed default `stdio_stream_cfg_t`.
 */
stdio_stream_cfg_t stdio_stream_default_cfg(void);

/**
 * @brief Build a default environment for the `stdio_stream` adapter.
 *
 * @param[in] stdio_ops OSAL stdio operations table.
 * @param[in] adapter_mem_ops Borrowed memory operations used by the adapter backend.
 * @param[in] port_mem_ops Borrowed memory operations used by the `stream` port handle.
 *
 * @return A well-formed `stdio_stream_env_t` initialized from the provided
 * dependencies.
 *
 * See contract:
 * - @ref specifications_stdio_stream_default_env
 */
stdio_stream_env_t stdio_stream_default_env(
	const osal_stdio_ops_t *stdio_ops,
	const osal_mem_ops_t *adapter_mem_ops,
	const osal_mem_ops_t *port_mem_ops);

/**
 * @brief Create a standard-I/O-backed stream instance.
 *
 * @param[out] out Receives the created stream handle.
 * @param[in] args Standard I/O stream selection arguments.
 * @param[in] cfg Adapter configuration.
 * @param[in] env Adapter environment.
 *
 * See contract:
 * - @ref specifications_stdio_stream_create_stream
 */
stream_status_t stdio_stream_create_stream(
	stream_t **out,
	const stream_io_creator_args_t *args,
	const stdio_stream_cfg_t *cfg,
	const stdio_stream_env_t *env);

/**
 * @brief Build an adapter descriptor for registering `stdio_stream` in a
 * factory.
 *
 * @param[out] out Descriptor to initialize.
 * @param[in] key Adapter key used by the factory to select this adapter.
 * @param[in] cfg Adapter configuration to bind into the descriptor.
 * @param[in] env Adapter environment to bind into the descriptor.
 * @param[in] mem Memory operations used for descriptor-owned user data.
 *
 * See contract:
 * - @ref specifications_stdio_stream_create_desc
 */
stream_status_t stdio_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const stdio_stream_cfg_t *cfg,
	const stdio_stream_env_t *env,
	const osal_mem_ops_t *mem);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STDIO_STREAM_CR_API_H */

