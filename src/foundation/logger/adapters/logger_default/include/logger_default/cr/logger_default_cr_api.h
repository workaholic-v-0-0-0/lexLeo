/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_default_cr_api.h
 * @ingroup logger_default_cr_api
 * @brief Composition Root API for wiring the `logger_default` adapter.
 *
 * @details
 * This header exposes the CR-facing entry points used to:
 * - build default `logger_default` configuration and environment values,
 * - create a logger instance backed by the `logger_default` adapter.
 */

#ifndef LEXLEO_LOGGER_DEFAULT_CR_API_H
#define LEXLEO_LOGGER_DEFAULT_CR_API_H

#include "logger_default/cr/logger_default_types.h"
#include "logger/borrowers/logger_types.h"

/**
 * @brief Return a default configuration for the `logger_default` adapter.
 *
 * @details
 * This helper provides a value-initialized baseline configuration that the
 * Composition Root may override.
 *
 * @return
 * A well-formed `logger_default_cfg_t`.
 */
logger_default_cfg_t logger_default_default_cfg(void);

/**
 * @brief Build a default environment for the `logger_default` adapter.
 *
 * @details
 * This helper packages borrowed dependencies into a value-initialized
 * `logger_default_env_t`.
 *
 * The returned environment does not take ownership of any pointer passed as
 * argument.
 *
 * @param[in] stream
 * Borrowed target stream used by the adapter.
 *
 * @param[in] time_ops
 * Borrowed time operations used for timestamp generation.
 *
 * @param[in] adapter_mem
 * Borrowed memory operations used for adapter-backend allocation.
 *
 * @param[in] port_env
 * Borrowed `logger` port environment.
 *
 * @return
 * A well-formed `logger_default_env_t` aggregating the provided dependencies.
 */
logger_default_env_t logger_default_default_env(
	stream_t *stream,
	const osal_time_ops_t *time_ops,
	const osal_mem_ops_t *adapter_mem,
	const logger_env_t *port_env);

/**
 * @brief Create a logger instance backed by the `logger_default` adapter.
 *
 * @details
 * This function allocates and initializes a public `logger_t` whose backend
 * writes emitted messages to the injected target stream.
 *
 * Ownership:
 * - On success, a newly allocated logger is returned in `*out`.
 * - The caller becomes responsible for destroying it via `logger_destroy()`.
 * - On failure, `*out` is left unchanged.
 *
 * @param[out] out
 * Receives the created logger handle.
 * Must not be `NULL`.
 *
 * @param[in] cfg
 * Adapter configuration.
 * Must not be `NULL`.
 *
 * @param[in] env
 * Adapter environment containing injected dependencies.
 * Must not be `NULL`.
 *
 * @retval LOGGER_STATUS_OK
 * Logger successfully created.
 *
 * @retval LOGGER_STATUS_INVALID
 * One or more arguments are invalid.
 *
 * @retval LOGGER_STATUS_OOM
 * Memory allocation failed.
 */
logger_status_t logger_default_create_logger(
	logger_t **out,
	const logger_default_cfg_t *cfg,
	const logger_default_env_t *env);

#endif // LEXLEO_LOGGER_DEFAULT_CR_API_H
