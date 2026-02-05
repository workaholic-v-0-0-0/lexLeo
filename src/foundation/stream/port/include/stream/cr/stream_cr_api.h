/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_cr_api.h
 * @ingroup stream_cr_api
 * @brief Composition Root helpers for the `stream` port.
 *
 * @details
 * This header exposes small CR-facing helpers used to build default public
 * `stream` port wiring objects.
 *
 * @note This API belongs to the Composition Root surface.
 * Runtime modules should not depend on it directly.
 */

#ifndef LEXLEO_STREAM_CR_API_API_H
#define LEXLEO_STREAM_CR_API_API_H

#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_env.h"

/**
 * @brief Return the default borrower-facing ops table for the `stream` port.
 *
 * @details
 * The returned table exposes the default public borrower operations of the
 * `stream` port.
 *
 * @return
 * Non-`NULL` pointer to a well-formed `stream_ops_t`.
 *
 * @post
 * - `ret->read != NULL`
 * - `ret->write != NULL`
 * - `ret->flush != NULL`
 */
const stream_ops_t *stream_default_ops(void);

/**
 * @brief Build a default `stream_env_t` from injected memory operations.
 *
 * @param[in] mem_ops
 * Memory operations to expose through the returned environment.
 *
 * @return
 * A `stream_env_t` such that `ret.mem == mem_ops`.
 *
 * @note
 * This helper does not allocate and performs no validation.
 * It only packages the provided dependency into a public `stream_env_t`.
 */
stream_env_t stream_default_env(const osal_mem_ops_t *mem_ops);

#endif //LEXLEO_STREAM_CR_API_API_H
