// src/foundation/input_provider_legacy/include/input_provider_legacy.h

#ifndef LEXLEO_INPUT_PROVIDER_H
#define LEXLEO_INPUT_PROVIDER_H

#include "input_provider_legacy_types.h"

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

struct stream_legacy;

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

typedef enum {
	INPUT_PROVIDER_MODE_UNINITIALIZED = 0,
    INPUT_PROVIDER_MODE_CHUNKS,
    INPUT_PROVIDER_MODE_BORROWED_STREAM
} input_provider_legacy_mode;

typedef struct input_provider input_provider;



//-----------------------------------------------------------------------------
// Creation, destruction and scanner binding
//-----------------------------------------------------------------------------


input_provider *input_provider_legacy_create(void);

void input_provider_legacy_destroy(input_provider *p);

// one bind per session ; scanner is borrowed
bool input_provider_legacy_bind_to_scanner(input_provider *p, yyscan_t scanner);



//-----------------------------------------------------------------------------
// mode switching
//-----------------------------------------------------------------------------


bool input_provider_legacy_set_mode_chunks(input_provider *p);

// s is borrowed from client code
bool input_provider_legacy_set_mode_borrowed_stream(input_provider *p, struct stream_legacy *s);



//-----------------------------------------------------------------------------
// CHUNKS mode
//-----------------------------------------------------------------------------


// Appends raw bytes to the internal dynamic buffer.
// `bytes` is borrowed from client code
bool input_provider_legacy_append(
		input_provider *p,
		const char *bytes, // bytes is borrowed from client code
		size_t len );

// Same but adds '\n' at the end
bool input_provider_legacy_append_line(
		input_provider *p,
		const char *bytes,
		size_t len );

bool input_provider_legacy_append_string_as_line(
		input_provider *p,
		const char *s );

bool input_provider_legacy_reset_chunks(input_provider *p);


#endif //LEXLEO_INPUT_PROVIDER_H
