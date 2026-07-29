/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file dynamic_buffer_stream_ctor_ud.h
 * @ingroup dynamic_buffer_stream_internal_group
 * @brief Private constructor user data stored by a `dynamic_buffer_stream`
 * adapter provider.
 *
 * @details
 * Defines the private data used to bind a `dynamic_buffer_stream` adapter
 * configuration and its external borrowed dependencies to a backend
 * constructor.
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_CTOR_UD_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_CTOR_UD_H

#include "dynamic_buffer_stream/cr/dynamic_buffer_stream_cr_types.h"

/**
 * @brief Private constructor user data for `dynamic_buffer_stream`
 * adapter providers.
 *
 * @details
 * Captures the adapter configuration and environment required by the backend
 * constructor. The environment provides the external borrowed dependencies
 * used to create and complete the initialization of `dynamic_buffer_stream`
 * backends.
 */
typedef struct dynamic_buffer_stream_ctor_ud_t {

	/** Bound adapter configuration. */
	dynamic_buffer_stream_cfg_t cfg;


	/** Bound adapter environment. */
	dynamic_buffer_stream_env_t env;

} dynamic_buffer_stream_ctor_ud_t;

#endif /* LEXLEO_DYNAMIC_BUFFER_STREAM_CTOR_UD_H */
