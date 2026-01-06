// src/frontend/src/input_provider_legacy.c

#include "stream_legacy.h"
#include "dynamic_buffer_stream_legacy.h"

#include <stddef.h>
#include <string.h>

typedef struct YYSTYPE YYSTYPE;

#include "internal/input_provider_legacy_internal.h"


input_provider *input_provider_legacy_create() {
	input_provider *ret = INPUT_PROVIDER_MALLOC(sizeof(input_provider));
	if (!ret) return NULL;
	memset(ret, 0, sizeof(input_provider));
	ret->mode = INPUT_PROVIDER_MODE_UNINITIALIZED;
	return ret;
}

void input_provider_legacy_destroy(input_provider *p) {
	if (!p) return;
	if (p->chunks_stream) stream_legacy_close(p->chunks_stream);
	INPUT_PROVIDER_FREE(p);
}

bool input_provider_legacy_bind_to_scanner(input_provider *p, yyscan_t scanner) {
	if (!p || !scanner || p->lexer_scanner) return false;
	p->lexer_scanner = scanner;
	return true;
}

bool input_provider_legacy_set_mode_chunks(input_provider *p) {
	if (!p) return false;
	if (p->chunks_stream) {
		stream_legacy_close(p->chunks_stream);
		p->chunks_stream = NULL;
	}
	p->chunks_stream = dynamic_buffer_stream_legacy_create();
	if (!p->chunks_stream) {
		p->mode = INPUT_PROVIDER_MODE_UNINITIALIZED;
		return false;
	}
	p->mode = INPUT_PROVIDER_MODE_CHUNKS;
	return true;
}

bool input_provider_legacy_set_mode_borrowed_stream(input_provider *p, stream_legacy *s) {
	if (!p || !s) return false;
	if (p->chunks_stream) {
		stream_legacy_close(p->chunks_stream);
		p->chunks_stream = NULL;
	}
	p->borrowed_stream = s;
	p->mode = INPUT_PROVIDER_MODE_BORROWED_STREAM;
	return true;
}

bool input_provider_legacy_append(input_provider *p, const char *bytes, size_t len) {
	if (
			   !p
			|| p->mode != INPUT_PROVIDER_MODE_CHUNKS
			|| !bytes
			|| !p->chunks_stream )
		return false;
	return (len == stream_legacy_write(p->chunks_stream, bytes, len));
}

// be aware that it is not atomic!
bool input_provider_legacy_append_line(
		input_provider *p,
		const char *bytes,
		size_t len ) {
	if (!input_provider_legacy_append(p, bytes, len)) return false;
	return input_provider_legacy_append(p, "\n", 1);
}

bool input_provider_legacy_append_string_as_line(
		input_provider *p,
		const char *s) {
	if (!input_provider_legacy_append(p, s, strlen(s))) return false;
	return input_provider_legacy_append(p, "\n", 1);
}

static stream_legacy *input_provider_legacy_get_active_stream(const input_provider *p) {
	if (!p) return NULL;
	switch (p->mode) {
		case INPUT_PROVIDER_MODE_BORROWED_STREAM:
			return p->borrowed_stream;
		case INPUT_PROVIDER_MODE_CHUNKS:
			return p->chunks_stream;
		default:
			return NULL;
	}
}

size_t input_provider_legacy_read(input_provider *p, void *buf, size_t n) {
	if (!p || !buf || n == 0) return 0;
	stream_legacy *s = input_provider_legacy_get_active_stream(p);
	if (!s) return 0;
	return stream_legacy_read(s, buf, n);
}

bool input_provider_legacy_reset_chunks(input_provider *p) {
	return input_provider_legacy_set_mode_chunks(p);
}