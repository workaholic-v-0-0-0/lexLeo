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
 * @defgroup logger logger port
 * @brief Structured logging interface (port).
 *
 * This module defines the stable logging API used by the rest of the
 * codebase. Concrete logging backends (file, stderr, stream, etc.) are
 * provided by adapters and injected through the wiring context.
 *
 * The public API is intentionally small: create/destroy are wiring-only
 * (see internal headers), while consumers use @ref logger_log to emit
 * messages through an injected @ref logger_t instance.
 */

/** @{ */

logger_status_t logger_log(logger_t *l, const char *message);

/** @} */

#endif //LEXLEO_LOGGER_H
