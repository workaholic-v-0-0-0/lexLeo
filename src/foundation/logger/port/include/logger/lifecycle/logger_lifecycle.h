/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_lifecycle.h
 * @ingroup logger_lifecycle_api
 * @brief Lifecycle services for `logger_t` handles.
 *
 * @details
 * This header exposes the public destruction service for runtime
 * `logger_t` objects.
 */

#ifndef LEXLEO_LOGGER_LIFECYCLE_H
#define LEXLEO_LOGGER_LIFECYCLE_H

/**
 * @brief Destroy a logger handle.
 *
 * @param[in,out] l
 * Address of the logger handle to destroy.
 *
 * @details
 * If `l == NULL` or `*l == NULL`, this function does nothing.
 * Otherwise, it releases the logger object and sets `*l` to `NULL`.
 */
void logger_destroy(logger_t **l);

#endif //LEXLEO_LOGGER_LIFECYCLE_H
