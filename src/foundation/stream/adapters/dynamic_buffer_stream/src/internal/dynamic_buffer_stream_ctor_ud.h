/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream_ctor_ud.h
 * @ingroup dynamic_buffer_stream_internal_group
 * @brief Private constructor user-data stored in `dynamic_buffer_stream`
 * factory descriptors.
 *
 * @details
 * This header exposes the private user-data object allocated by
 * `dynamic_buffer_stream_create_desc()` and later consumed by
 * `dynamic_buffer_stream_ctor()`.
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_CTOR_UD_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_CTOR_UD_H

#include "dynamic_buffer_stream/cr/dynamic_buffer_stream_types.h"

/**
 * @brief Private constructor user-data for `dynamic_buffer_stream` factory
 * registration.
 *
 * @details
 * This structure stores the configuration and environment snapshots bound into
 * the adapter descriptor and later reused by `dynamic_buffer_stream_ctor()`.
 */
typedef struct dynamic_buffer_stream_ctor_ud_t {
	/** Bound adapter configuration. */
	dynamic_buffer_stream_cfg_t cfg;

	/** Bound adapter environment. */
	dynamic_buffer_stream_env_t env;
} dynamic_buffer_stream_ctor_ud_t;

#endif // LEXLEO_DYNAMIC_BUFFER_STREAM_CTOR_UD_H
