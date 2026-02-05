/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/port/include/cr/
 * stream_cr_api.h
 */

#ifndef LEXLEO_STREAM_CR_API_API_H
#define LEXLEO_STREAM_CR_API_API_H

#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_env.h"

typedef struct stream_cfg_t {
	//
} stream_cfg_t;

const stream_ops_t *stream_default_ops(void);

stream_cfg_t stream_default_cfg(void);

stream_env_t stream_default_env(const osal_mem_ops_t *mem_ops);

#endif //LEXLEO_STREAM_CR_API_API_H
