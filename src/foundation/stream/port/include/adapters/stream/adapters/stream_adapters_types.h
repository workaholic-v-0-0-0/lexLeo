/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_adapters_types.h
 * @ingroup stream_adapters_api
 * @brief Adapter-facing types for the `stream` port.
 *
 * @details
 * Declares the callback and provider types used to construct stream adapters
 * and bind them to the `stream` port.
 */

#ifndef LEXLEO_STREAM_ADAPTERS_TYPES_H
#define LEXLEO_STREAM_ADAPTERS_TYPES_H

#include "stream/common/stream_status_type.h"
#include "stream/common/stream_vtbl_type.h"

#include "osal/mem/osal_mem_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Construct a stream adapter backend.
 *
 * @param[in] ud Adapter-specific constructor user data.
 * @param[in] args Adapter-specific construction arguments.
 * @param[out] out_backend Receives the created backend.
 *
 * @return Construction status.
 */
typedef stream_status_t (*stream_backend_ctor_fn_t)(
	const void *ud,
	const void *args,
	void **out_backend);

/**
 * @brief Destroy provider-owned user data.
 *
 * @param[in] ud Provider-owned user data to destroy.
 * @param[in] mem Memory operations used for destruction.
 */
typedef void (*stream_ctor_ud_dtor_fn_t)(
	void *ud,
	const osal_mem_ops_t *mem);

/**
 * @brief Adapter provider.
 *
 * @details
 * Groups the operations and context required to construct stream adapter.
 */
typedef struct stream_adapter_provider_t {

	/** Stream adapter backend constructor. */
	stream_backend_ctor_fn_t backend_ctor;

	/** Dispatch table associated with constructed backends. */
	const stream_vtbl_t *vtbl;

	/** Owned stream adapter backend constructor user data. */
	void *ud;

	/** Optional destructor for stream adapter backend constructor user
	 * data. */
	stream_ctor_ud_dtor_fn_t ud_dtor;

	/** Memory operations used to destroy the provider and by the stream
	 * adapter backend constructor. */
	const osal_mem_ops_t *mem;

} stream_adapter_provider_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_ADAPTERS_TYPES_H */
