/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_file_creator_cr.c
 * @ingroup stream_internal_group
 * @brief File stream creator lifetime implementation.
 *
 * @details
 * This file implements the CR-facing allocation and destruction services for
 * `stream_file_creator_t` objects.
 */

#include "stream/owners/creators/stream_file_creator.h"

#include "internal/creators/stream_file_creator_handle.h"

#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_assert.h"

stream_status_t stream_create_file_creator(
	stream_file_creator_t **out,
	stream_factory_t *factory,
	stream_key_t key,
	const osal_mem_ops_t *mem
) {
	LEXLEO_ASSERT(
		   out
		&& factory
		&& key && *key != '\0'
		&& mem
		&& mem->calloc
	);

	stream_file_creator_t *tmp = mem->calloc(1, sizeof(*tmp));
	if (!tmp) {
		return STREAM_STATUS_OOM;
	}

	tmp->base.factory = factory;
	tmp->base.key = key;
	tmp->base.mem = mem;

	*out = tmp;

	return STREAM_STATUS_OK;
}

void stream_destroy_file_creator(stream_file_creator_t **creator)
{
	if (!creator || !*creator) {
		return;
	}

	stream_file_creator_t *tmp = *creator;
	*creator = NULL;

	LEXLEO_ASSERT(
		   tmp->base.mem
		&& tmp->base.mem->free
	);

	tmp->base.mem->free(tmp);
}
