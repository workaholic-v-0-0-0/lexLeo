/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_owner.c
 * @ingroup stream_internal_group
 * @brief Owner-facing stream operation implementation.
 *
 * @details
 * This file implements owner-facing destruction and factory-based stream
 * creation services.
 */

#include "stream/owners/stream_owners_api.h"
#include "internal/stream_factory_create_stream.h"

#include "internal/stream_handle.h"
#include "internal/stream_factory_handle.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/str/osal_str.h"

#include "policy/lexleo_assert.h"

stream_status_t stream_destroy(stream_t **s)
{
	if (!s || !*s) {
		return STREAM_STATUS_OK;
	}

	stream_t *tmp = *s;

	LEXLEO_ASSERT(
		   tmp->vtbl->close
		&& tmp->mem
		&& tmp->mem->free
	);

	stream_status_t st = STREAM_STATUS_OK;

	if (tmp->backend) {
		st = tmp->vtbl->close(tmp->backend);
	}

	tmp->mem->free(tmp);
	*s = NULL;

	return st;
}

static const stream_branch_t *stream_registry_find(
	const stream_registry_t *reg,
	stream_key_t key
) {
	if (
		   !reg
		|| !reg->entries
		|| !reg->count
		|| !key
	) {
		return NULL;
	}

	for (size_t i = 0; i < reg->count; i++) {
		const stream_branch_t *e = &reg->entries[i];
		if (e->key && osal_strcmp(e->key, key) == 0) {
			return e;
		}
	}
	return NULL;
}

static stream_status_t stream_registry_create(
	const stream_registry_t *reg,
	stream_key_t key,
	const void *args,
	stream_t **out_stream
) {
	if (!out_stream) {
		return STREAM_STATUS_INVALID;
	}
	*out_stream = NULL;

	const stream_branch_t *e = stream_registry_find(reg, key);
	if (!e || !e->ctor) return STREAM_STATUS_NO_BACKEND;

	return e->ctor(e->ud, args, out_stream);
}

stream_status_t stream_factory_create_stream(
	const stream_factory_t *f,
	stream_key_t key,
	const void *args,
	stream_t **out )
{
	if (!out || !f || !key || *key == '\0')
		return STREAM_STATUS_INVALID;

	if (!stream_registry_find(&f->reg, key)) {
		return STREAM_STATUS_NOT_FOUND;
	}

	stream_t *tmp = NULL;
	stream_status_t st =
		stream_registry_create(
			&f->reg,
			key,
			args,
			&tmp
		);

	if (st == STREAM_STATUS_OK) *out = tmp;

	return st;
}
