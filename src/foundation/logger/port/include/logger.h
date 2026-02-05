/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/logger/port/include/
 * logger.h
 */

#ifndef LEXLEO_LOGGER_H
#define LEXLEO_LOGGER_H

#include "logger_types.h"

/**
 * @addtogroup logger_api
 * @{
 */

/**
 * Emit a log message through an injected logger instance.
 */
logger_status_t logger_log(logger_t *l, const char *message);

/** @} */

#endif //LEXLEO_LOGGER_H
