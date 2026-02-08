/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/fs_stream/src/
 * fs_stream.c
 */

// this adapter module is more a glue to the relative osal_file adapter than an adapter itself

#include "stream/borrowers/stream_types.h"
#include "stream/borrowers/stream.h"
#include "wiring/fs_stream.h"
#include "internal/fs_stream_handle.h"
#include "mem/osal_mem_ops.h"
#include "file/osal_file.h"

fs_stream_ctx_t fs_stream_default_ctx(
		const osal_mem_ops_t *mem_ops ) {
	fs_stream_ctx_t ctx;
	ctx.deps.mem = mem_ops ? mem_ops : osal_mem_default_ops();
	return ctx;
}

static stream_status_t map_osal_status(osal_file_status_t osal_st) {
	switch (osal_st) {
        case OSAL_FILE_OK:
            return STREAM_STATUS_OK;

        case OSAL_FILE_EOF:
            return STREAM_STATUS_EOF;

        case OSAL_FILE_ERR:
        case OSAL_FILE_IO:
        case OSAL_FILE_NOENT:
        case OSAL_FILE_PERM:
        case OSAL_FILE_NOSYS:
            return STREAM_STATUS_IO_ERROR;

        default:
            return STREAM_STATUS_IO_ERROR;
    }
}

static size_t fs_stream_read(
	void *backend,
	void* buf,
	size_t n,
	stream_status_t *st )
{
	fs_stream_t *fs_stream = (fs_stream_t *)backend;
	if (!fs_stream || (!buf && n)) {
		if (st) *st = STREAM_STATUS_INVALID;
		return (size_t)0;
	}
	if (!fs_stream->state.f && n)  {
		if (st) *st = STREAM_STATUS_NO_BACKEND;
		return (size_t)0;
	}
	osal_file_status_t osal_st = OSAL_FILE_ERR;
	size_t s = osal_file_read(fs_stream->state.f, buf, n, &osal_st);
	if (st) *st = map_osal_status(osal_st);
	return s;
}

static size_t fs_stream_write(
	void *backend,
	const void* buf,
	size_t n,
	stream_status_t *st )
{
	fs_stream_t *fs_stream = (fs_stream_t *)backend;
	if (!fs_stream || (!buf && n)) {
		if (st) *st = STREAM_STATUS_INVALID;
		return (size_t)0;
	}
	if (!fs_stream->state.f && n)  {
		if (st) *st = STREAM_STATUS_NO_BACKEND;
		return (size_t)0;
	}
	osal_file_status_t osal_st = OSAL_FILE_ERR;
	size_t s = osal_file_write(fs_stream->state.f, buf, n, &osal_st);

	if (st)
		*st =
			(osal_st == OSAL_FILE_EOF) ?
			STREAM_STATUS_IO_ERROR
			:
			map_osal_status(osal_st);
	return s;
}

static stream_status_t fs_stream_flush(void *backend) {
	fs_stream_t *fs_stream = (fs_stream_t *)backend;
	if (!fs_stream)
		return STREAM_STATUS_INVALID;
	if (!fs_stream->state.f)
		 return STREAM_STATUS_NO_BACKEND;

	osal_file_status_t osal_st = osal_file_flush(fs_stream->state.f);

    if (osal_st == OSAL_FILE_EOF)
        return STREAM_STATUS_IO_ERROR;

	return map_osal_status(osal_st);
}

static stream_status_t fs_stream_close(void *backend) {
	fs_stream_t *fs_stream = (fs_stream_t *)backend;
	if (!fs_stream)
		return STREAM_STATUS_INVALID;
	if (!fs_stream->state.f || !fs_stream->state.autoclose)
		 return STREAM_STATUS_OK;

	osal_file_status_t osal_st = osal_file_close(fs_stream->state.f);
	fs_stream->state.f = NULL;

	if (osal_st == OSAL_FILE_EOF)
	    return STREAM_STATUS_IO_ERROR;

	return map_osal_status(osal_st);
}

static const stream_vtbl_t DEFAULT_VTBL = {
	.read = fs_stream_read,
	.write = fs_stream_write,
	.flush = fs_stream_flush,
	.close = fs_stream_close
};



//open, factory