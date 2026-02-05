/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_key_type.h
 * @ingroup stream_adapters_api
 * @brief Public key type used to identify `stream` adapters.
 *
 * @details
 * This header exposes the public key type used by adapter registration and
 * factory-based stream creation services.
 */

#ifndef LEXLEO_STREAM_KEY_TYPE_H
#define LEXLEO_STREAM_KEY_TYPE_H

/**
 * @brief Public identifier type for a registered stream adapter.
 *
 * @details
 * A `stream_key_t` names a concrete adapter within public registration and
 * lookup APIs.
 *
 * It is typically used:
 * - in `stream_adapter_desc_t` to declare an adapter identity,
 * - in factory-side APIs to resolve a previously registered adapter.
 */
typedef const char *stream_key_t;

#endif // LEXLEO_STREAM_KEY_TYPE_H
