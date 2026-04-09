/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_adapters_api.h
 * @ingroup logger_adapters_api
 * @brief Adapter-side API for constructing and binding `logger_t` objects.
 *
 * @details
 * This header exposes the adapter-facing contract used to bind a backend
 * implementation to the generic `logger_t` port.
 *
 * It defines:
 * - backend operation function-pointer types through `logger_vtbl_t`,
 * - the adapter-side constructor helper `logger_create()`.
 *
 * Typical usage:
 * - an adapter defines backend operations matching this contract,
 * - fills a `logger_vtbl_t`,
 * - allocates or prepares its backend state,
 * - calls `logger_create()` to build the public logger handle.
 */

#ifndef LEXLEO_LOGGER_ADAPTERS_API_H
#define LEXLEO_LOGGER_ADAPTERS_API_H

#include "logger/borrowers/logger_types.h"
#include "logger/adapters/logger_env.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct logger_vtbl_t
 * @brief Adapter dispatch table bound to a `logger_t` instance.
 *
 * @details
 * This table defines the concrete backend operations used by the generic
 * logger port for message emission and destruction.
 *
 * A valid vtable must provide all operations.
 */
typedef struct logger_vtbl_t {
	/** Backend log operation. Must not be NULL. */
	logger_status_t (*log)(void *backend, const char *message);

	/** Backend destroy operation. Must not be NULL. */
	void (*destroy)(void *backend);
} logger_vtbl_t;

/**
 * @brief Create a generic logger handle from adapter-provided backend bindings.
 *
 * @details
 * This function is intended to be called by logger adapters to construct a
 * public `logger_t` from:
 * - a dispatch table describing backend operations,
 * - an adapter-owned backend instance,
 * - an injected environment carrying runtime dependencies.
 *
 * The produced logger handle takes ownership of neither the vtable nor the
 * environment object itself; it only stores the required runtime references.
 * The backend lifetime and destruction semantics are governed by the adapter
 * contract, notably through `vtbl->destroy`.
 *
 * @param[out] out
 * Receives the created logger handle.
 * Must not be NULL.
 *
 * @param[in] vtbl
 * Backend dispatch table.
 * Must not be NULL and must provide non-NULL operations.
 *
 * @param[in] backend
 * Adapter-owned backend instance to bind to the logger.
 * May be NULL only if the resulting semantics are explicitly supported by the
 * caller, though normal adapters are expected to provide a valid backend.
 *
 * @param[in] env
 * Logger environment carrying injected runtime dependencies.
 * Must not be NULL.
 *
 * @return
 * - `LOGGER_STATUS_OK` on success
 * - `LOGGER_STATUS_INVALID` if arguments are invalid
 * - `LOGGER_STATUS_OOM` on allocation failure
 *
 * @post
 * On success, `*out` contains a valid logger handle that must later be
 * destroyed via `logger_destroy()`.
 */
logger_status_t logger_create(
	logger_t **out,
	const logger_vtbl_t *vtbl,
	void *backend,
	const logger_env_t *env );

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_LOGGER_ADAPTERS_API_H
