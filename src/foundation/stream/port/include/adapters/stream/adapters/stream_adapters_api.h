/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_adapters_api.h
 * @ingroup stream_adapters_api
 * @brief Adapter-facing API for the `stream` port.
 *
 * @details
 * This header exposes the adapter-facing contract used to bind backend
 * implementations to the generic `stream` port.
 *
 * It defines the adapter-side constructor helper @ref stream_create().
 *
 * Backend operation types, dispatch tables and environment types are declared
 * by @ref stream_adapters_types.h.
 *
 * Typical usage:
 * - an adapter defines backend operations matching this contract,
 * - fills a @ref stream_vtbl_t,
 * - initializes its backend-specific state,
 * - calls stream_create(), then stream_complete_default_init() to build the
 *   public stream handle.
 */

#ifndef LEXLEO_STREAM_ADAPTERS_API_H
#define LEXLEO_STREAM_ADAPTERS_API_H

#include "stream/owners/stream_owners_api.h"
#include "stream/adapters/stream_adapters_types.h"
#include "stream/borrowers/stream_borrowers_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Build a default `stream_env_t`.
 *
 * @param[in] vtbl Virtual table.
 * @param[in] mem_ops Memory operations.
 *
 * @return
 * A default `stream_env_t`.
 *
 * See contract:
 * - @ref specifications_stream_default_env
 */
stream_env_t stream_default_env(
	const stream_vtbl_t *vtbl,
	const osal_mem_ops_t *mem_ops);

/**
 * @brief Creates a stream handle.
 *
 * @details
 * Allocates and initializes a stream handle from the provided borrowed
 * environment.
 *
 * Only the borrowed handle fields are initialized by this function. Owned
 * runtime resources are initialized later by
 * `stream_complete_default_init()`.
 *
 * @param[out] out Receives the created stream handle.
 * @param[in] env Stream borrowed environment.
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
 * @brief Completes the default initialization of a stream handle.
 *
 * @details
 * Transfers ownership of the backend to the stream handle. The backend is
 * created by the adapter before calling this function and its ownership is
 * transferred to the stream handle on success.
 *
 * @param[in,out] stream Stream handle to complete.
 * @param[in] backend Owned backend to attach to the stream handle.
 *
 * @return Initialization status.
 *
 * See contract:
 * - @ref specifications_stream_complete_default_init
 */
stream_status_t stream_complete_default_init(
	stream_t *stream,
	void *backend
);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_ADAPTERS_API_H */
