// src/foundation/stream/port/include/stream/adapters/stream_install.h

// <here> finish fixing include/* ; see message
// next : src/, go back to fs_stream, unit test, document thoroughly

#ifndef LEXLEO_STREAM_INSTALL_H
#define LEXLEO_STREAM_INSTALL_H

#include "stream/adapters/stream_key_type.h"
#include "stream/borrowers/stream_types.h"
#include "mem/osal_mem_ops.h"

typedef void (*ud_dtor_fn_t)(void *ud, const osal_mem_ops_t *mem);

// constructor that an adapter must implement
typedef stream_status_t (*stream_ctor_fn_t)(
	void *ud,
	const void *args,
	stream_t **out);

// descriptor that an adapter must provide
// so that CR can register it into factory
typedef struct stream_adapter_desc_t {
	stream_key_t key;
	stream_ctor_fn_t ctor;
	void *ud;
	ud_dtor_fn_t ud_dtor;
} stream_adapter_desc_t;

#endif //LEXLEO_STREAM_INSTALL_H
