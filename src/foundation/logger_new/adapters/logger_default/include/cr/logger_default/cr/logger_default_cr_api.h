/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_default_cr_api.h
 * @ingroup logger_default_cr_api
 * @brief Composition Root API for the `logger_default` adapter.
 *
 * @details
 * Exposes the CR-facing services used to configure and construct
 * `logger_default` backends from their external borrowed dependencies and
 * complete their initialization.
 */

#ifndef LEXLEO_LOGGER_DEFAULT_CR_API_H
#define LEXLEO_LOGGER_DEFAULT_CR_API_H

#include "logger/adapters/logger_adapters_types.h"

#include "logger_default/cr/logger_default_cr_types.h"

#include "stream/borrowers/stream_borrowers_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return the `logger` port dispatch table implemented by the
 * `logger_default` adapter.
 *
 * @return
 * Borrowed pointer to the `logger_default` adapter `logger_vtbl_t`.
 *
 * See contract:
 * - @ref specifications_logger_default_vtbl
 * "logger_default_vtbl() specifications".
 */
const logger_vtbl_t *logger_default_vtbl(void);

/**
 * @brief Return a default configuration for the `logger_default`
 * adapter.
 *
 * @details
 * This helper builds the default configuration used to complete the
 * initialization of a `logger_default` backend.
 *
 * @return A well-formed default `logger_default_cfg_t`.
 *
 * See contract:
 * - @ref specifications_logger_default_default_cfg
 */
logger_default_cfg_t logger_default_default_cfg(void);

/**
 * @brief Return a default `logger_default` environment.
 *
 * @param[in] stream Borrowed target stream used by the adapter.
 * @param[in] time_ops Borrowed time operations table.
 * @param[in] mem_ops Borrowed memory operations table.
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
	const osal_mem_ops_t *mem_ops);

/**
 * @brief Create a `logger_default` backend.
 *
 * @details
 * Initializes the backend's external borrowed dependencies from `env`.
 * Remaining initialization is completed separately through
 * `logger_default_complete_default_init()`.
 *
 * @param[out] out Receives the created backend.
 * @param[in] env External borrowed dependencies required to construct the backend.
 *
 * @return Creation status.
 *
 * See contract:
 * - @ref specifications_logger_default_create
 */
logger_default_status_t logger_default_create(
	logger_default_t **out,
	const logger_default_env_t *env);

/**
 * @brief Complete the default initialization of a
 * `logger_default` backend.
 *
 * @param[in,out] logger_default Backend to complete.
 * @param[in] cfg Backend configuration.
 *
 * @return Initialization status.
 *
 * See contract:
 * - @ref specifications_logger_default_complete_default_init
 */
logger_default_status_t logger_default_complete_default_init(
	logger_default_t *logger_default,
	const logger_default_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_DEFAULT_CR_API_H */
