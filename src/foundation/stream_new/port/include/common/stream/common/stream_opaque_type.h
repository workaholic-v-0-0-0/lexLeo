/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_opaque_type.h
 * @ingroup stream_common_api
 * @brief Opaque stream handle type.
 *
 * @details
 * Declares the opaque stream handle shared across the public stream APIs.
 */

#ifndef LEXLEO_STREAM_OPAQUE_TYPE_H
#define LEXLEO_STREAM_OPAQUE_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opaque handle to a stream instance.
 */
typedef struct stream_t stream_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_OPAQUE_TYPE_H */
