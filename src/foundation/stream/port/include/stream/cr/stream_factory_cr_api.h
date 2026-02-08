/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/port/include/cr/
 * stream_factory_cr_api.h
 */

#ifndef LEXLEO_STREAM_FACTORY_CR_API_H
#define LEXLEO_STREAM_FACTORY_CR_API_H

#include "stream/adapters/stream_constructor.h"
#include "stream/borrowers/stream_types.h"
#include "stream/owners/stream_key_type.h"
#include "stream/cr/stream_cr_api.h"
#include "mem/osal_mem_ops.h"

typedef struct stream_factory_t stream_factory_t;

stream_status_t stream_create_empty_factory(
	stream_factory_t **out,
	size_t capacity,
	const stream_ctx_t *ctx );

stream_status_t stream_destroy_factory(stream_factory_t **fact);

stream_status_t stream_factory_add_adapter(
    stream_factory_t *fact,
    stream_key_t key,
    stream_ctor_fn_t ctor,
    void *ud);

#endif //LEXLEO_STREAM_FACTORY_CR_API_H
