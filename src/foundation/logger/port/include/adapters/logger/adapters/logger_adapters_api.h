/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_adapters_api.h
 * @ingroup logger_adapters_api
 * @brief Adapter-facing API for the `logger` port.
 *
 * @details
 * This header exposes the adapter-facing contract used to bind backend
 * implementations to the generic `logger` port.
 *
 * It defines the adapter-side constructor helper @ref logger_create().
 *
 * Backend operation types, dispatch tables and environment types are declared
 * by @ref logger_adapters_types.h.
 *
 * Typical usage:
 * - an adapter defines backend operations matching this contract,
 * - fills a @ref logger_vtbl_t,
 * - initializes its backend-specific state,
 * - calls logger_create(), then logger_complete_default_init() to build the
 *   public logger handle.
 */

#ifndef LEXLEO_LOGGER_ADAPTERS_API_H
#define LEXLEO_LOGGER_ADAPTERS_API_H

#include "logger/adapters/logger_adapters_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Build a default `logger_env_t`.
 *
 * @param[in] vtbl Virtual table.
 * @param[in] mem_ops Memory operations.
 *
 * @return
 * A default `logger_env_t`.
 *
 * See contract:
 * - @ref specifications_logger_default_env
 */
logger_env_t logger_default_env(
	const logger_vtbl_t *vtbl,
	const osal_mem_ops_t *mem_ops);

/**
 * @brief Creates a logger handle.
 *
 * @details
 * Allocates and initializes a logger handle from the provided borrowed
 * environment.
 *
 * Only the borrowed handle fields are initialized by this function. Owned
 * runtime resources are initialized later by
 * `logger_complete_default_init()`.
 *
 * @param[out] out Receives the created logger handle.
 * @param[in] env Logger borrowed environment.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_logger_create
 */
logger_status_t logger_create(
	logger_t **out,
	const logger_env_t *env);

/**
 * @brief Completes the default initialization of a logger handle.
 *
 * @details
 * Transfers ownership of the backend to the logger handle. The backend is
 * created by the adapter before calling this function and its ownership is
 * transferred to the logger handle on success.
 *
 * @param[in,out] logger Logger handle to complete.
 * @param[in] backend Owned backend to attach to the logger handle.
 *
 * @return Initialization status.
 *
 * See contract:
 * - @ref specifications_logger_complete_default_init
 */
logger_status_t logger_complete_default_init(
	logger_t *logger,
	void *backend);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_ADAPTERS_API_H */
