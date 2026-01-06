// src/frontend/input_provider/include/input_provider_ops.h

#ifndef LEXLEO_INPUT_PROVIDER_OPS_H
#define LEXLEO_INPUT_PROVIDER_OPS_H

#include "input_provider_legacy_types.h"

#include <stdbool.h>

struct stream;

typedef struct input_provider_legacy_ops_t {
	bool (*set_mode_chunks)(struct input_provider *p);
	bool (*set_mode_borrowed_stream)(struct input_provider *p, struct stream *s);
	bool (*append)(struct input_provider *p, const char *bytes, size_t len);
	input_provider_legacy_read_fn_t read;
} input_provider_legacy_ops_t;

const input_provider_legacy_ops_t *input_provider_legacy_default_ops(void);

#endif //LEXLEO_INPUT_PROVIDER_OPS_H
