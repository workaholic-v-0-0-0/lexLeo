/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream_cr.c
 * @ingroup fs_stream_internal_group
 * @brief Composition Root support implementation for the `fs_stream` adapter.
 *
 * @details
 * Implements the CR-facing construction and registration services for the
 * `fs_stream` adapter, including backend creation, default initialization,
 * and adapter provider construction.
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
	const osal_mem_ops_t *mem_ops
) {
	LEXLEO_ASSERT(file_ops && mem_ops);

	return (fs_stream_env_t){
		.file_ops = file_ops,
		.mem_ops = mem_ops
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
		&& fs_stream->file
	);

	osal_file_status_t file_st = OSAL_FILE_STATUS_OK;
	size_t ret =
		fs_stream->file_ops->read(
			buf,
			1,
			n,
			fs_stream->file,
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
		&& fs_stream->file
	);

	osal_file_status_t file_st = OSAL_FILE_STATUS_OK;
	size_t ret =
		fs_stream->file_ops->write(
			buf,
			1,
			n,
			fs_stream->file,
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
		&& fs_stream->file
	);

	return
		map_osal_file_status(
			fs_stream->file_ops->flush(fs_stream->file)
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
	);

	stream_status_t close_st = STREAM_STATUS_OK;
	if (fs_stream->file) {
		close_st =
			map_osal_file_status(fs_stream->file_ops->close(fs_stream->file));

	}

	fs_stream->file = NULL;
	fs_stream->mem_ops->free(fs_stream);
	fs_stream = NULL;

	return close_st;
}

static const stream_vtbl_t g_fs_stream_vtbl = {
	.read = fs_stream_read,
	.write = fs_stream_write,
	.flush = fs_stream_flush,
	.close = fs_stream_close
};

const stream_vtbl_t *fs_stream_vtbl(void)
{
	return &g_fs_stream_vtbl;
}

fs_stream_status_t fs_stream_create(
	fs_stream_t **out,
	const fs_stream_env_t *env
) {
	LEXLEO_ASSERT(out && env && env->mem_ops && env->mem_ops->calloc);

	fs_stream_t *tmp = env->mem_ops->calloc(1, sizeof(*tmp));
	if (!tmp) {
		return FS_STREAM_STATUS_OOM;
	}

	tmp->mem_ops = env->mem_ops;
	tmp->file_ops = env->file_ops;

	*out = tmp;
	return FS_STREAM_STATUS_OK;
}

static fs_stream_status_t fs_stream_status_from_osal_file_status(
	osal_file_status_t status
) {
	switch (status) {
		case OSAL_FILE_STATUS_OK:
			return FS_STREAM_STATUS_OK;

		case OSAL_FILE_STATUS_EOF:
			return FS_STREAM_STATUS_EOF;

		case OSAL_FILE_STATUS_INVALID:
			return FS_STREAM_STATUS_INVALID;

		case OSAL_FILE_STATUS_NOENT:
			return FS_STREAM_STATUS_NOT_FOUND;

		case OSAL_FILE_STATUS_OOM:
			return FS_STREAM_STATUS_OOM;

		case OSAL_FILE_STATUS_PERM:
		case OSAL_FILE_STATUS_EXISTS:
		case OSAL_FILE_STATUS_NOSPC:
		case OSAL_FILE_STATUS_NAMETOOLONG:
		case OSAL_FILE_STATUS_NOTDIR:
		case OSAL_FILE_STATUS_ISDIR:
		case OSAL_FILE_STATUS_BADF:
		case OSAL_FILE_STATUS_FBIG:
		case OSAL_FILE_STATUS_INTR:
		case OSAL_FILE_STATUS_MFILE:
		case OSAL_FILE_STATUS_NFILE:
		case OSAL_FILE_STATUS_LOOP:
		case OSAL_FILE_STATUS_ROFS:
		case OSAL_FILE_STATUS_SPIPE:
		case OSAL_FILE_STATUS_XDEV:
		case OSAL_FILE_STATUS_NODEV:
		case OSAL_FILE_STATUS_NXIO:
		case OSAL_FILE_STATUS_STALE:
		case OSAL_FILE_STATUS_IO:
			return FS_STREAM_STATUS_IO_ERROR;

		default:
			LEXLEO_ASSERT(false);
			return FS_STREAM_STATUS_IO_ERROR;
	}
}

