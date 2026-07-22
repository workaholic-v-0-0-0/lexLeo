/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_adapters_vtbl.h
 * @ingroup logger_adapters_api
 * @brief Adapter dispatch-table types for the `logger` port.
 *
 * @details
 * This header defines the backend operation callback types and the
 * @ref logger_vtbl_t dispatch table used to bind adapter-specific behavior
 * to a public `logger_t` handle.
 */

#ifndef LEXLEO_LOGGER_ADAPTERS_VTBL_H
#define LEXLEO_LOGGER_ADAPTERS_VTBL_H

#include "logger/borrowers/logger_borrowers_types.h"

/**
 * @brief Backend log operation for a logger adapter.
 *
 * @param[in] backend Adapter-owned backend instance bound to the logger.
 * @param[in] message Null-terminated message to emit.
 *
 * @return Operation status.
 */
typedef logger_status_t (*logger_log_fn_t)(
	void *backend,
	const char *message);

/**
 * @brief Backend destroy operation for a logger adapter.
 *
 * @param[in] backend Adapter-owned backend instance bound to the logger.
 *
 * @details
 * Invoked by @ref logger_destroy() when releasing the logger handle.
 */
typedef logger_status_t (*logger_destroy_fn_t)(void *backend);

/**
 * @struct logger_vtbl_t
 * @brief Adapter dispatch table bound to a @ref logger_t instance.
 *
 * @details
 * Groups the backend operations used by the `logger` port.
 */
typedef struct logger_vtbl_t {

	/** Backend log operation. Must not be `NULL`. */
	logger_log_fn_t log;

	/** Backend destroy operation. Must not be `NULL`. */
	logger_destroy_fn_t destroy;

} logger_vtbl_t;

#endif /* LEXLEO_LOGGER_ADAPTERS_VTBL_H */
