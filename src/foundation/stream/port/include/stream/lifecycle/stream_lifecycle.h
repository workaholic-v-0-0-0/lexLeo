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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Destroy a stream handle.
 *
 * @param[in,out] s
 * Address of the stream handle to destroy.
 *
 * @return
 * `STREAM_STATUS_OK` if there is no stream to destroy or if destruction
 * succeeds. Otherwise, returns the status reported by the backend close
 * callback.
 *
 * @details
 * If `s == NULL` or `*s == NULL`, this function returns `STREAM_STATUS_OK`.
 * Otherwise, it first delegates backend cleanup to the stream close callback.
 *
 * If backend cleanup succeeds, it releases the stream object and sets `*s` to
 * `NULL`.
 *
 * If backend cleanup fails, it returns the reported status and leaves `*s`
 * unchanged.
 */
stream_status_t stream_destroy(stream_t **s);

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_STREAM_LIFECYCLE_H