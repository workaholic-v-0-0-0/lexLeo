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

#include "logger/borrowers/logger_borrowers_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return a default configuration for the `logger_default` adapter.
 *
 * @return
 * A well-formed `logger_default_cfg_t`.
 *
 * See contract:
 * - @ref specifications_logger_default_default_cfg "logger_default_default_cfg() specifications"
 */
logger_default_cfg_t logger_default_default_cfg(void);

/**
 * @brief Build a default environment for the `logger_default` adapter.
 *
 * @param[in] stream Borrowed target stream used by the adapter.
 * @param[in] time_ops Borrowed time operations table.
 * @param[in] adapter_mem_ops Borrowed memory operations table.
 * @param[in] port_mem_ops Borrowed memory operations used by the `logger`
 * port handle.
 *
 * @return A well-formed `logger_default_env_t` initialized from the provided
 * dependencies.
 *
 * See contract:
 * - @ref specifications_logger_default_default_env "logger_default_default_env() specifications"
 */
logger_default_env_t logger_default_default_env(
	stream_t *stream,
	const osal_time_ops_t *time_ops,
	const osal_mem_ops_t *adapter_mem_ops,
	const osal_mem_ops_t *port_mem_ops);

/**
 * @brief Create a logger instance backed by the `logger_default` adapter.
 *
 * @param[out] out
 * Receives the created logger handle.
 *
 * @param[in] cfg
 * Adapter configuration.
 *
 * @param[in] env
 * Adapter environment containing the injected dependencies.
 *
 * @retval LOGGER_STATUS_OK
 * Logger successfully created.
 *
 * @retval LOGGER_STATUS_INVALID
 * One or more arguments are invalid.
 *
 * @retval LOGGER_STATUS_OOM
 * Memory allocation failed.
 *
 * See contract:
 * - @ref specifications_logger_default_create_logger "logger_default_create_logger() specifications"
 */
logger_status_t logger_default_create_logger(
	logger_t **out,
	const logger_default_cfg_t *cfg,
	const logger_default_env_t *env);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_DEFAULT_CR_API_H */
