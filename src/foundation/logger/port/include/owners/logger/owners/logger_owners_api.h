/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_owners_api.h
 * @ingroup logger_owners_api
 * @brief Owner-facing lifecycle operations for the `logger` port.
 *
 * @details
 * This header exposes the public lifecycle operations available to modules
 * that own a @ref logger_t instance.
 */

#ifndef LEXLEO_LOGGER_OWNERS_API_H
#define LEXLEO_LOGGER_OWNERS_API_H

#include "logger/borrowers/logger_borrowers_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Destroy a `logger` handle.
 *
 * @param[in,out] s Address of the logger handle to destroy.
 *
 * See contract:
 * - @ref specifications_logger_destroy
 */
logger_status_t logger_destroy(logger_t **l);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_OWNERS_API_H */
