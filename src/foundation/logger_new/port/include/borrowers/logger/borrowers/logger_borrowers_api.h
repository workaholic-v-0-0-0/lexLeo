/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_borrowers_api.h
 * @ingroup logger_borrowers_api
 * @brief Borrower-facing API for the `logger` port.
 *
 * @details
 * Declares the runtime operations available to modules that borrow a `logger`
 * instance through its opaque handle.
 */

#ifndef LEXLEO_LOGGER_BORROWERS_API_H
#define LEXLEO_LOGGER_BORROWERS_API_H

#include "logger/common/logger_opaque_type.h"
#include "logger/common/logger_status_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Emit a log message through a logger.
 *
 * @param[in] logger Logger handle used to emit the message.
 * @param[in] message Null-terminated message to log.
 *
 * @return Operation status.
 *
 * See contract:
 * - @ref specifications_logger_log
 */
logger_status_t logger_log(logger_t *logger, const char *message);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_BORROWERS_API_H */
