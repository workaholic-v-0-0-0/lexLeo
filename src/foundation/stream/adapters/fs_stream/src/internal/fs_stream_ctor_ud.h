/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/fs_stream/src/internal/
 * fs_stream_ctor_ud.h
 *
 * Internal file system stream backend structure.
 *
 */

#ifndef LEXLEO_FS_STREAM_CTOR_UD_H
#define LEXLEO_FS_STREAM_CTOR_UD_H

#include "fs_stream/cr/fs_stream_types.h"

typedef struct fs_stream_ctor_ud_t {
	const fs_stream_cfg_t *cfg; // migre to not a pointer when evoluting
	fs_stream_env_t env;
} fs_stream_ctor_ud_t;

#endif //LEXLEO_FS_STREAM_CTOR_UD_H
