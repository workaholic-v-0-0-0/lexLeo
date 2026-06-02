/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_cr.c
 * @ingroup stream_internal_group
 * @brief Composition Root implementation for the `stream` port.
 *
 * @details
 * This file implements the CR-facing helpers used to build default stream
 * environments, create and destroy stream factories, and register adapter
 * descriptors.
 */

#include "stream/cr/stream_cr_api.h"

#include "../internal/stream_factory_handle.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/str/osal_str.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_assert.h"

#define STREAM_FACTORY_DEFAULT_CAPACITY 16

stream_factory_cfg_t stream_default_factory_cfg(void)
{
	return (stream_factory_cfg_t) {
		.fact_cap = STREAM_FACTORY_DEFAULT_CAPACITY
	};
}

stream_status_t stream_create_factory(
    stream_factory_t **out,
    const stream_factory_cfg_t *cfg,
    const osal_mem_ops_t *mem)
{
    if (
               !out
            || !cfg
            || !mem
    ) {
    	return STREAM_STATUS_INVALID;
    }

	LEXLEO_ASSERT(
		   mem->calloc
		&& mem->free
	);

    stream_factory_t *f = mem->calloc(1, sizeof(*f));
    if (!f) {
	    return STREAM_STATUS_OOM;
    }

    f->mem = mem;
    f->reg.entries = NULL;
    f->reg.count = 0;
    f->reg.cap = 0;

    if (cfg->fact_cap > 0) {
        f->reg.entries = mem->calloc(cfg->fact_cap, sizeof(*f->reg.entries));
        if (!f->reg.entries) {
            mem->free(f);
            return STREAM_STATUS_OOM;
        }
        f->reg.cap = cfg->fact_cap;
    }

    *out = f;
    return STREAM_STATUS_OK;
}

void stream_destroy_factory(stream_factory_t **fact)
{
    if (!fact || !*fact) {
	    return;
    }

    stream_factory_t *f = *fact;
    *fact = NULL;

    const osal_mem_ops_t *mem = f->mem;

	LEXLEO_ASSERT(mem && mem->free);

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
    const stream_adapter_desc_t *desc
) {
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
			&& osal_strcmp(reg->entries[i].key, desc->key) == 0
		) {
 		   	return STREAM_STATUS_ALREADY_EXISTS;
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
