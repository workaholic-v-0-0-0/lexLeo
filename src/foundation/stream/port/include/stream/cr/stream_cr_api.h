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
#include "mem/osal_mem_ops.h"

#include <stddef.h>

typedef struct stream_ctx_t {
	const osal_mem_ops_t *mem;
} stream_ctx_t;

void stream_destroy(stream_t *s); // stream_t creation via adapter or factory

stream_ctx_t stream_default_ctx(const osal_mem_ops_t *mem_ops);

const stream_ops_t *stream_default_ops(void);

#endif //LEXLEO_STREAM_CR_API_API_H
