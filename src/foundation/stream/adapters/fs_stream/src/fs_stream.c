/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream.c
 * @ingroup fs_stream_internal_group
 * @brief fs_stream adapter implementation.
 *
 * @details File-backed stream adapter.
 */

#include "internal/fs_stream_handle.h"
#include "internal/fs_stream_ctor_ud.h"

#include "fs_stream/cr/fs_stream_cr_api.h"

#include "osal/mem/osal_mem.h"
#include "osal/str/osal_str.h"

#include "policy/lexleo_assert.h"

fs_stream_cfg_t fs_stream_default_cfg(void)
{
	return (fs_stream_cfg_t) { .reserved = 0 };
}

fs_stream_env_t fs_stream_default_env(
	const osal_file_ops_t *file_ops,
	const osal_mem_ops_t *adapter_mem_ops,
	const stream_env_t *port_env
) {
	LEXLEO_ASSERT(
		   file_ops
		&& adapter_mem_ops
		&& port_env
	);
	return (fs_stream_env_t){
		.file_ops = file_ops,
		.adapter_mem_ops = adapter_mem_ops,
		.port_env = *port_env
	};
}

static stream_status_t map_osal_file_status(osal_file_status_t st)
{
	switch (st) {
		case OSAL_FILE_STATUS_OK:
			return STREAM_STATUS_OK;

		case OSAL_FILE_STATUS_EOF:
			return STREAM_STATUS_EOF;

		case OSAL_FILE_STATUS_INVALID:
			return STREAM_STATUS_INVALID;

		case OSAL_FILE_STATUS_OOM:
			return STREAM_STATUS_OOM;

		case OSAL_FILE_STATUS_NOENT:
			return STREAM_STATUS_NOT_FOUND;

		case OSAL_FILE_STATUS_EXISTS:
			return STREAM_STATUS_ALREADY_EXISTS;

		default:
			return STREAM_STATUS_IO_ERROR;
	}
}

static size_t fs_stream_read(
	void *backend,
	void *buf,
	size_t n,
	stream_status_t *st
) {
	LEXLEO_ASSERT( /* via stream_read() contract */
		   backend
		&& buf
		&& n > 0
	);

	fs_stream_t *fs_stream = (fs_stream_t *)backend;

	LEXLEO_ASSERT(
		   fs_stream->file_ops
		&& fs_stream->file_ops->read
		&& fs_stream->state.file
	);

	osal_file_status_t file_st = OSAL_FILE_STATUS_OK;
	size_t ret =
		fs_stream->file_ops->read(
			buf,
			1,
			n,
			fs_stream->state.file,
			&file_st
		);
	if (st) {
		*st = map_osal_file_status(file_st);
	}
	return ret;
}

static size_t fs_stream_write(
	void *backend,
	const void *buf,
	size_t n,
	stream_status_t *st
) {
	LEXLEO_ASSERT( /* via stream_write() contract */
		   backend
		&& buf
		&& n > 0
	);

	fs_stream_t *fs_stream = (fs_stream_t *)backend;

	LEXLEO_ASSERT(
		   fs_stream->file_ops
		&& fs_stream->file_ops->write
		&& fs_stream->state.file
	);

	osal_file_status_t file_st = OSAL_FILE_STATUS_OK;
	size_t ret =
		fs_stream->file_ops->write(
			buf,
			1,
			n,
			fs_stream->state.file,
			&file_st
		);
	if (st) {
		*st = map_osal_file_status(file_st);
	}
	return ret;
}

static stream_status_t fs_stream_flush(
	void *backend
) {
	LEXLEO_ASSERT(backend); /* via stream_flush() contract */

	fs_stream_t *fs_stream = (fs_stream_t *)backend;

	LEXLEO_ASSERT(
		   fs_stream->file_ops
		&& fs_stream->file_ops->flush
		&& fs_stream->state.file
	);

	return
		map_osal_file_status(
			fs_stream->file_ops->flush(fs_stream->state.file)
		);
}

