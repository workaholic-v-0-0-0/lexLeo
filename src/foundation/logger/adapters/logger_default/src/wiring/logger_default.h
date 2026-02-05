/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/logger/adapters/logger_default/src/wiring/
 * logger_default.h
 */

#ifndef LEXLEO_LOGGER_DEFAULT_H
#define LEXLEO_LOGGER_DEFAULT_H

#include "logger_default_ctx.h"
#include "logger_types.h"

typedef struct logger_t logger_t;

logger_status_t logger_default_create_logger(
    logger_t **out,
    const logger_default_ctx_t *ctx );

/* Destroys a logger created by this adapter. */
void logger_default_destroy_logger(logger_t **l);

#endif //LEXLEO_LOGGER_DEFAULT_H
