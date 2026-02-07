// src/foundation/stream/port/src/internal/stream_internal.h

#ifndef LEXLEO_STREAM_INTERNAL_H
#define LEXLEO_STREAM_INTERNAL_H

#include "internal/stream_ctx.h"

struct stream_t {
	stream_vtbl_t vtbl;
	void *backend;
	const osal_mem_ops_t *mem;
};

#endif //LEXLEO_STREAM_INTERNAL_H
