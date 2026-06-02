/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_adapter.c
 * @ingroup stream_internal_group
 * @brief Adapter-side stream construction implementation.
 *
 * @details
 * This file implements the adapter-facing helpers used to create a `stream_t`
 * handle from borrowed dependencies, then attach the adapter-owned backend
 * during default initialization.
 */

#include "stream/adapters/stream_adapters_api.h"

#include "internal/stream_handle.h"

#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_assert.h"

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
	const stream_env_t *env )
{
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

	*out = NULL;

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
