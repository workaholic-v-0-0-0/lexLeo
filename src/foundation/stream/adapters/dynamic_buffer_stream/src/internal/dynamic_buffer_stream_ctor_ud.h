/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/dynamic_buffer_stream/src/internal/
 * dynamic_buffer_stream_ctor_ud.h
 *
 * Internal dynamic buffer stream backend structure.
 *
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_CTOR_UD_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_CTOR_UD_H

#include "dynamic_buffer_stream/cr/dynamic_buffer_stream_cr_api.h"
#include <stddef.h>

typedef struct dynamic_buffer_stream_ctor_ud_t {
	dynamic_buffer_stream_cfg_t cfg;
	dynamic_buffer_stream_env_t env;
} dynamic_buffer_stream_ctor_ud_t;

#endif //LEXLEO_DYNAMIC_BUFFER_STREAM_CTOR_UD_H
