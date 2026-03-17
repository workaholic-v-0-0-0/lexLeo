/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger.h
 * @ingroup logger_borrowers_api
 * @brief Borrower-facing runtime operations for the `logger` port.
 *
 * @details
 * This header exposes the public runtime operation used to emit messages
 * through a borrowed `logger_t` handle.
 */

#ifndef LEXLEO_LOGGER_H
#define LEXLEO_LOGGER_H

#include "logger_types.h"

/**
 * @brief Emit a log message through a logger.
 *
 * @param[in] l
 * Logger handle used to emit the message.
 *
 * @param[in] message
 * Null-terminated message to log.
 *
 * @return
 * Operation status.
 *
 * @details
 * This function exposes the borrower-facing logging operation of the
 * `logger` port.
 */
logger_status_t logger_log(logger_t *l, const char *message);

#endif //LEXLEO_LOGGER_H
