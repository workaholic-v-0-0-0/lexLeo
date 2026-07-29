/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_owner.c
 * @ingroup stream_internal_group
 * @brief Owner-facing stream operation implementation.
 *
 * @details
 * Implements stream destruction and the internal factory-based construction
 * of complete `stream_t` handles.
 */

#include "stream/common/stream_factory_opaque_type.h"
#include "stream/common/stream_factory_status_type.h"
#include "stream/owners/stream_owners_api.h"
#include "stream/adapters/stream_adapters_types.h"

#include "internal/stream_handle.h"
#include "internal/stream_factory_handle.h"
#include "internal/stream_internal_api.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/str/osal_str.h"

#include "policy/lexleo_assert.h"

void stream_destroy(stream_t **s)
{
	if (!s || !*s) {
		return;
	}

	stream_t *stream = *s;

	LEXLEO_ASSERT(
		   stream->vtbl
		&& stream->vtbl->close
		&& stream->mem
		&& stream->mem->free
	);

	if (stream->backend) {
		(void)stream->vtbl->close(stream->backend);
	}

	stream->mem->free(stream);
	*s = NULL;
}

static const stream_adapter_provider_t *stream_factory_get_adapter_provider(
	const stream_factory_t *factory,
	const stream_adapter_id_t key
) {
	LEXLEO_ASSERT(
		   factory
		&& factory->reg.entries
		&& key
		&& *key != '\0'
	);

	for (size_t i = 0; i < factory->reg.count; i++) {
		if (osal_strcmp(key, factory->reg.entries[i].key) == 0) {
			return factory->reg.entries[i].value;
		}
	}

	return NULL;
}

stream_status_t stream_factory_status_to_stream_status(
	stream_factory_status_t factory_status
) {
	switch (factory_status) {
		case STREAM_FACTORY_STATUS_OK: return STREAM_STATUS_OK;
		case STREAM_FACTORY_STATUS_EOF: return STREAM_STATUS_EOF;
		case STREAM_FACTORY_STATUS_INVALID: return STREAM_STATUS_INVALID;
		case STREAM_FACTORY_STATUS_NOT_FOUND: return STREAM_STATUS_NOT_FOUND;
		case STREAM_FACTORY_STATUS_NO_BACKEND: return STREAM_STATUS_NO_BACKEND;
		case STREAM_FACTORY_STATUS_IO_ERROR: return STREAM_STATUS_IO_ERROR;
		case STREAM_FACTORY_STATUS_OOM: return STREAM_STATUS_OOM;
		case STREAM_FACTORY_STATUS_ALREADY_EXISTS:
		case STREAM_FACTORY_STATUS_FULL:
		default:
			LEXLEO_ASSERT(false);
			return STREAM_STATUS_IO_ERROR;
	}
}

stream_factory_status_t stream_status_to_stream_factory_status(
	stream_status_t status
) {
	switch (status) {
		case STREAM_STATUS_OK: return STREAM_FACTORY_STATUS_OK;
		case STREAM_STATUS_EOF: return STREAM_FACTORY_STATUS_EOF;
		case STREAM_STATUS_INVALID: return STREAM_FACTORY_STATUS_INVALID;
		case STREAM_STATUS_NOT_FOUND: return STREAM_FACTORY_STATUS_NOT_FOUND;
		case STREAM_STATUS_NO_BACKEND: return STREAM_FACTORY_STATUS_NO_BACKEND;
		case STREAM_STATUS_IO_ERROR: return STREAM_FACTORY_STATUS_IO_ERROR;
		case STREAM_STATUS_OOM: return STREAM_FACTORY_STATUS_OOM;
		default: LEXLEO_ASSERT(false); return STREAM_FACTORY_STATUS_IO_ERROR;
	}
}

stream_factory_status_t stream_factory_create_stream(
	const stream_factory_t *factory,
	stream_adapter_id_t adapter_id,
	const void *args,
	stream_t **out
) {
	LEXLEO_ASSERT(
		   factory
		&& factory->reg.entries
		&& factory->stream_provider.ctor
		&& adapter_id
		&& *adapter_id != '\0'
		&& out
	);

	const stream_adapter_provider_t *adapter_provider =
		stream_factory_get_adapter_provider(
			factory,
			adapter_id
		);
	if (!adapter_provider) {
		return STREAM_FACTORY_STATUS_NOT_FOUND;
	}

	LEXLEO_ASSERT(
		   adapter_provider->backend_ctor
		&& adapter_provider->vtbl
		&& adapter_provider->vtbl->close
	);

	void *backend = NULL;
	stream_status_t st =
		adapter_provider->backend_ctor(
			adapter_provider->ud,
			args,
			&backend
		);
	if (st != STREAM_STATUS_OK) {
		return stream_status_to_stream_factory_status(st);
	}
	LEXLEO_ASSERT(backend);

	stream_t *tmp = NULL;

	st =
		factory->stream_provider.ctor(
			factory->stream_provider.ud,
			adapter_provider->vtbl,
			backend,
			&tmp
		);
	if (st != STREAM_STATUS_OK) {
		(void)adapter_provider->vtbl->close(backend);
		return stream_status_to_stream_factory_status(st);
	}
	LEXLEO_ASSERT(tmp);

	*out = tmp;
	return STREAM_FACTORY_STATUS_OK;
}
