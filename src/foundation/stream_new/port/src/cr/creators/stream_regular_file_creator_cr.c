/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_regular_file_creator_cr.c
 * @ingroup stream_internal_group
 * @brief Regular-file stream creator lifetime implementation.
 *
 * @details
 * Implements the CR-facing allocation and destruction operations for
 * `stream_regular_file_creator_t` objects.
 */

#include "stream/cr/creators/stream_regular_file_creator_cr_api.h"

#include "internal/stream_creator_handle.h"
#include "internal/creators/stream_regular_file_creator_type.h"

#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_assert.h"

stream_factory_status_t stream_create_regular_file_creator(
	stream_regular_file_creator_t **out,
	stream_factory_t *factory,
	stream_adapter_id_t adapter_id,
	const osal_mem_ops_t *mem
) {
	LEXLEO_ASSERT(
		   out
		&& factory
		&& adapter_id
		&& *adapter_id != '\0'
		&& mem
		&& mem->calloc
	);

	stream_regular_file_creator_t *tmp = mem->calloc(1, sizeof(*tmp));
	if (!tmp) {
		return STREAM_FACTORY_STATUS_OOM;
	}

	tmp->base.factory = factory;
	tmp->base.adapter_id = adapter_id;
	tmp->base.mem = mem;

	*out = tmp;
	return STREAM_FACTORY_STATUS_OK;
}

void stream_destroy_regular_file_creator(
	stream_regular_file_creator_t **creator
) {
	if (!creator || !*creator) {
		return;
	}

	LEXLEO_ASSERT((*creator)->base.mem && (*creator)->base.mem->free);

	(*creator)->base.mem->free(*creator);
	*creator = NULL;
}
