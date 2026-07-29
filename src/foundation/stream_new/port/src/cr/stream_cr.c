/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_cr.c
 * @ingroup stream_internal_group
 * @brief Composition Root implementation for the `stream` port.
 *
 * @details
 * Implements the CR-facing operations used to build default stream
 * environments, create and destroy stream factories, and register adapter
 * providers.
 */

#include "stream/cr/stream_cr_api.h"

#include "internal/stream_handle.h"
#include "internal/stream_factory_handle.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/str/osal_str.h"

#include "policy/lexleo_assert.h"

#define STREAM_FACTORY_DEFAULT_CAPACITY 16

stream_env_t stream_default_env(
	const stream_vtbl_t *vtbl,
	const osal_mem_ops_t *mem_ops
) {
	return (stream_env_t) {
		.vtbl = vtbl,
		.mem = mem_ops };
}

stream_status_t stream_create(
	stream_t **out,
	const stream_env_t *env
) {
	LEXLEO_ASSERT(
		   out
		&& env
		&& env->mem
		&& env->mem->calloc
		&& env->vtbl
		&& env->vtbl->read
		&& env->vtbl->write
		&& env->vtbl->flush
		&& env->vtbl->close
	);

	stream_t *tmp = env->mem->calloc(1, sizeof(*tmp));
	if (!tmp) {
		return STREAM_STATUS_OOM;
	}

	tmp->vtbl = env->vtbl;
	tmp->mem = env->mem;

	tmp->backend = NULL;

	*out = tmp;
	return STREAM_STATUS_OK;
}

stream_status_t stream_complete_default_init(
	stream_t *stream,
	void *backend
) {
	LEXLEO_ASSERT(stream);

	if (!stream->backend) {
		LEXLEO_ASSERT(backend);
		stream->backend = backend;
	} else {
		LEXLEO_ASSERT(!backend);
	}

	return STREAM_STATUS_OK;
}

stream_factory_cfg_t stream_default_factory_cfg(void)
{
	return (stream_factory_cfg_t){
		.fact_cap = STREAM_FACTORY_DEFAULT_CAPACITY
	};
}

typedef struct stream_ctor_ud_t {
	const osal_mem_ops_t *mem;
} stream_ctor_ud_t;

static stream_status_t stream_ctor(
	const void *ud,
	const stream_vtbl_t *vtbl,
	void *backend,
	stream_t **out
) {
	LEXLEO_ASSERT(
		   ud
		&& vtbl
		&& backend
		&& out
	);

	const osal_mem_ops_t *mem = ((const stream_ctor_ud_t *)ud)->mem;

	LEXLEO_ASSERT(mem && mem->calloc);

	stream_t *tmp = mem->calloc(1, sizeof(*tmp));
	if (!tmp) {
		return STREAM_STATUS_OOM;
	}

	tmp->vtbl = vtbl;
	tmp->mem = mem;
	tmp->backend = backend;

	*out = tmp;
	return STREAM_STATUS_OK;
}

static void stream_ctor_ud_dtor(
	void *ud,
	const osal_mem_ops_t *mem
) {
	LEXLEO_ASSERT(ud && mem && mem->free);
	mem->free((void*)ud);
}

stream_factory_status_t stream_create_factory(
	stream_factory_t **out,
	const stream_factory_cfg_t *cfg,
	const osal_mem_ops_t *mem
) {
	LEXLEO_ASSERT(
		   out
		&& mem
		&& cfg
		&& cfg->fact_cap > 0
		&& mem->calloc
		&& mem->free
	);

	stream_factory_t *tmp = mem->calloc(1, sizeof(*tmp));
	if (!tmp) {
		return STREAM_FACTORY_STATUS_OOM;
	}

	stream_ctor_ud_t *ctor_ud = mem->calloc(1, sizeof(*ctor_ud));
	if (!ctor_ud) {
		mem->free(tmp);
		return STREAM_FACTORY_STATUS_OOM;
	}
	ctor_ud->mem = mem;

	tmp->stream_provider.ctor = stream_ctor;
	tmp->stream_provider.ud = ctor_ud;
	tmp->stream_provider.ud_dtor = stream_ctor_ud_dtor;

	tmp->reg.entries = mem->calloc(cfg->fact_cap, sizeof(*tmp->reg.entries));
	if (!tmp->reg.entries) {
		tmp->stream_provider.ud_dtor(tmp->stream_provider.ud, mem);
		mem->free(tmp);
		return STREAM_FACTORY_STATUS_OOM;
	}

	tmp->reg.count = 0;
	tmp->reg.cap = cfg->fact_cap;
	tmp->mem = mem;

	*out = tmp;
	return STREAM_FACTORY_STATUS_OK;
}

static bool stream_factory_contains_id(
	const stream_factory_t *fact,
	stream_adapter_id_t adapter_id
) {
	LEXLEO_ASSERT(fact);

	for (size_t i = 0; i < fact->reg.count; ++i) {
		if (
			   fact->reg.entries[i].key
			&& osal_strcmp(fact->reg.entries[i].key, adapter_id) == 0
		) {
			return true;
		}
	}
	return false;
}

stream_factory_status_t stream_factory_add_adapter(
	stream_factory_t *fact,
	stream_adapter_id_t adapter_id,
	stream_adapter_provider_t *adapter_provider
) {
	LEXLEO_ASSERT(
		   fact
		&& fact->reg.entries
		&& adapter_id
		&& *adapter_id != '\0'
		&& osal_strlen(adapter_id) <= 255
		&& adapter_provider
	);

	if (fact->reg.count >= fact->reg.cap) {
		return STREAM_FACTORY_STATUS_FULL;
	}

	if (stream_factory_contains_id(fact, adapter_id)) {
		return STREAM_FACTORY_STATUS_ALREADY_EXISTS;
	}

	fact->reg.entries[fact->reg.count].key = adapter_id;
	fact->reg.entries[fact->reg.count++].value = adapter_provider;

	return STREAM_FACTORY_STATUS_OK;
}

void stream_destroy_adapter_provider(
	stream_adapter_provider_t *adapter_provider
) {
	LEXLEO_ASSERT(
		   adapter_provider
		&& adapter_provider->mem
		&& adapter_provider->mem->free
	);

	if (adapter_provider->ud_dtor) {
		adapter_provider->ud_dtor(adapter_provider->ud, adapter_provider->mem);
	}

	adapter_provider->mem->free(adapter_provider);
}

void stream_destroy_factory(stream_factory_t **fact)
{
	if (!fact || !*fact) {
		return;
	}

	LEXLEO_ASSERT(
		   (*fact)->mem
		&& (*fact)->mem->free
		&& (*fact)->reg.entries
		&& (*fact)->stream_provider.ud_dtor
	);

	for (size_t i = 0; i < (*fact)->reg.count; i++) {
		stream_destroy_adapter_provider((*fact)->reg.entries[i].value);
	}

	(*fact)->mem->free((*fact)->reg.entries);
	(*fact)->stream_provider.ud_dtor(
		(*fact)->stream_provider.ud,
		(*fact)->mem
	);
	(*fact)->mem->free(*fact);
	*fact = NULL;
}
