// src/integration/flex/include/lexleo_flex_backend.h

#ifndef LEXLEO_FLEX_BACKEND_H
#define LEXLEO_FLEX_BACKEND_H

#include "lexleo_token.h"

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// OPAQUE HANDLE
// ----------------------------------------------------------------------------

typedef struct lexleo_flex_backend lexleo_flex_backend;
struct input_provider;

// ----------------------------------------------------------------------------
// PUBLIC API
// ----------------------------------------------------------------------------

typedef enum {
	LEXLEO_FLEX_STATUS_OK = 0,
	LEXLEO_FLEX_STATUS_ERROR = 1
} lexleo_flex_status_t;

typedef enum {
	LEXLEO_FLEX_NEXT_ERROR = -1,
	LEXLEO_FLEX_NEXT_EOF = 0,
	LEXLEO_FLEX_NEXT_TOKEN = 1
} lexleo_flex_next_rc_t;

lexleo_flex_status_t lexleo_flex_backend_create(
		lexleo_flex_backend **out,
		struct input_provider *provider);

void lexleo_flex_backend_destroy(lexleo_flex_backend *b);

lexleo_flex_next_rc_t lexleo_flex_backend_next(
		lexleo_flex_backend *b,
		lexleo_token_t *out);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_FLEX_BACKEND_H
