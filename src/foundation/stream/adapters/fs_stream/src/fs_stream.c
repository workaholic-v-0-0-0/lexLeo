/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file fs_stream.c
 * @ingroup fs_stream_internal_group
 * @brief fs_stream adapter implementation (OSAL file backend).
 *
 * @details File-backed stream adapter. This module is a thin wrapper around OSAL file operations.
 */

#include "internal/fs_stream_handle.h"
#include "internal/fs_stream_ctor_ud.h"

#include "fs_stream/cr/fs_stream_cr_api.h"

#include "osal/mem/osal_mem.h"

#include "policy/lexleo_assert.h"

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
	size_t s = fs_stream->file_ops->read(fs_stream->state.f, buf, n, &osal_st);
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
	size_t s =
		fs_stream->file_ops->write(
			fs_stream->state.f, buf, n, &osal_st );

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

	osal_file_status_t osal_st =
		fs_stream->file_ops->flush(
			fs_stream->state.f );

    if (osal_st == OSAL_FILE_EOF)
        return STREAM_STATUS_IO_ERROR;

	return map_osal_status(osal_st);
}

static stream_status_t fs_stream_close(void *backend) {
	fs_stream_t *fs_stream = (fs_stream_t *)backend;
    if (!fs_stream) return STREAM_STATUS_INVALID;

    osal_file_status_t osal_st = OSAL_FILE_OK;

    if (fs_stream->state.f && fs_stream->state.autoclose) {
        osal_st = fs_stream->file_ops->close(fs_stream->state.f);
    }
    fs_stream->state.f = NULL;

    LEXLEO_ASSERT(fs_stream->mem_ops && fs_stream->mem_ops->free);
    fs_stream->mem_ops->free(fs_stream);

    if (osal_st == OSAL_FILE_EOF) return STREAM_STATUS_IO_ERROR;
    return map_osal_status(osal_st);
}

static const stream_vtbl_t VTBL = {
	.read = fs_stream_read,
	.write = fs_stream_write,
	.flush = fs_stream_flush,
	.close = fs_stream_close
};

static stream_status_t create_backend(
    fs_stream_t **out,
	const fs_stream_args_t *args,
    const fs_stream_cfg_t *cfg,
    const fs_stream_env_t *env)
{
    if (
    		   !out
			|| !args
			|| args->path == NULL
			|| *args->path == '\0'
			|| args->flags == 0
    		|| !cfg
    		|| !env ) {
        return STREAM_STATUS_INVALID;
    }

	LEXLEO_ASSERT(env->file_ops != NULL);
	LEXLEO_ASSERT(env->file_env.mem != NULL);

    fs_stream_t *backend =
        (fs_stream_t *)env->file_env.mem->calloc(1, sizeof(*backend));
    if (!backend) {
        return STREAM_STATUS_OOM;
    }

    backend->mem_ops = env->file_env.mem;
    backend->file_ops = env->file_ops;

	osal_file_status_t st = OSAL_FILE_ERR;
    backend->state.f =
		backend->file_ops->open(
			args->path,
			args->flags,
			&st,
			&env->file_env );

	stream_status_t sst = map_osal_status(st);
	if (!backend->state.f || sst != STREAM_STATUS_OK) {
    	env->file_env.mem->free(backend);
    	return (sst != STREAM_STATUS_OK) ? sst : STREAM_STATUS_IO_ERROR;
	}

	backend->state.autoclose = args->autoclose;

    *out = backend;
    return STREAM_STATUS_OK;
}

static void destroy_backend(fs_stream_t **p)
{
    if (!p || !*p) return;
    fs_stream_t *b = *p;
    *p = NULL;

    if (b->state.f && b->file_ops && b->file_ops->close) {
        (void)b->file_ops->close(b->state.f);
        b->state.f = NULL;
    }

    if (b->mem_ops && b->mem_ops->free) {
        b->mem_ops->free(b);
    }
}

static void fs_stream_destroy_ud_ctor(
	const void *ud,
	const osal_mem_ops_t *mem ) // MUST BE THE FACTORY'S
{
	if (!ud) return;
	LEXLEO_ASSERT(mem && mem->free);
	mem->free((void*)ud);
}

stream_status_t fs_stream_create_stream(
	stream_t **out,
	const fs_stream_args_t *args,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env )
{
	if (!out || !env) {
        return STREAM_STATUS_INVALID;
    }

    fs_stream_t *backend = NULL;
    stream_status_t st = create_backend(&backend, args, cfg, env);
    if (st != STREAM_STATUS_OK) return st;

	stream_t *tmp = NULL;
    st = stream_create(&tmp, &VTBL, backend, &env->port_env);
    if (st != STREAM_STATUS_OK) {
		destroy_backend(&backend);
        return st;
    }

	*out = tmp;
    return STREAM_STATUS_OK;
}

stream_status_t fs_stream_ctor(
	const void *ud,
	const void *args,
	stream_t **out )
{
	const fs_stream_ctor_ud_t *ctor_ud =
		(const fs_stream_ctor_ud_t *) ud;

	if (!out || !args || !ctor_ud) {
		return STREAM_STATUS_INVALID;
	}

	stream_t *tmp = NULL;
	stream_status_t st =
		fs_stream_create_stream(
			&tmp,
			(const fs_stream_args_t *)args,
			&ctor_ud->cfg,
			&ctor_ud->env );
	if (st == STREAM_STATUS_OK) *out = tmp;
	return st;
}

stream_status_t fs_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env,
	const osal_mem_ops_t *mem ) // MUST BE THE FACTORY'S
{
	if (out) *out = (stream_adapter_desc_t){0};

	stream_adapter_desc_t tmp = {0};

		if (
			   !out
			|| !key
			|| *key == '\0'
			|| !cfg
			|| !env
			|| !mem
			|| !env->file_env.mem
			|| !env->file_ops )
		return STREAM_STATUS_INVALID;

	tmp.key = key;
	tmp.ctor = fs_stream_ctor;

	fs_stream_ctor_ud_t *ud = mem->calloc(1, sizeof(*ud));
	if (!ud)
		return STREAM_STATUS_OOM;

	tmp.ud = ud;
	osal_memcpy(&ud->cfg, cfg, sizeof(*cfg));
	osal_memcpy(&ud->env, env, sizeof(*env));
	tmp.ud_dtor = fs_stream_destroy_ud_ctor;

	*out = tmp;
	return STREAM_STATUS_OK;
}

fs_stream_cfg_t fs_stream_default_cfg(void) {
	return (fs_stream_cfg_t) { .reserved = 0 };
}

fs_stream_env_t fs_stream_default_env(
	const osal_file_env_t *file_env,
	const osal_file_ops_t *file_ops,
	const stream_env_t *port_env )
{
	LEXLEO_ASSERT(file_env);
	LEXLEO_ASSERT(file_ops);
	LEXLEO_ASSERT(port_env);

	return (fs_stream_env_t){
		.file_env = *file_env,
		.file_ops = file_ops,
		.port_env = *port_env
	};
}
