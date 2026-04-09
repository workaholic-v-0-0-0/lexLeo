/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream.c
 * @ingroup stream_internal_group
 * @brief Private implementation of the `stream` port.
 */

#include "internal/stream_handle.h"

#include "stream/borrowers/stream.h"
#include "stream/lifecycle/stream_lifecycle.h"
#include "stream/adapters/stream_adapters_api.h"

#include "policy/lexleo_assert.h"

size_t stream_read(stream_t *s, void *buf, size_t n, stream_status_t *st) {
    if (n == 0) {
		if (st) *st = STREAM_STATUS_OK;
		return (size_t)0;
	}

    if (!s || !buf) {
        if (st) *st = STREAM_STATUS_INVALID;
        return (size_t)0;
    }

    if (!s->backend) {
        if (st) *st = STREAM_STATUS_NO_BACKEND;
        return (size_t)0;
    }

    return s->vtbl.read(s->backend, buf, n, st);
}

size_t stream_write(
	stream_t *s,
	const void *buf,
	size_t n,
	stream_status_t *st )
{
    if (n == 0) {
		if (st) *st = STREAM_STATUS_OK;
		return (size_t)0;
	}

    if (!s || !buf) {
        if (st) *st = STREAM_STATUS_INVALID;
        return (size_t)0;
    }

    if (!s->backend) {
        if (st) *st = STREAM_STATUS_NO_BACKEND;
        return (size_t)0;
    }

    return s->vtbl.write(s->backend, buf, n, st);
}

stream_status_t stream_flush(stream_t *s) {
    if (!s || !s->vtbl.flush) return STREAM_STATUS_INVALID;
    if (!s->backend) return STREAM_STATUS_NO_BACKEND;

    return s->vtbl.flush(s->backend);
}

stream_status_t stream_create(
		stream_t **out,
		const stream_vtbl_t *vtbl,
		void *backend,
		const stream_env_t *env )
{
	if (
			   !out
			|| !vtbl
			|| !vtbl->read
			|| !vtbl->write
			|| !vtbl->flush
			|| !vtbl->close
			|| !env
			|| !env->mem )
		return STREAM_STATUS_INVALID;

	stream_t *tmp = env->mem->calloc(1, sizeof(*tmp));
	if (!tmp)
		return STREAM_STATUS_OOM;

	tmp->vtbl = *vtbl;
	tmp->backend = backend;
	tmp->mem = env->mem;

	*out = tmp;
	return STREAM_STATUS_OK;
}

void stream_destroy(stream_t **s)
{
	if (!s || !*s) {
		return;
	}

	stream_t *stream = *s;
	*s = NULL;

	const osal_mem_ops_t *mem = stream->mem;
	stream_close_fn_t close_fn = stream->vtbl.close;
	void *backend = stream->backend;

	if (close_fn) {
		stream_status_t st = close_fn(backend);
		LEXLEO_ASSERT(
			st == STREAM_STATUS_OK ||
			st == STREAM_STATUS_IO_ERROR ||
			st == STREAM_STATUS_NO_BACKEND);
	}

	LEXLEO_ASSERT(mem && mem->free);
	mem->free(stream);
}

const stream_ops_t *stream_default_ops(void) {
	static const stream_ops_t OPS = {
		.read = stream_read,
		.write = stream_write,
		.flush = stream_flush
	};
	return &OPS;
}

stream_env_t stream_default_env(const osal_mem_ops_t *mem_ops) {
	return (stream_env_t) { .mem = mem_ops };
}