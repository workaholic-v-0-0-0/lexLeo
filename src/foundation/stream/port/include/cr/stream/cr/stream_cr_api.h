/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_cr_api.h
 * @ingroup stream_cr_api
 * @brief Composition Root helpers for the `stream` port.
 *
 * @details
 * This header exposes the CR-facing helpers used to build default `stream`
 * port wiring objects.
 *
 * @note Runtime modules should not include this API directly.
 */

#ifndef LEXLEO_STREAM_CR_API_H
#define LEXLEO_STREAM_CR_API_H

#include "stream/adapters/stream_adapters_api.h"
#include "stream/cr/stream_creators_cr_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration for a stream factory.
 */
typedef struct stream_factory_cfg_t {

	/** Maximum number of adapter descriptors that can be registered. */
	size_t fact_cap;

} stream_factory_cfg_t;

/**
 * @brief Return a default stream factory configuration.
 *
 * @return
 * A well-formed `stream_factory_cfg_t`.
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
 * @param[in] env Factory environment.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_stream_create_factory
 */
stream_status_t stream_create_factory(
	stream_factory_t **out,
	const stream_factory_cfg_t *cfg,
	const osal_mem_ops_t *mem);

/**
 * @brief Destroy a stream factory.
 *
 * @param[in,out] fact Address of the factory handle to destroy.
 *
 * See contract:
 * - @ref specifications_stream_destroy_factory
 */
void stream_destroy_factory(stream_factory_t **fact);

/**
 * @brief Register an adapter descriptor into a stream factory.
 *
 * @param[in,out] fact Target factory.
 * @param[in] desc Adapter descriptor to register.
 *
 * @return Registration status.
 *
 * See contract:
 * - @ref specifications_stream_factory_add_adapter
 */
stream_status_t stream_factory_add_adapter(
	stream_factory_t *fact,
	const stream_adapter_desc_t *desc);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_CR_API_H */
