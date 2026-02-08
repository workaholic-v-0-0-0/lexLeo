// src/foundation/stream/include/owners/stream_factory.h

#ifndef LEXLEO_STREAM_FACTORY_H
#define LEXLEO_STREAM_FACTORY_H

#include "stream/borrowers/stream_types.h"
#include "stream/owners/stream_key_type.h"

typedef struct stream_factory_t stream_factory_t;

stream_status_t stream_factory_create_stream(
	const stream_factory_t *f,
	stream_key_t key,
	const void *args,
	stream_t **out);

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

// non stream_factory_destroy_stream cause close do it

#endif //LEXLEO_STREAM_FACTORY_H
