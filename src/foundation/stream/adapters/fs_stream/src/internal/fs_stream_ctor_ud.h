/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_ctor_ud.h
 * @ingroup fs_stream_internal_group
 * @brief Constructor user-data stored in the factory descriptor.
 */

#ifndef LEXLEO_FS_STREAM_CTOR_UD_H
#define LEXLEO_FS_STREAM_CTOR_UD_H

#include "fs_stream/cr/fs_stream_types.h"

/**
 * @brief User-data stored in the stream adapter descriptor.
 *
 * This structure is allocated by fs_stream_create_desc() and passed to the
 * factory constructor callback.
 */
typedef struct fs_stream_ctor_ud_t {
	fs_stream_cfg_t cfg;
	fs_stream_env_t env;
} fs_stream_ctor_ud_t;

#endif //LEXLEO_FS_STREAM_CTOR_UD_H
