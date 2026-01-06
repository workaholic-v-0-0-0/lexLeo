/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/adapters/stream/dynamic_buffer_stream/include/internal/
 * stream_ctx.h
 */

#ifndef LEXLEO_STREAM_CTX_H
#define LEXLEO_STREAM_CTX_H

#include "stream_types.h"
#include "osal_mem_ops.h"
#include <stddef.h>

typedef struct stream_ctx_t {
	const osal_mem_ops_t *mem;
} stream_ctx_t;

typedef size_t (*stream_read_fn_t)(void *backend, void* buf, size_t n);
typedef size_t (*stream_write_fn_t)(void *backend, const void* buf, size_t n);
typedef int (*stream_flush_fn_t)(void *backend);
typedef int (*stream_close_fn_t)(void *backend);

typedef struct stream_vtbl_t {
	stream_read_fn_t read;
	stream_write_fn_t write;
	stream_flush_fn_t flush;
	stream_close_fn_t close;
} stream_vtbl_t;

typedef enum {
	STREAM_STATUS_OK = 0,
	STREAM_STATUS_ERROR = 1
} stream_status_t;

stream_status_t stream_create(
	stream_t **out,
	const stream_vtbl_t *vtbl,
	void *backend,
	const stream_ctx_t *ctx );

void stream_destroy(stream_t *s);

#endif //LEXLEO_STREAM_CTX_H

// src/ports/stream/include/internal/stream_ctx.h