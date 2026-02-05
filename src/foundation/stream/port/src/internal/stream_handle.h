// src/foundation/stream/port/src/internal/stream_handle.h

#ifndef LEXLEO_STREAM_HANDLE_H
#define LEXLEO_STREAM_HANDLE_H

#include "stream/adapters/stream_adapters_api.h"
#include "osal/mem/osal_mem_ops.h"

struct stream_t {
	stream_vtbl_t vtbl;
	void *backend;
	const osal_mem_ops_t *mem;
};

#endif //LEXLEO_STREAM_HANDLE_H
