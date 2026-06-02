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
 * This header defines the private user data object allocated by
 * `fs_stream_create_desc()` and later consumed by `fs_stream_ctor()`.
 */

#ifndef LEXLEO_FS_STREAM_CTOR_UD_H
#define LEXLEO_FS_STREAM_CTOR_UD_H

#include "fs_stream/cr/fs_stream_types.h"

/**
 * @brief Private constructor user data for `fs_stream` registration.
 *
 * @details
 * Stores the configuration and environment captured when building the adapter
 * descriptor.
 */
typedef struct fs_stream_ctor_ud_t {

	/** Bound adapter configuration. */
	fs_stream_cfg_t cfg;

	/** Bound adapter environment. */
	fs_stream_env_t env;

} fs_stream_ctor_ud_t;

#endif /* LEXLEO_FS_STREAM_CTOR_UD_H */
