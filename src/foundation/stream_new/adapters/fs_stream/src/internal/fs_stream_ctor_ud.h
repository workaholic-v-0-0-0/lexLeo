/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_ctor_ud.h
 * @ingroup fs_stream_internal_group
 * @brief Private constructor user data stored in `fs_stream` factory
 * descriptors.
 *
 * @details
 * Defines the private data used to bind an `fs_stream` adapter configuration
 * and its external borrowed dependencies to a factory constructor.
 */

#ifndef LEXLEO_FS_STREAM_CTOR_UD_H
#define LEXLEO_FS_STREAM_CTOR_UD_H

#include "fs_stream/cr/fs_stream_cr_types.h"

/**
 * @brief Private constructor user data for `fs_stream` registration.
 *
 * @details
 * Captures the adapter configuration and environment required by the factory
 * constructor. The environment provides the external borrowed dependencies
 * used to create and complete the initialization of `fs_stream` backends.
 */
typedef struct fs_stream_ctor_ud_t {

	/** Bound adapter configuration. */
	fs_stream_cfg_t cfg;

	/** Bound adapter environment. */
	fs_stream_env_t env;

} fs_stream_ctor_ud_t;

#endif /* LEXLEO_FS_STREAM_CTOR_UD_H */
