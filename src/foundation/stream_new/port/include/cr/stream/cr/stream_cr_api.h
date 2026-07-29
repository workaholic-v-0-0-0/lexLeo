/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_cr_api.h
 * @ingroup stream_cr_api
 * @brief Composition Root API for the `stream` port.
 *
 * @details
 * This header exposes the CR-facing operations used to configure and
 * construct `stream` port objects.
 *
 * @note Runtime modules should not include this API directly.
 */

#ifndef LEXLEO_STREAM_CR_API_H
#define LEXLEO_STREAM_CR_API_H

#include "stream/common/stream_vtbl_type.h"
#include "stream/cr/stream_cr_types.h"
#include "stream/cr/stream_creators_cr_api.h"

#include "osal/mem/osal_mem_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Build a default `stream_env_t`.
 *
 * @param[in] vtbl Stream adapter dispatch table.
 * @param[in] mem_ops Memory operations.
 *
 * @return A default `stream_env_t`.
 *
 * See contract:
 * - @ref specifications_stream_default_env
 */
stream_env_t stream_default_env(
	const stream_vtbl_t *vtbl,
	const osal_mem_ops_t *mem_ops
);

/**
 * @brief Create a stream handle.
 *
 * @param[out] out Receives the created stream handle.
 * @param[in] env Borrowed dependencies required to construct the stream
 * handle.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_stream_create
 */
stream_status_t stream_create(
	stream_t **out,
	const stream_env_t *env
);

/**
 * @brief Complete the default initialization of a stream handle.
 *
 * @param[in,out] stream Stream handle to complete.
 * @param[in,out] backend Adapter backend whose ownership is transferred to
 * the stream on success.
 *
 * @return Initialization status.
 *
 * @details
 * Completes initialization of a stream handle by binding an adapter backend
 * that was not injected during the preceding construction phase.
 *
 * The backend is adapter-specific and is therefore created outside the
 * `stream` port before being transferred to the stream handle.
 *
 * See contract:
 * - @ref specifications_stream_complete_default_init
 */
stream_status_t stream_complete_default_init(
	stream_t *stream,
	void *backend
);

/**
 * @brief Return the default stream factory configuration.
 *
 * @return A well-formed `stream_factory_cfg_t`.
 *
 * See contract:
 * - @ref specifications_stream_default_factory_cfg
 */
stream_factory_cfg_t stream_default_factory_cfg(void);

/**
 * @brief Create a stream factory.
 *
 * @param[out] out Receives the created factory handle.
 * @param[in] cfg Factory configuration.
 * @param[in] mem Borrowed memory operations used by the factory.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_stream_create_factory
 */
stream_factory_status_t stream_create_factory(
	stream_factory_t **out,
	const stream_factory_cfg_t *cfg,
	const osal_mem_ops_t *mem
);

/**
 * @brief Destroy a stream factory.
 *
 * @param[in,out] fact Address of the factory handle to destroy.
 *
 * See contract:
 * - @ref specifications_stream_destroy_factory
 */
void stream_destroy_factory(
	stream_factory_t **fact
);

/**
 * @brief Register an adapter provider with a stream factory.
 *
 * @param[in,out] fact Target factory.
 * @param[in] adapter_id Identifier associated with the adapter provider.
 * @param[in,out] adapter_provider Adapter provider whose ownership is
 * transferred to the factory on success.
 *
 * @return Registration status.
 *
 * See contract:
 * - @ref specifications_stream_factory_add_adapter
 */
stream_factory_status_t stream_factory_add_adapter(
	stream_factory_t *fact,
	stream_adapter_id_t adapter_id,
	stream_adapter_provider_t *adapter_provider
);

/**
 * @brief Destroy a stream adapter provider.
 *
 * @param[in] adapter_provider
 * Adapter provider whose ownership is consumed by this function.
 *
 * See contract:
 *
 * - @ref specifications_stream_destroy_adapter_provider
 *   "stream_destroy_adapter_provider() specifications".
 */
void stream_destroy_adapter_provider(
	stream_adapter_provider_t *adapter_provider
);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_CR_API_H */
