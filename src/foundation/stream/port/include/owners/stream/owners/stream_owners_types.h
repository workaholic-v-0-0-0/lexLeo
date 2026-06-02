/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_owners_types.h
 * @ingroup stream_owners_api
 * @brief Owner-visible types for the `stream` port.
 *
 * @details
 * This header defines the opaque factory handle and adapter key type used by
 * the owner-facing `stream` API.
 */

#ifndef LEXLEO_STREAM_OWNERS_TYPES_H
#define LEXLEO_STREAM_OWNERS_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque stream factory type.
 *
 * @details
 * A `stream_factory_t` provides adapter registration and stream creation
 * services.
 */
typedef struct stream_factory_t stream_factory_t;

/**
 * @brief Identifier for a registered stream adapter.
 */
typedef const char *stream_key_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_OWNERS_TYPES_H */
