/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file logger_env.h
 * @ingroup logger_adapters_api
 * @brief Public environment type for the `logger` port.
 *
 * @details
 * This header exposes the public environment object shared by logger-related
 * services that require injected runtime dependencies.
 */

#ifndef LEXLEO_LOGGER_ENV_H
#define LEXLEO_LOGGER_ENV_H

#include "osal/mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Runtime environment for the `logger` port.
 *
 * @details
 * This structure carries injected runtime dependencies required by the public
 * `logger` port implementation.
 *
 * It is typically prepared by the Composition Root and then passed to
 * adapter-side services that need access to shared runtime support.
 */
typedef struct logger_env_t {
	/**
	 * @brief Memory operations used by the `logger` port.
	 *
	 * @details
	 * This dependency provides the allocation services used to create and
	 * destroy public `logger_t`-related runtime objects.
	 */
	const osal_mem_ops_t *mem;
} logger_env_t;

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_LOGGER_ENV_H
