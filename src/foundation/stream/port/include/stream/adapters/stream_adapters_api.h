// src/foundation/stream/port/include/adapters/stream_adapters_api.h

#ifndef LEXLEO_STREAM_ADAPTERS_API_H
#define LEXLEO_STREAM_ADAPTERS_API_H

#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_constructor.h"

#include <stddef.h>

typedef struct stream_ctx_t stream_ctx_t; // to put in adapter ctx

typedef size_t (*stream_read_fn_t)(void *backend, void* buf, size_t n, stream_status_t *st);
typedef size_t (*stream_write_fn_t)(void *backend, const void* buf, size_t n, stream_status_t *st);
typedef stream_status_t (*stream_flush_fn_t)(void *backend);
typedef stream_status_t (*stream_close_fn_t)(void *backend);

typedef struct stream_vtbl_t {
	stream_read_fn_t read;
	stream_write_fn_t write;
	stream_flush_fn_t flush;
	stream_close_fn_t close;
} stream_vtbl_t;

// used by adapter to define its stream_ctor_fn_t
stream_status_t stream_create(
	stream_t **out,
	const stream_vtbl_t *vtbl,
	void *backend,
	const stream_ctx_t *ctx );

#endif //LEXLEO_STREAM_ADAPTERS_API_H