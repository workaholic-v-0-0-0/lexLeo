/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_owners_api.h
 * @ingroup stream_owners_api
 * @brief Owner-facing API for the `stream` port.
 *
 * @details
 * This header aggregates the owner-facing `stream` API: borrower operations,
 * owner-visible types, creator contracts, factory creation, and destruction.
 */

#ifndef LEXLEO_STREAM_OWNERS_API_H
#define LEXLEO_STREAM_OWNERS_API_H

#include "stream/borrowers/stream_borrowers_api.h"
#include "stream/owners/stream_owners_types.h"
#include "stream/owners/stream_creators_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Destroy a `stream` handle.
 *
 * @param[in,out] s Address of the stream handle to destroy.
 *
 * See contract:
 * - @ref specifications_stream_destroy
 */
stream_status_t stream_destroy(stream_t **s);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_OWNERS_API_H */
