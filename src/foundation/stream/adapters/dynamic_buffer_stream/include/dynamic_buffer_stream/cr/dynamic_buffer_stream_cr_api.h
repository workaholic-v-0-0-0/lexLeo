/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/dynamic_buffer_stream/include/dynamic_buffer_stream
 * dynamic_buffer_stream_cr_api.h
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_CR_API_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_CR_API_H

#include "stream/adapters/stream_adapters_api.h"
#include "stream/adapters/stream_install.h"
#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_key_type.h"
#include "osal/mem/osal_mem_ops.h"

#include <stddef.h>

typedef struct dynamic_buffer_stream_cfg_t {
	size_t default_cap;
} dynamic_buffer_stream_cfg_t;

typedef struct dynamic_buffer_stream_env_t {
	const osal_mem_ops_t *mem;
	stream_env_t port_env;
} dynamic_buffer_stream_env_t;

dynamic_buffer_stream_cfg_t dynamic_buffer_stream_default_cfg(void);

dynamic_buffer_stream_env_t dynamic_buffer_stream_default_env(
	const osal_mem_ops_t *mem,
	const stream_env_t *port_env );

stream_status_t dynamic_buffer_stream_create_stream(
	stream_t **out,
	const dynamic_buffer_stream_cfg_t *cfg,
	const dynamic_buffer_stream_env_t *env );

// Factory callback delegating to create_stream
stream_status_t dynamic_buffer_stream_ctor(
	void *ud,
	const void *args,
	stream_t **out );

stream_status_t dynamic_buffer_stream_create_desc(
    stream_adapter_desc_t *out,
    stream_key_t key,
    const dynamic_buffer_stream_cfg_t *cfg,
    const dynamic_buffer_stream_env_t *env,
	const osal_mem_ops_t *mem ); // MUST BE THE FACTORY'S

// CR must register itself using dynamic_buffer_stream_create_desc

// args must be documented ; args = NULL for this adapter

#endif //LEXLEO_DYNAMIC_BUFFER_STREAM_CR_API_H