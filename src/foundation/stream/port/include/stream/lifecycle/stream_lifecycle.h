/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_lifecycle.h
 * @ingroup stream_lifecycle_api
 * @brief Lifecycle services for `stream_t` handles.
 *
 * @details
 * This header exposes the public destruction service for runtime
 * `stream_t` objects.
 */

#ifndef LEXLEO_STREAM_LIFECYCLE_H
#define LEXLEO_STREAM_LIFECYCLE_H

#include "stream/borrowers/stream_types.h"

/**
 * @brief Destroy a stream handle.
 *
 * @param[in,out] s
 * Address of the stream handle to destroy.
 *
 * @details
 * If `s == NULL` or `*s == NULL`, this function does nothing.
 * Otherwise, it releases the stream object and sets `*s` to `NULL`.
 */
void stream_destroy(stream_t **s);

#endif // LEXLEO_STREAM_LIFECYCLE_H