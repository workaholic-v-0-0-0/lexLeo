// src/integration/flex/include_private/lexleo_flex_test_api.h

// WARNING: Private test API. Do not include from production targets.

#ifndef LEXLEO_LEXLEO_FLEX_TEST_API_H
#define LEXLEO_LEXLEO_FLEX_TEST_API_H

#include "lexleo_flex_backend.h"

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// VIRTUAL TABLE
// ----------------------------------------------------------------------------

typedef
lexleo_flex_status_t
(*lexleo_flex_backend_create_fn_t) (
		lexleo_flex_backend **out,
		struct input_provider *provider );

typedef void (*lexleo_flex_backend_destroy_fn_t) (lexleo_flex_backend *b);

typedef
lexleo_flex_next_rc_t
(*lexleo_flex_backend_next_fn_t) (
		lexleo_flex_backend *b,
		lexleo_token_t *out );

typedef struct lexleo_flex_backend_ops_t {
	lexleo_flex_backend_create_fn_t create_fn;
	lexleo_flex_backend_destroy_fn_t destroy_fn;
	lexleo_flex_backend_next_fn_t next_fn;
} lexleo_flex_backend_ops_t;

lexleo_flex_status_t lexleo_flex_backend_create_with_ops(
	lexleo_flex_backend **out,
	struct input_provider *provider,
	const lexleo_flex_backend_ops_t *ops );

// ----------------------------------------------------------------------------
// DEFAULT VIRTUAL TABLE
// ----------------------------------------------------------------------------

const lexleo_flex_backend_ops_t *lexleo_flex_backend_default_ops(void);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_LEXLEO_FLEX_TEST_API_H
