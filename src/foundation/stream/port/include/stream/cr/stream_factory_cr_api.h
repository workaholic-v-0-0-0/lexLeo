/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/stream/port/include/cr/
 * stream_factory_cr_api.h
 */

#ifndef LEXLEO_STREAM_FACTORY_CR_API_H
#define LEXLEO_STREAM_FACTORY_CR_API_H

#include "stream/adapters/stream_install.h"
#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_key_type.h"
#include "stream/cr/stream_cr_api.h"
#include "osal/mem/osal_mem_ops.h"

#include <stddef.h>

typedef struct stream_factory_t stream_factory_t;

typedef struct stream_factory_cfg_t {
	size_t fact_cap;
} stream_factory_cfg_t;

stream_status_t stream_create_factory(
	stream_factory_t **out,
	const stream_factory_cfg_t *cfg,
	const stream_env_t *env );

void stream_destroy_factory(stream_factory_t **fact);

stream_status_t stream_factory_add_adapter(
	stream_factory_t *fact,
	const stream_adapter_desc_t *desc );

stream_status_t stream_factory_create_stream(
	const stream_factory_t *f,
	stream_key_t key,
	const void *args,
	stream_t **out);

stream_factory_cfg_t stream_default_factory_cfg(void);

#endif //LEXLEO_STREAM_FACTORY_CR_API_H

/* draft
{
	const stream_branch_t *stream_branch = f->stream_registry_find(f,key);
	stream_status_t st =
		stream_branch_t->ctor(
			ud?,
			args,
			out );
}
for doubling, just change the stream_branch_t with key "an_adapter_stream"
with a stream_branch_t key "an_adapter_stream" and a ctor
which make a fake_adapter_stream

no no need, for doubling, just buid a fake factory so with the
field registry  initialized with a const stream_registry_t * which
map key "an_adapter_stream" onto a ctor
which make a fake_adapter_stream
*/