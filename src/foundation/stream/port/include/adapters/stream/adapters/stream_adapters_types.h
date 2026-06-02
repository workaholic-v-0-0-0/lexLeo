/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_adapters_types.h
 * @ingroup stream_adapters_api
 * @brief Adapter-facing types for the `stream` port.
 *
 * @details
 * This header defines the types used by `stream` adapter implementations.
 *
 * It provides:
 * - the adapter environment @ref stream_env_t,
 * - adapter constructor and user-data callback types,
 * - the @ref stream_adapter_desc_t registration descriptor.
 *
 * Backend operation types and the dispatch table are declared in
 * @ref stream_adapters_vtbl.h.
 */

#ifndef LEXLEO_STREAM_ADAPTERS_TYPES_H
#define LEXLEO_STREAM_ADAPTERS_TYPES_H

#include "stream_adapters_vtbl.h"
#include "stream/borrowers/stream_borrowers_types.h"
#include "stream/owners/stream_owners_types.h"
#include "stream/adapters/stream_adapters_vtbl.h"

#include "osal/mem/osal_mem_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct stream_env_t
 *
 * @brief Borrowed dependencies required by the `stream` port.
 *
 * @details
 * The Composition Root provides these dependencies when creating a `stream`
 * handle. Pointer fields remain borrowed and must outlive the stream handles
 * using this environment.
 */
typedef struct stream_env_t {

	/** Borrowed memory operations. */
	const osal_mem_ops_t *mem;

	/** Borrowed backend virtual table. */
	const stream_vtbl_t *vtbl;

} stream_env_t;

/**
 * @brief Destructor for descriptor-owned user data.
 *
 * @param[in] ud Opaque user data to destroy.
 * @param[in] mem Memory operations used for destruction.
 */
typedef void (*ud_dtor_fn_t)(const void *ud, const osal_mem_ops_t *mem);

/**
 * @brief Adapter constructor used by stream factory services.
 *
 * @param[in] ud Opaque constructor user data.
 * @param[in] args Adapter-specific creation arguments.
 * @param[out] out Receives the created stream handle.
 *
 * @return Construction status.
 */
typedef stream_status_t (*stream_ctor_fn_t)(
	const void *ud,
	const void *args,
	stream_t **out);

/**
 * @brief Descriptor used to register a stream adapter.
 */
typedef struct stream_adapter_desc_t {

	/** Adapter registration key. */
	stream_key_t key;

	/** Adapter constructor. */
	stream_ctor_fn_t ctor;

	/** Optional constructor user data. */
	const void *ud;

	/** Optional destructor for `ud`. */
	ud_dtor_fn_t ud_dtor;

} stream_adapter_desc_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_ADAPTERS_TYPES_H */
