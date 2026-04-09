// src/frontend/input_provider/include/input_provider_legacy_types.h

#ifndef LEXLEO_INPUT_PROVIDER_TYPES_H
#define LEXLEO_INPUT_PROVIDER_TYPES_H

#include <stddef.h>

struct input_provider;

typedef size_t (*input_provider_legacy_read_fn_t)(
	struct input_provider *p,
	void *buf,
	size_t n
);

#endif //LEXLEO_INPUT_PROVIDER_TYPES_H