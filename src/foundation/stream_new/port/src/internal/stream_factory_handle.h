/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_factory_handle.h
 * @ingroup stream_internal_group
 * @brief Private stream factory handle definition.
 *
 * @details
 * Defines the private adapter provider registry, stream provider, and factory
 * state used by the `stream` factory implementation.
 */

#ifndef LEXLEO_STREAM_FACTORY_HANDLE_H
#define LEXLEO_STREAM_FACTORY_HANDLE_H

#include "stream/common/stream_adapter_id_type.h"
#include "stream/common/stream_opaque_type.h"
#include "stream/adapters/stream_adapters_types.h"

#include "osal/mem/osal_mem_types.h"

#include "policy/lexleo_cstd_types.h"

/**
 * @brief Adapter provider entry registered in a stream factory.
 *
 * @details
 * Associates an adapter identifier with the corresponding adapter provider.
 */
typedef struct stream_adapter_provider_entry_t {

	/** Borrowed adapter identifier. */
	stream_adapter_id_t key;

	/** Owned adapter provider. */
	stream_adapter_provider_t *value;

} stream_adapter_provider_entry_t;

/**
 * @brief Private adapter provider registry used by `stream_factory_t`.
 *
 * @details
 * Stores the adapter provider entries registered with the factory in a
 * bounded linear array.
 */
typedef struct stream_registry_t {

	/** Storage for registered adapter provider entries. */
	stream_adapter_provider_entry_t *entries;

	/** Number of valid registered entries. */
	size_t count;

	/** Maximum number of entries that can be stored. */
	size_t cap;

} stream_registry_t;

/**
 * @brief Construct a complete stream handle.
 *
 * @param[in] ud Constructor-specific user data.
 * @param[in] vtbl Stream adapter dispatch table.
 * @param[in,out] backend Adapter backend whose ownership is transferred to
 * the created stream on success.
 * @param[out] out Receives the created stream handle.
 *
 * @return Stream construction status.
 */
typedef stream_status_t (*stream_ctor_fn_t)(
	const void *ud,
	const stream_vtbl_t *vtbl,
	void *backend,
	stream_t **out
);

/**
 * @brief Private stream provider.
 *
 * @details
 * Groups the constructor and constructor-specific context required by the
 * factory to construct complete `stream_t` handles.
 */
typedef struct stream_provider_t {

	/** Constructs a complete stream handle. */
	stream_ctor_fn_t ctor;

	/** Constructor-specific user data. */
	void *ud;

	/** Optional destructor for provider-owned user data. */
	stream_ctor_ud_dtor_fn_t ud_dtor;

} stream_provider_t;

/**
 * @brief Private handle structure for a `stream_factory_t`.
 *
 * @details
 * Defines the private in-memory representation of a `stream_factory_t`.
 */
struct stream_factory_t {

	/** Adapter provider registry. */
	stream_registry_t reg;

	/** Stream provider used to construct complete stream handles. */
	stream_provider_t stream_provider;

	/** Memory operations used to destroy the factory handle. */
	const osal_mem_ops_t *mem;

};

#endif /* LEXLEO_STREAM_FACTORY_HANDLE_H */
