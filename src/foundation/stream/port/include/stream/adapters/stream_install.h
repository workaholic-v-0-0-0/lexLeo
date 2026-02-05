// src/foundation/stream/port/include/stream/adapters/stream_install.h

#ifndef LEXLEO_STREAM_INSTALL_H
#define LEXLEO_STREAM_INSTALL_H

#include "stream/adapters/stream_key_type.h"
#include "stream/borrowers/stream_types.h"
#include "osal/mem/osal_mem_ops.h"

typedef void (*ud_dtor_fn_t)(void *ud, const osal_mem_ops_t *mem);

// constructor that an adapter must implement
typedef stream_status_t (*stream_ctor_fn_t)(
	void *ud,
	const void *args,
	stream_t **out);

// adapter must provide to CR a constructor of
// this this type, where mod_key_t param is to be
// provide by CR
typedef struct stream_adapter_desc_t {
	stream_key_t key;
	stream_ctor_fn_t ctor;
	void *ud;
	ud_dtor_fn_t ud_dtor;
} stream_adapter_desc_t;

#endif //LEXLEO_STREAM_INSTALL_H
