/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_factory_cr_api.h
 * @ingroup stream_cr_api
 * @brief Composition Root factory API for the `stream` port.
 *
 * @details
 * This header exposes the public factory-oriented Composition Root services
 * used to:
 * - create and destroy a `stream` factory,
 * - register adapter descriptors,
 * - create streams from registered adapter keys,
 * - build default factory configuration values.
 *
 * @note This API belongs to the Composition Root surface.
 * Runtime modules should not depend on it directly.
 */

#ifndef LEXLEO_STREAM_FACTORY_CR_API_H
#define LEXLEO_STREAM_FACTORY_CR_API_H

#include "stream/adapters/stream_install.h"
#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_key_type.h"
#include "stream/cr/stream_cr_api.h"
#include "osal/mem/osal_mem_ops.h"

#include <stddef.h>


/**
 * @brief Opaque stream factory type.
 *
 * @details
 * A `stream_factory_t` stores adapter descriptors indexed by public
 * `stream_key_t` values and can later create `stream_t` handles from them.
 */
typedef struct stream_factory_t stream_factory_t;

/**
 * @brief Configuration for `stream_factory_t`.
 *
 * @details
 * This structure defines the public configuration values required to create a
 * stream factory.
 */
typedef struct stream_factory_cfg_t {
	/** Maximum number of adapter descriptors that can be registered. */
	size_t fact_cap;
} stream_factory_cfg_t;

/**
 * @brief Create a stream factory.
 *
 * @param[out] out
 * Receives the created factory handle. Must not be `NULL`.
 *
 * @param[in] cfg
 * Factory configuration. Must not be `NULL`.
 *
 * @param[in] env
 * Factory environment. Must not be `NULL`.
 *
 * @return
 * - `STREAM_STATUS_OK` on success
 * - `STREAM_STATUS_INVALID` if an argument is invalid
 *
 * @post
 * On success, `*out` contains a valid `stream_factory_t *`.
 */
stream_status_t stream_create_factory(
	stream_factory_t **out,
	const stream_factory_cfg_t *cfg,
	const stream_env_t *env );

/**
 * @brief Destroy a stream factory.
 *
 * @param[in,out] fact
 * Address of the factory handle to destroy.
 *
 * @details
 * If `fact == NULL` or `*fact == NULL`, this function does nothing.
 * Otherwise, it releases the factory object and sets `*fact` to `NULL`.
 */
void stream_destroy_factory(stream_factory_t **fact);

/**
 * @brief Register an adapter descriptor into a stream factory.
 *
 * @param[in,out] fact
 * Target factory. Must not be `NULL`.
 *
 * @param[in] desc
 * Adapter descriptor to register. Must not be `NULL`.
 *
 * @return
 * - `STREAM_STATUS_OK` on success
 * - `STREAM_STATUS_INVALID` if an argument is invalid
 * - `STREAM_STATUS_ALREADY_EXISTS` if the key is already registered
 * - `STREAM_STATUS_FULL` if the factory has reached its registration capacity
 *
 * @details
 * On success, a later call to `stream_factory_create_stream()` with the same
 * key can resolve the registered descriptor.
 */
stream_status_t stream_factory_add_adapter(
	stream_factory_t *fact,
	const stream_adapter_desc_t *desc );

/**
 * @brief Create a stream from a registered adapter key.
 *
 * @param[in] f
 * Factory used for adapter lookup. Must not be `NULL`.
 *
 * @param[in] key
 * Registered adapter key. Must not be `NULL` and must not be empty.
 *
 * @param[in] args
 * Adapter-specific creation arguments. Must not be `NULL`.
 *
 * @param[out] out
 * Receives the created stream handle. Must not be `NULL`.
 *
 * @return
 * - `STREAM_STATUS_OK` on success
 * - `STREAM_STATUS_INVALID` if an argument is invalid
 * - `STREAM_STATUS_NOT_FOUND` if `key` is not registered
 * - any adapter-specific failure status propagated from the selected adapter
 *
 * @details
 * This function resolves the adapter descriptor registered under `key` and
 * delegates stream creation to its constructor.
 */
stream_status_t stream_factory_create_stream(
	const stream_factory_t *f,
	stream_key_t key,
	const void *args,
	stream_t **out );

/**
 * @brief Return the default configuration for a stream factory.
 *
 * @return
 * A well-formed default `stream_factory_cfg_t`.
 */
stream_factory_cfg_t stream_default_factory_cfg(void);

#endif //LEXLEO_STREAM_FACTORY_CR_API_H
