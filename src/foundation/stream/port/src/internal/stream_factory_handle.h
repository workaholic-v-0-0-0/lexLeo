// src/foundation/stream/port/src/internal/stream_factory_handle.h

#ifndef LEXLEO_STREAM_FACTORY_HANDLE_H
#define LEXLEO_STREAM_FACTORY_HANDLE_H

#include "stream/owners/stream_key_type.h"
#include "stream/adapters/stream_constructor.h"
#include "mem/osal_mem_ops.h"

#include <stddef.h>

typedef struct stream_branch_t {
	stream_key_t key;
	stream_ctor_fn_t ctor;
	void *ud;
} stream_branch_t;

typedef struct stream_registry_t {
    stream_branch_t *entries;
    size_t count;
    size_t cap;
} stream_registry_t;

struct stream_factory_t {
    stream_registry_t reg;
    const osal_mem_ops_t *mem;
};

#endif //LEXLEO_STREAM_FACTORY_HANDLE_H