static stream_status_t fs_stream_close(
	void *backend
) {
	LEXLEO_ASSERT(backend); /* via stream_destroy() contract */

	fs_stream_t *fs_stream = (fs_stream_t *)backend;

	LEXLEO_ASSERT(
		   fs_stream->file_ops
		&& fs_stream->file_ops->close
		&& fs_stream->mem_ops
		&& fs_stream->mem_ops->free
		&& fs_stream->state.file
	);

	stream_status_t close_st =
		map_osal_file_status(
			fs_stream->file_ops->close(fs_stream->state.file)
		);

	if (close_st == STREAM_STATUS_OK) {
		fs_stream->state.file = NULL;
		fs_stream->mem_ops->free(fs_stream);
	}

	return close_st;
}

static const stream_vtbl_t g_fs_stream_vtbl = {
	.read = fs_stream_read,
	.write = fs_stream_write,
	.flush = fs_stream_flush,
	.close = fs_stream_close
};

stream_status_t fs_stream_create_stream(
	stream_t **out,
	const fs_stream_args_t *args,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env
) {
	(void)cfg; // reserved for future use

	if (
		   !out
		|| !args
		|| !args->path
		|| *args->path == '\0'
		|| !args->mode
		|| (   osal_strcmp(args->mode, "rb") != 0
			&& osal_strcmp(args->mode, "wb") != 0
			&& osal_strcmp(args->mode, "ab") != 0 )
		|| !cfg
		|| !env
	) {
		return STREAM_STATUS_INVALID;
	}

	LEXLEO_ASSERT(
		   env->file_ops
		&& env->file_ops->open
		&& env->file_ops->close
		&& env->adapter_mem_ops
		&& env->adapter_mem_ops->calloc
		&& env->adapter_mem_ops->free
	);

	fs_stream_t *fs_stream = env->adapter_mem_ops->calloc(1, sizeof(*fs_stream));
	if (!fs_stream) {
		return STREAM_STATUS_OOM;
	}

	OSAL_FILE *file = NULL;
	osal_file_status_t open_st =
		env->file_ops->open(
			&file,
			args->path,
			args->mode,
			env->adapter_mem_ops
		);
	if (open_st != OSAL_FILE_STATUS_OK) {
		env->adapter_mem_ops->free(fs_stream);
		return STREAM_STATUS_IO_ERROR;
	}

	fs_stream->state.file = file;
	fs_stream->file_ops = env->file_ops;
	fs_stream->mem_ops = env->adapter_mem_ops;

	stream_t *out_stream = NULL;
	stream_status_t stream_st =
		stream_create(
			&out_stream,
			&g_fs_stream_vtbl,
			fs_stream,
			&env->port_env);
	if (stream_st != STREAM_STATUS_OK) {
		env->file_ops->close(file);
		env->adapter_mem_ops->free(fs_stream);
		return stream_st;
	}

	*out = out_stream;

	return STREAM_STATUS_OK;
}

static stream_status_t fs_stream_ctor(
	const void *ud,
	const void *args,
	stream_t **out
) {
	const fs_stream_ctor_ud_t *casted_ud =
		(const fs_stream_ctor_ud_t *)ud;

	if (!casted_ud || !args || !out) {
		return STREAM_STATUS_INVALID;
	}

	return fs_stream_create_stream(
		out,
		args,
		&casted_ud->cfg,
		&casted_ud->env
	);
}

static void fs_stream_ud_dtor(const void *ud, const osal_mem_ops_t *mem
) {
	LEXLEO_ASSERT(ud && mem && mem->free);
	mem->free((void*)ud);
}

stream_status_t fs_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env,
	const osal_mem_ops_t *mem
) {
	if (out) {
		*out = (stream_adapter_desc_t){0};
	}

	if (
		   !out
		|| !key
		|| *key == '\0'
		|| !cfg
		|| !env
		|| !mem
	) {
		return STREAM_STATUS_INVALID;
	}

	LEXLEO_ASSERT(
		   mem->calloc
		&& mem->free
	);

	fs_stream_ctor_ud_t *ud = mem->calloc(1, sizeof(*ud));
	if (!ud) {
		return STREAM_STATUS_OOM;
	}

	ud->cfg = *cfg;
	ud->env = *env;

	out->key = key;
	out->ctor = fs_stream_ctor;
	out->ud = (void*)ud;
	out->ud_dtor = fs_stream_ud_dtor;

	return STREAM_STATUS_OK;
}
