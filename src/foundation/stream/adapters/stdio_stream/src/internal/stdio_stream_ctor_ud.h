/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stdio_stream_ctor_ud.h
 * @ingroup stdio_stream_internal_group
 * @brief Private constructor user data stored in `stdio_stream`
 * factory descriptors.
 *
 * @details
 * Defines the private data used to bind a `stdio_stream` adapter
 * configuration and its external borrowed dependencies to a factory
 * constructor.
 */

#ifndef LEXLEO_STDIO_STREAM_CTOR_UD_H
#define LEXLEO_STDIO_STREAM_CTOR_UD_H

#include "stdio_stream/cr/stdio_stream_cr_types.h"

/**
 * @brief Private constructor user data for `stdio_stream` registration.
 *
 * @details
 * Captures the adapter configuration and environment required by the factory
 * constructor. The environment provides the external borrowed dependencies
 * used to create and complete the initialization of `stdio_stream` backends.
 */
typedef struct stdio_stream_ctor_ud_t {

	/** Bound adapter configuration. */
	stdio_stream_cfg_t cfg;

	/** Bound adapter environment. */
	stdio_stream_env_t env;

} stdio_stream_ctor_ud_t;

#endif /* LEXLEO_STDIO_STREAM_CTOR_UD_H */
