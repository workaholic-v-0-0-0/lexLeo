/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_factory.c
 * @ingroup stream_internal_group
 * @brief Private implementation of the `stream` factory services.
 *
 * @details
 * This file implements:
 * - registry lookup helpers,
 * - factory creation and destruction,
 * - adapter registration,
 * - stream creation through registered adapters.
 */

#include "internal/stream_factory_handle.h"

#include "stream/cr/stream_factory_cr_api.h"

#include "osal/mem/osal_mem.h"

#include "policy/lexleo_cstd_types.h"

static const stream_branch_t *stream_registry_find(
	const stream_registry_t *reg,
	stream_key_t key)
{
	if (!reg || !reg->entries || !reg->count || !key) return NULL;

	for (size_t i = 0; i < reg->count; i++) {
		const stream_branch_t *e = &reg->entries[i];
		if (e->key && osal_strcmp(e->key, key) == 0) return e;
	}
	return NULL;
}

static stream_status_t stream_registry_create(
	const stream_registry_t *reg,
	stream_key_t key,
	const void *args,
	stream_t **out_stream)
{
	if (!out_stream) return STREAM_STATUS_INVALID;
	*out_stream = NULL;

	const stream_branch_t *e = stream_registry_find(reg, key);
	if (!e || !e->ctor) return STREAM_STATUS_NO_BACKEND;

	return e->ctor(e->ud, args, out_stream);
}

stream_status_t stream_create_factory(
    stream_factory_t **out,
    const stream_factory_cfg_t *cfg,
    const stream_env_t *env )
{
    if (
               !out
            || !cfg
            || !env
            || !env->mem
            || !env->mem->calloc
            || !env->mem->free )
        return STREAM_STATUS_INVALID;

    stream_factory_t *f = env->mem->calloc(1, sizeof(*f));
    if (!f) return STREAM_STATUS_OOM;

    f->mem = env->mem;
    f->reg.entries = NULL;
    f->reg.count = 0;
    f->reg.cap = 0;

    if (cfg->fact_cap > 0) {
        f->reg.entries = env->mem->calloc(cfg->fact_cap, sizeof(*f->reg.entries));
        if (!f->reg.entries) {
            env->mem->free(f);
            return STREAM_STATUS_OOM;
        }
        f->reg.cap = cfg->fact_cap;
    }

    *out = f;
    return STREAM_STATUS_OK;
}

void stream_destroy_factory(stream_factory_t **fact)
{
    if (!fact || !*fact) return;

    stream_factory_t *f = *fact;
    *fact = NULL;

    const osal_mem_ops_t *mem = f->mem;
    if (!mem || !mem->free) return;

    if (f->reg.entries) {
		for (size_t i = 0; i < f->reg.count; ++i) {
        	stream_branch_t *e = &f->reg.entries[i];
        	if (e->ud_dtor) e->ud_dtor(e->ud, mem);
    	}
		mem->free(f->reg.entries);
	}
    mem->free(f);
}

stream_status_t stream_factory_add_adapter(
    stream_factory_t *fact,
    const stream_adapter_desc_t *desc )
{
    if (
			   !fact
			|| !desc
			|| !desc->key
			|| *desc->key == '\0'
			|| !desc->ctor
			|| (desc->ud && !desc->ud_dtor) )
        return STREAM_STATUS_INVALID;

    stream_registry_t *reg = &fact->reg;

    if (!reg->entries || reg->cap == 0) {
        return STREAM_STATUS_INVALID;
    }

    if (reg->count >= reg->cap) {
        return STREAM_STATUS_FULL;
    }

    // Enforce uniqueness
    for (size_t i = 0; i < reg->count; ++i) {
        if (
				   reg->entries[i].key
				&& osal_strcmp(reg->entries[i].key, desc->key) == 0 ) {
 		   	return STREAM_STATUS_ALREADY_EXISTS; // duplicate key
		}
    }

    reg->entries[reg->count++] = (stream_branch_t){
        .key = desc->key,
        .ctor = desc->ctor,
        .ud = desc->ud,
		.ud_dtor = desc->ud_dtor
    };

    return STREAM_STATUS_OK;
}

stream_status_t stream_factory_create_stream(
    const stream_factory_t *f,
    stream_key_t key,
    const void *args,
    stream_t **out )
{
    if (!out || !f || !args || !key || *key == '\0')
    	return STREAM_STATUS_INVALID;

	if (!stream_registry_find(&f->reg, key))
		return STREAM_STATUS_NOT_FOUND;

	stream_t *tmp = NULL;
    stream_status_t st = stream_registry_create(&f->reg, key, args, &tmp);

	if (st == STREAM_STATUS_OK) *out = tmp;

	return st;
}
