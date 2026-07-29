/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stdio_stream_cr_api.h
 * @ingroup stdio_stream_cr_api
 * @brief Composition Root API for the `stdio_stream` adapter.
 *
 * @details
 * Exposes the CR-facing entry points used to build `stdio_stream`
 * configuration and environment values, construct backends from their
 * external borrowed dependencies, complete their initialization with internal
 * borrowed dependencies, and build adapter providers suitable for
 * registration in a `stream` factory.
 */

#ifndef LEXLEO_STDIO_STREAM_CR_API_H
#define LEXLEO_STDIO_STREAM_CR_API_H

#include "stream/adapters/stream_adapters_standard_stream_contract.h"

#include "stdio_stream/cr/stdio_stream_cr_types.h"

#include "osal/stdio/osal_stdio_ops.h"
#include "osal/mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return the `stream` port dispatch table implemented by the
 * `stdio_stream` adapter.
 *
 * @return
 * Borrowed pointer to the `stdio_stream` adapter `stream_vtbl_t`.
 *
 * See contract:
 * - @ref specifications_stdio_stream_vtbl "stdio_stream_vtbl() specifications".
 */
const stream_vtbl_t *stdio_stream_vtbl(void);

/**
 * @brief Return a default configuration for the `stdio_stream` adapter.
 *
 * @return A well-formed default `stdio_stream_cfg_t`.
 *
 * See contract:
 * - @ref specifications_stdio_stream_default_cfg
 */
stdio_stream_cfg_t stdio_stream_default_cfg(void);

/**
 * @brief Return a default `stdio_stream` environment.
 *
 * @details
 * This helper builds an environment from the external borrowed dependencies
 * required by the `stdio_stream` adapter.
 *
 * @param[in] stdio_ops External borrowed stdio operations.
 * @param[in] mem_ops External borrowed memory operations.
 *
 * @return
 * A well-formed `stdio_stream_env_t`.
 *
 * See contract:
 * - @ref specifications_stdio_stream_default_env
 */
stdio_stream_env_t stdio_stream_default_env(
    const osal_stdio_ops_t *stdio_ops,
    const osal_mem_ops_t *mem_ops);

/**
 * @brief Create a `stdio_stream` backend.
 *
 * @param[out] out Receives the created backend.
 * @param[in] env External borrowed dependencies required to construct the
 * backend.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_stdio_stream_create
 */
stdio_stream_status_t stdio_stream_create(
    stdio_stream_t **out,
    const stdio_stream_env_t *env);

/**
 * @brief Complete the default initialization of a `stdio_stream` backend.
 *
 * @details
 * Resolves and attaches the backend's internal borrowed standard stream
 * dependency when it has not already been provided.
 *
 * @param[in,out] stdio_stream Backend to complete.
 * @param[in] cfg Backend configuration.
 * @param[in] args Standard stream creation arguments.
 *
 * @return Initialization status.
 *
 * See contract:
 * - @ref specifications_stdio_stream_complete_default_init
 */
stdio_stream_status_t stdio_stream_complete_default_init(
    stdio_stream_t *stdio_stream,
    const stdio_stream_cfg_t *cfg,
    const stream_standard_stream_creator_args_t *args);

/**
 * @brief Create an adapter provider for the `stdio_stream` adapter.
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
 * - @ref specifications_stdio_stream_create_adapter_provider
 *   "stdio_stream_create_adapter_provider() specifications".
 */
stdio_stream_status_t stdio_stream_create_adapter_provider(
    stream_adapter_provider_t **out,
    const stdio_stream_cfg_t *cfg,
    const stdio_stream_env_t *env);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STDIO_STREAM_CR_API_H */
