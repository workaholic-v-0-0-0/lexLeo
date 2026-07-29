/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_vtbl_type.h
 * @ingroup logger_common_api
 * @brief Backend dispatch-table types for the `logger` port.
 *
 * @details
 * Declares the backend operation callback types and dispatch table shared
 * between the `logger` port implementation and its adapters.
 */

#ifndef LEXLEO_LOGGER_VTBL_TYPE_H
#define LEXLEO_LOGGER_VTBL_TYPE_H

#include "logger/common/logger_status_type.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 *
 * @return Operation status.
 */
typedef logger_status_t (*logger_destroy_fn_t)(void *backend);

/**
 * @struct logger_vtbl_t
 * @brief Adapter dispatch table for the `logger` port.
 *
 * @details
 * Groups the backend operations implemented by a `logger` adapter.
 */
typedef struct logger_vtbl_t {

	/** Backend log operation. Must not be `NULL`. */
	logger_log_fn_t log;

	/** Backend destroy operation. Must not be `NULL`. */
	logger_destroy_fn_t destroy;

} logger_vtbl_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_VTBL_TYPE_H */
