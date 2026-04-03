/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stdio_stream_ctor_ud.h
 * @ingroup stdio_stream_internal_group
 * @brief Private constructor user-data stored in `stdio_stream`
 * factory descriptors.
 *
 * @details
 * This header exposes the private user-data object allocated by
 * `stdio_stream_create_desc()` and later consumed by
 * `stdio_stream_ctor()`.
 */

#ifndef LEXLEO_STDIO_STREAM_CTOR_UD_H
#define LEXLEO_STDIO_STREAM_CTOR_UD_H

#include "stdio_stream/cr/stdio_stream_types.h"

/**
 * @brief Private constructor user-data for `stdio_stream` factory
 * registration.
 *
 * @details
 * This structure stores the configuration and environment snapshots bound into
 * the adapter descriptor and later reused by `stdio_stream_ctor()`.
 */
typedef struct stdio_stream_ctor_ud_t {
	/** Bound adapter configuration. */
	stdio_stream_cfg_t cfg;

	/** Bound adapter environment. */
	stdio_stream_env_t env;
} stdio_stream_ctor_ud_t;

#endif // LEXLEO_STDIO_STREAM_CTOR_UD_H
