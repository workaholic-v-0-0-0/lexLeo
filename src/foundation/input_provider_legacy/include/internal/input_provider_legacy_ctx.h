// src/frontend/input_provider/include/internal/input_provider_ctx.h

#ifndef LEXLEO_INPUT_PROVIDER_CTX_H
#define LEXLEO_INPUT_PROVIDER_CTX_H

#include "input_provider_ops.h"

typedef struct input_provider_ctx_t {
	const input_provider_legacy_ops_t *ops;
} input_provider_ctx_t;

#endif //LEXLEO_INPUT_PROVIDER_CTX_H