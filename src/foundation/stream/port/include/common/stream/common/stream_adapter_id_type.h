/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_adapter_id_type.h
 * @ingroup stream_common_api
 * @brief Stream adapter identifier type.
 *
 * @details
 * Declares the identifier type used to select a stream adapter provider
 * registered with a `stream_factory_t`.
 */

#ifndef LEXLEO_STREAM_ADAPTER_ID_TYPE_H
#define LEXLEO_STREAM_ADAPTER_ID_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Stream adapter identifier.
 *
 * @details
 * Identifies a stream adapter provider registered with a
 * `stream_factory_t`.
 */
typedef const char *stream_adapter_id_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_ADAPTER_ID_TYPE_H */
