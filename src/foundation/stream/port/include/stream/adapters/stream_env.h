/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_env.h
 * @ingroup stream_adapters_api
 * @brief Public environment type for the `stream` port.
 *
 * @details
 * This header exposes the public environment object shared by stream-related
 * services that require injected runtime dependencies.
 */

#ifndef LEXLEO_STREAM_ENV_H
#define LEXLEO_STREAM_ENV_H

#include "osal/mem/osal_mem_ops.h"

/**
 * @brief Runtime environment for the `stream` port.
 *
 * @details
 * This structure carries injected runtime dependencies required by the public
 * `stream` port implementation.
 *
 * It is typically prepared by the Composition Root and then passed to
 * adapter-side or factory-side services that need access to shared runtime
 * support.
 */
typedef struct stream_env_t {
	/**
	 * @brief Memory operations used by the `stream` port.
	 *
	 * @details
	 * This dependency provides the allocation services used to create and
	 * destroy public `stream_t`-related runtime objects.
	 */
	const osal_mem_ops_t *mem;
} stream_env_t;

#endif // LEXLEO_STREAM_ENV_H
