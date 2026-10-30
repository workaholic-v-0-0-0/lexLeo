/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_opaque_type.h
 * @ingroup logger_common_api
 * @brief Opaque logger handle type.
 *
 * @details
 * Declares the opaque logger handle shared across the public logger APIs.
 */

#ifndef LEXLEO_LOGGER_OPAQUE_TYPE_H
#define LEXLEO_LOGGER_OPAQUE_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque handle to a logger instance.
 */
typedef struct logger_t logger_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LOGGER_OPAQUE_TYPE_H */
