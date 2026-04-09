/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream.h
 * @ingroup stream_borrowers_api
 * @brief Borrower-facing runtime operations for the `stream` port.
 *
 * @details
 * This header exposes the public runtime operations used to interact with a
 * borrowed `stream_t` handle.
 */

#ifndef LEXLEO_STREAM_H
#define LEXLEO_STREAM_H

#include "stream/borrowers/stream_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read bytes from a stream.
 *
 * @param[in] s
 * Stream handle to read from.
 *
 * @param[out] buf
 * Destination buffer receiving up to `n` bytes.
 *
 * @param[in] n
 * Maximum number of bytes to read.
 *
 * @param[out] st
 * Optional status output.
 *
 * @return
 * Number of bytes actually read.
 *
 * @details
 * This function exposes the borrower-facing read operation of the `stream`
 * port.
 *
 * If `n == 0`, the function returns `0` and, when `st != NULL`, reports
 * `STREAM_STATUS_OK`.
 */
size_t stream_read(stream_t *s, void *buf, size_t n, stream_status_t *st);

/**
 * @brief Write bytes to a stream.
 *
 * @param[in] s
 * Stream handle to write to.
 *
 * @param[in] buf
 * Source buffer containing up to `n` bytes to write.
 *
 * @param[in] n
 * Maximum number of bytes to write.
 *
 * @param[out] st
 * Optional status output.
 *
 * @return
 * Number of bytes actually written.
 *
 * @details
 * This function exposes the borrower-facing write operation of the `stream`
 * port.
 *
 * If `n == 0`, the function returns `0` and, when `st != NULL`, reports
 * `STREAM_STATUS_OK`.
 */
size_t stream_write(stream_t *s, const void *buf, size_t n, stream_status_t *st);

/**
 * @brief Flush a stream.
 *
 * @param[in] s
 * Stream handle to flush.
 *
 * @return
 * Operation status.
 *
 * @details
 * This function exposes the borrower-facing flush operation of the `stream`
 * port.
 */
stream_status_t stream_flush(stream_t *s);

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_STREAM_H