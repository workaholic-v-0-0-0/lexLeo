// src/foundation/stream/port/include/adapters/stream_env.h

#ifndef LEXLEO_STREAM_ENV_H
#define LEXLEO_STREAM_ENV_H

#include "mem/osal_mem_ops.h"

typedef struct stream_env_t {
	const osal_mem_ops_t *mem;
} stream_env_t;

#endif //LEXLEO_STREAM_ENV_H