fs_stream_status_t fs_stream_complete_default_init(
	fs_stream_t *fs_stream,
	const fs_stream_cfg_t *cfg,
	const stream_regular_file_creator_args_t *args
) {
	(void)cfg;

	LEXLEO_ASSERT(
		   fs_stream
		&& fs_stream->file_ops
		&& fs_stream->file_ops->open
		&& cfg
		&& args
		&& (
			  osal_strcmp(args->mode, "rb") == 0
		   || osal_strcmp(args->mode, "wb") == 0
		   || osal_strcmp(args->mode, "ab") == 0
		)
	);

	if (fs_stream->file) {
		return FS_STREAM_STATUS_OK;
	}

	if (!args->path || *args->path == '\0' || osal_strlen(args->path) > 255) {
		return FS_STREAM_STATUS_INVALID;
	}

	OSAL_FILE *file = NULL;
	osal_file_status_t osal_file_st =
		fs_stream->file_ops->open(
			&file,
			args->path,
			args->mode,
			fs_stream->mem_ops
		);
	if (osal_file_st != OSAL_FILE_STATUS_OK) {
		return fs_stream_status_from_osal_file_status(osal_file_st);
	}

	LEXLEO_ASSERT(file);
	fs_stream->file = file;

	return FS_STREAM_STATUS_OK;
}

static stream_status_t fs_stream_status_to_stream_status(
	fs_stream_status_t status
) {
	switch (status) {
		case FS_STREAM_STATUS_OK: return STREAM_STATUS_OK;
		case FS_STREAM_STATUS_OOM: return STREAM_STATUS_OOM;
		case FS_STREAM_STATUS_INVALID: return STREAM_STATUS_INVALID;
		default: return STREAM_STATUS_IO_ERROR;
	}
}

static stream_status_t fs_stream_ctor(
	const void *ud,
	const void *args,
	void **out
) {
	const fs_stream_ctor_ud_t *fs_stream_ctor_ud =
		(const fs_stream_ctor_ud_t *)ud;
	const stream_regular_file_creator_args_t *stream_regular_file_creator_args =
		(const stream_regular_file_creator_args_t *)args;

	LEXLEO_ASSERT(
		   fs_stream_ctor_ud
		&& stream_regular_file_creator_args
		&& out
	);

	fs_stream_t *tmp = NULL;

	fs_stream_status_t fs_stream_st =
		fs_stream_create(
			&tmp,
			&fs_stream_ctor_ud->env
		);
	if (fs_stream_st != FS_STREAM_STATUS_OK) {
		return fs_stream_status_to_stream_status(fs_stream_st);
	}

	fs_stream_st =
		fs_stream_complete_default_init(
			tmp,
			&fs_stream_ctor_ud->cfg,
			stream_regular_file_creator_args
		);
	if (fs_stream_st != FS_STREAM_STATUS_OK) {
		fs_stream_vtbl()->close(tmp);
		return fs_stream_status_to_stream_status(fs_stream_st);
	}

	*out = tmp;
	return STREAM_STATUS_OK;
}

static void fs_stream_ctor_ud_dtor(
	void *ud,
	const osal_mem_ops_t *mem
) {
	fs_stream_ctor_ud_t *fs_stream_ctor_ud =
		(fs_stream_ctor_ud_t *)ud;

	LEXLEO_ASSERT(fs_stream_ctor_ud && mem && mem->free);

	mem->free(fs_stream_ctor_ud);
}

fs_stream_status_t fs_stream_create_adapter_provider(
	stream_adapter_provider_t **out,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env
) {
	LEXLEO_ASSERT(
		   out
		&& cfg
		&& env
		&& env->mem_ops
		&& env->mem_ops->calloc
		&& env->mem_ops->free
	);

	fs_stream_ctor_ud_t *fs_stream_ctor_ud =
		env->mem_ops->calloc(1, sizeof(*fs_stream_ctor_ud));
	if (!fs_stream_ctor_ud) {
		return FS_STREAM_STATUS_OOM;
	}

	fs_stream_ctor_ud->cfg = *cfg;
	fs_stream_ctor_ud->env = *env;

	stream_adapter_provider_t *tmp = env->mem_ops->calloc(1, sizeof(*tmp));
	if (!tmp) {
		fs_stream_ctor_ud_dtor(fs_stream_ctor_ud, env->mem_ops);
		return FS_STREAM_STATUS_OOM;
	}

	tmp->backend_ctor = fs_stream_ctor;
	tmp->vtbl = fs_stream_vtbl();
	tmp->ud = fs_stream_ctor_ud;
	tmp->ud_dtor = fs_stream_ctor_ud_dtor;
	tmp->mem = env->mem_ops;

	*out = tmp;
	return FS_STREAM_STATUS_OK;
}
