/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_factory_handle.h
 * @ingroup stream_internal_group
 * @brief Private factory handle definition for the `stream` port.
 *
 * @details
 * This header exposes the private factory-side handle structures used by the
 * `stream` port implementation.
 */

#ifndef LEXLEO_STREAM_FACTORY_HANDLE_H
#define LEXLEO_STREAM_FACTORY_HANDLE_H

#include "stream/adapters/stream_key_type.h"
#include "stream/adapters/stream_install.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/str/osal_str_ops.h"

#include "policy/lexleo_cstd_types.h"

/**
 * @brief Private registered adapter entry.
 *
 * @details
 * A `stream_branch_t` is the private stored form of one
 * `stream_adapter_desc_t` inside the factory registry.
 */
typedef struct stream_branch_t {
	/** Registered adapter key. */
	stream_key_t key;

	/** Registered adapter constructor. */
	stream_ctor_fn_t ctor;

	/** Optional constructor user data. */
	const void *ud;

	/** Optional destructor for `ud`. */
	ud_dtor_fn_t ud_dtor;
} stream_branch_t;

/**
 * @brief Private adapter registry used by `stream_factory_t`.
 *
 * @details
 * This registry stores the currently registered adapter entries in a bounded
 * linear array.
 */
typedef struct stream_registry_t {
	/** Storage for registered entries. */
	stream_branch_t *entries;

	/** Number of valid registered entries. */
	size_t count;

	/** Maximum number of entries that can be stored. */
	size_t cap;
} stream_registry_t;

/**
 * @brief Private handle structure for a `stream_factory_t`.
 *
 * @details
 * This structure is the private in-memory handle bound to a public
 * `stream_factory_t` object.
 *
 * It stores:
 * - the internal adapter registry,
 * - the memory operations used for teardown,
 * - the string operations used for registry key lookup.
 */
struct stream_factory_t {
	/** Internal adapter registry. */
	stream_registry_t reg;

	/** Memory operations used to destroy the factory handle. */
	const osal_mem_ops_t *mem;

	/** String operations used to compare registry keys. */
	const osal_str_ops_t *str_ops;
};

#endif // LEXLEO_STREAM_FACTORY_HANDLE_H