/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/adapters/fs_stream/src/
 * fs_stream.c
 *
 * File system stream backend implementation (adapter).
 * this adapter module is rather a glue to the relative
 * osal_file adapter than an adapter itself
 */

#include "internal/fs_stream_handle.h"
#include "internal/fs_stream_ctor_ud.h"
#include "fs_stream/cr/fs_stream_cr_api.h"
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
    const fs_stream_cfg_t *cfg, // MUST BE NULL FOR NOW
    const fs_stream_env_t *env)
{
    if (out) *out = NULL;

    if (
    		   !out
			|| !args
			|| args->path == NULL
			|| *args->path == '\0'
			|| args->flags == 0
    		|| cfg // to be reversed when evoluting
    		|| !env
			|| !env->file_env.mem
    		|| !env->file_env.mem->calloc
    		|| !env->file_env.mem->free
    		|| !env->file_ops
    		|| !env->file_ops->open
    		|| !env->file_ops->read
    		|| !env->file_ops->write
    		|| !env->file_ops->flush
    		|| !env->file_ops->close ) {
        return STREAM_STATUS_INVALID;
    }

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

stream_status_t fs_stream_create_stream(
	stream_t **out,
	const fs_stream_args_t *args,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env )
{
	if (out) *out = NULL;
    if (!out || !args || !env) return STREAM_STATUS_INVALID;

    fs_stream_t *backend = NULL;
    stream_status_t st = create_backend(&backend, args, cfg, env);
    if (st != STREAM_STATUS_OK) return st;

    st = stream_create(out, &VTBL, backend, &env->port_env);
    if (st != STREAM_STATUS_OK) {
        if (backend && backend->mem_ops && backend->mem_ops->free)
			backend->mem_ops->free(backend);
        return st;
    }
    return STREAM_STATUS_OK;
}

stream_status_t fs_stream_ctor(
	void *ud,
	const void *args,
	stream_t **out )
{
	if (out) *out = NULL;

	fs_stream_ctor_ud_t *ctor_ud =
		(fs_stream_ctor_ud_t *) ud;

	if (!out || !args || !ctor_ud || ctor_ud->cfg != NULL) {
		return STREAM_STATUS_INVALID;
	}

	return
		fs_stream_create_stream(
			out,
			(const fs_stream_args_t *)args,
			ctor_ud->cfg, // NULL for now ; when evoluting, put a &
			&ctor_ud->env );
}

static void fs_stream_destroy_ud_ctor(
	void *ud,
	const osal_mem_ops_t *mem ) // MUST BE THE FACTORY'S
{
	if (!ud) return;
	LEXLEO_ASSERT(mem && mem->free);
	mem->free(ud);
}

stream_status_t fs_stream_create_desc(
	stream_adapter_desc_t *out,
	stream_key_t key,
	const fs_stream_cfg_t *cfg,
	const fs_stream_env_t *env,
	const osal_mem_ops_t *mem ) // MUST BE THE FACTORY'S
{
	if (out) *out = (stream_adapter_desc_t){0};

		if (
			   !out
			|| !key
			|| *key == '\0'
			|| cfg // empty for now
			|| !env
			|| !mem
			|| !mem->calloc
			|| !mem->free
			|| !mem->memcpy
			|| !env->file_env.mem
			|| !env->file_env.mem->calloc
			|| !env->file_env.mem->free
			|| !env->file_ops
			|| !env->file_ops->open
			|| !env->file_ops->close
			|| !env->file_ops->read
			|| !env->file_ops->write
			|| !env->file_ops->flush )
		return STREAM_STATUS_INVALID;

	out->key = key;
	out->ctor = fs_stream_ctor;

	fs_stream_ctor_ud_t *ud = mem->calloc(1, sizeof(*ud));
	if (!ud)
		return STREAM_STATUS_OOM;

	ud->cfg = NULL; // cfg unused for now

	// decomment when cfg evoluting
	// mem->memcpy(&ud->cfg, cfg, sizeof(*cfg));
	mem->memcpy(&ud->env, env, sizeof(*env));
	out->ud = ud;
	out->ud_dtor = fs_stream_destroy_ud_ctor;

	return STREAM_STATUS_OK;
}

fs_stream_cfg_t fs_stream_default_cfg(void) {
	return
		(fs_stream_cfg_t)
			{ /* to be completed when evoluating */ };
}

fs_stream_env_t fs_stream_default_env(
	const osal_file_env_t *file_env,
	const osal_file_ops_t *file_ops,
	const stream_env_t *port_env )
{
	fs_stream_env_t env;
	env.file_env = file_env ? *file_env : osal_file_default_env(NULL);
	env.file_ops = file_ops ? file_ops : osal_file_default_ops();
	LEXLEO_ASSERT(port_env);
	env.port_env = *port_env;
	return env;
}
