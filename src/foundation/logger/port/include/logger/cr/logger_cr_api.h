/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_cr_api.h
 * @ingroup logger_cr_api
 * @brief Composition Root helpers for the `logger` port.
 *
 * @details
 * This header exposes small CR-facing helpers used to build default public
 * `logger` port wiring objects.
 *
 * @note This API belongs to the Composition Root surface.
 * Runtime modules should not depend on it directly.
 */

#ifndef LEXLEO_LOGGER_CR_API_H
#define LEXLEO_LOGGER_CR_API_H

#include "logger/adapters/logger_env.h"

/**
 * @brief Build a default `logger_env_t` from injected memory operations.
 *
 * @param[in] mem_ops
 * Memory operations to expose through the returned environment.
 *
 * @return
 * A `logger_env_t` such that `ret.mem == mem_ops`.
 *
 * @note
 * This helper does not allocate and performs no validation.
 * It only packages the provided dependency into a public `logger_env_t`.
 */
logger_env_t logger_default_env(const osal_mem_ops_t *mem_ops);

#endif //LEXLEO_LOGGER_CR_API_H
