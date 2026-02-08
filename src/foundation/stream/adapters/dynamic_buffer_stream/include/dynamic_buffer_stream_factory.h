/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/dynamic_buffer_stream/include/
 * dynamic_buffer_stream_factory.h
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_FACTORY_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_FACTORY_H

#include "stream/borrowers/stream.h"
#include "mem/osal_mem.h"
#include "mem/osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dynamic_buffer_stream_factory {
	void *userdata;
	stream_status_t (*create)(void *userdata, stream_t **out);
	const osal_mem_ops_t *mem;
} dynamic_buffer_stream_factory_t;

#ifdef __cplusplus
} // extern "C"
#endif

#endif //LEXLEO_DYNAMIC_BUFFER_STREAM_FACTORY_H
