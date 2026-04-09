/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_install.h
 * @ingroup stream_adapters_api
 * @brief Adapter installation contracts for the `stream` port.
 *
 * @details
 * This header exposes the public descriptor and constructor contracts used to
 * register concrete stream adapters into Composition Root factory services.
 */

#ifndef LEXLEO_STREAM_INSTALL_H
#define LEXLEO_STREAM_INSTALL_H

#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_key_type.h"

#include "osal/mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Destructor for adapter-owned constructor user data.
 *
 * @param[in] ud
 * Opaque adapter-owned user data to destroy.
 *
 * @param[in] mem
 * Memory operations to use for destruction.
 *
 * @details
 * This callback is used by factory-side services when they own adapter
 * registration data and must release descriptor-bound user data during
 * teardown.
 */
typedef void (*ud_dtor_fn_t)(const void *ud, const osal_mem_ops_t *mem);

/**
 * @brief Adapter constructor contract used by stream factory services.
 *
 * @param[in] ud
 * Opaque adapter-owned constructor context.
 *
 * @param[in] args
 * Adapter-specific creation arguments.
 *
 * @param[out] out
 * Receives the created stream handle.
 *
 * @return
 * A `stream_status_t` describing the construction result.
 *
 * @details
 * Concrete stream adapters must implement this constructor signature so they
 * can be registered through public `stream_adapter_desc_t` descriptors and
 * later invoked by factory-based creation services.
 */
typedef stream_status_t (*stream_ctor_fn_t)(
	const void *ud,
	const void *args,
	stream_t **out);

/**
 * @brief Public descriptor used to register a concrete stream adapter.
 *
 * @details
 * A `stream_adapter_desc_t` binds:
 * - a public adapter key,
 * - an adapter constructor,
 * - optional constructor user data,
 * - an optional destructor for that user data.
 *
 * This descriptor is typically prepared by an adapter-facing CR helper and
 * then registered into a `stream_factory_t`.
 */
typedef struct stream_adapter_desc_t {
	/**
	 * @brief Public key used to identify the adapter.
	 *
	 * @details
	 * This key is later used by factory-based creation APIs to select the
	 * registered adapter.
	 */
	stream_key_t key;

	/**
	 * @brief Adapter constructor used to create a `stream_t`.
	 *
	 * @details
	 * This callback must implement the `stream_ctor_fn_t` contract.
	 */
	stream_ctor_fn_t ctor;

	/**
	 * @brief Optional opaque user data bound to the constructor.
	 *
	 * @details
	 * This pointer is passed back to `ctor` when the adapter is selected.
	 */
	const void *ud;

	/**
	 * @brief Optional destructor for `ud`.
	 *
	 * @details
	 * When provided, this callback is used to release descriptor-owned user
	 * data during factory teardown.
	 */
	ud_dtor_fn_t ud_dtor;
} stream_adapter_desc_t;

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_STREAM_INSTALL_H
