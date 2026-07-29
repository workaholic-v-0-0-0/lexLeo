/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_cr_api.h
 * @ingroup logger_cr_api
 * @brief Composition Root operations for the `logger` port.
 *
 * @details
 * This header exposes the operations used by the Composition Root to create,
 * complete the initialization of, and destroy a @ref logger_t handle.
 */

#ifndef LEXLEO_LOGGER_CR_API_H
#define LEXLEO_LOGGER_CR_API_H

#include "logger/common/logger_opaque_type.h"
#include "logger/common/logger_status_type.h"
#include "logger/common/logger_vtbl_type.h"
#include "logger/cr/logger_cr_types.h"

#include "osal/mem/osal_mem_types.h"

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

/**
 * @brief Destroy a `logger` handle.
 *
 * @param[in,out] logger Address of the logger handle to destroy.
 *
 * See contract:
 * - @ref specifications_logger_destroy
 */
logger_status_t logger_destroy(logger_t **logger);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_CR_API_H */
