// src/io/src/input_provider.c

#include <stddef.h>
#include <string.h>

typedef struct YYSTYPE YYSTYPE;
#include "lexer.yy.h"
#include "internal/input_provider_internal.h"


// context

static const lexer_ops_t INPUT_PROVIDER_LEXER_OPS_DEFAULT = {
	.yy_scan_bytes_fn = yy_scan_bytes,
	.yy_delete_buffer_fn = yy_delete_buffer,
	.yyrestart_fn = yyrestart,
	.yy_switch_to_buffer_fn = yy_switch_to_buffer
};

static input_provider_ctx g_input_provider_ctx = {
	.lexer_ops = &INPUT_PROVIDER_LEXER_OPS_DEFAULT,
};


// core

input_provider *input_provider_create() {
	input_provider *ret = INPUT_PROVIDER_MALLOC(sizeof(input_provider));
	if (!ret) return NULL;

	memset(ret, 0, sizeof(input_provider));

	ret->dbuf.buf = INPUT_PROVIDER_MALLOC(sizeof(char) * INPUT_PROVIDER_INITIAL_SIZE_OF_BUFFER);
	if (!ret->dbuf.buf) {
		INPUT_PROVIDER_FREE(ret);
		return NULL;
	}

	ret->mode = INPUT_PROVIDER_MODE_CHUNKS;
	ret->dbuf.cap = INPUT_PROVIDER_INITIAL_SIZE_OF_BUFFER;
	ret->dbuf.len = 0;
	ret->lexer_buffer_state = NULL;

	return ret;
}

void input_provider_destroy(input_provider *p) {
	if (!p) return;
	if (p->lexer_buffer_state && p->lexer_scanner) {
		g_input_provider_ctx.lexer_ops->yy_delete_buffer_fn(p->lexer_buffer_state, p->lexer_scanner);
		p->lexer_buffer_state = NULL;
	}
	if (p->dbuf.buf) INPUT_PROVIDER_FREE(p->dbuf.buf);
	INPUT_PROVIDER_FREE(p);
}

bool input_provider_bind_to_scanner(input_provider *p, yyscan_t scanner) {
	if (!p || !scanner || p->lexer_scanner) return false;
	p->lexer_scanner = scanner;
	return true;
}

bool input_provider_set_mode_chunks(input_provider *p) {
	if (!p) return false;
	p->mode = INPUT_PROVIDER_MODE_CHUNKS;
	return true;
}

bool input_provider_set_mode_file(input_provider *p) {
	if (!p) return false;
	p->mode = INPUT_PROVIDER_MODE_FILE;
	return true;
}

bool input_provider_buffer_reset(input_provider *p) {
	if (!p) return false;
	p->dbuf.len = 0;
	memset(p->dbuf.buf, 0, p->dbuf.cap);
	return true;
}

static bool buffer_reserve(input_provider_dynamic_buffer *dbuf, size_t cap) {
	if (!dbuf) return false;
	if (dbuf->cap >= cap) return true;
	void *new_buf = INPUT_PROVIDER_REALLOC(dbuf->buf, sizeof(char) * cap);
	if (!new_buf) return false;
	dbuf->cap = cap;
	dbuf->buf = new_buf;
	return true;
}

static size_t next_cap(size_t cap) {
	return 1 + 2 * cap;
}

bool input_provider_buffer_append(input_provider *p, const char *bytes, size_t len) {
	if (!p || !bytes) return false;
	size_t new_cap = p->dbuf.cap;
	while (p->dbuf.len + len > new_cap) new_cap = next_cap(new_cap);
	if (!buffer_reserve(&p->dbuf, new_cap)) return false;
	memcpy(p->dbuf.buf + p->dbuf.len, bytes, len);
	p->dbuf.len += len;
	return true;
}

bool input_provider_buffer_append_line(
		input_provider *p,
		const char *bytes,
		size_t len ) {
	if (!input_provider_buffer_append(p, bytes, len)) return false;
	return input_provider_buffer_append(p, "\n", 1);
}

bool input_provider_set_file(input_provider *p, FILE *f) {
	if (!p || !f) return false;
	p->file = f;
	return true;
}

bool input_provider_publish(input_provider *p) {
	if (!p || !p->lexer_scanner) return false;

	switch (p->mode) {
		case INPUT_PROVIDER_MODE_CHUNKS: {
			YY_BUFFER_STATE tmp_lexer_buffer_state = NULL;
			tmp_lexer_buffer_state =
				input_provider_get_yy_scan_bytes_fn()(
					p->dbuf.buf,
					(int) p->dbuf.len,
					p->lexer_scanner );
			if (!tmp_lexer_buffer_state) return false;
			if (p->lexer_buffer_state) {
				input_provider_get_yy_delete_buffer_fn()(
					p->lexer_buffer_state,
					p->lexer_scanner );
				p->lexer_buffer_state = NULL;
			}
			p->lexer_buffer_state = tmp_lexer_buffer_state;
			break;
		}
		case INPUT_PROVIDER_MODE_FILE: {
			if (!p->file) return false;
			input_provider_get_yyrestart_fn()(p->file, p->lexer_scanner);
			break;
		}
		default:
			return false;
	}

	return true;
}


// setters and getters

void input_provider_set_lexer_ops(const lexer_ops_t *overrides) {
	static lexer_ops_t applied;
	applied = *g_input_provider_ctx.lexer_ops;

	if (overrides) {
		if (overrides->yy_scan_bytes_fn)
			applied.yy_scan_bytes_fn = overrides->yy_scan_bytes_fn;
		if (overrides->yy_delete_buffer_fn)
			applied.yy_delete_buffer_fn = overrides->yy_delete_buffer_fn;
		if (overrides->yyrestart_fn)
			applied.yyrestart_fn = overrides->yyrestart_fn;
		if (overrides->yy_switch_to_buffer_fn)
			applied.yy_switch_to_buffer_fn = overrides->yy_switch_to_buffer_fn;
	}
	g_input_provider_ctx.lexer_ops = &applied;
}

void input_provider_reset_lexer_ops(void) {
	g_input_provider_ctx.lexer_ops = &INPUT_PROVIDER_LEXER_OPS_DEFAULT;
}

void input_provider_set_yy_scan_bytes_fn(yy_scan_bytes_fn_t fn) {
	input_provider_set_lexer_ops(&(lexer_ops_t){
		.yy_scan_bytes_fn = fn ? fn : INPUT_PROVIDER_LEXER_OPS_DEFAULT.yy_scan_bytes_fn
	});
}

void input_provider_set_yy_delete_buffer_fn(yy_delete_buffer_fn_t fn) {
	input_provider_set_lexer_ops(&(lexer_ops_t){
		.yy_delete_buffer_fn = fn ? fn : INPUT_PROVIDER_LEXER_OPS_DEFAULT.yy_delete_buffer_fn
	});
}

void input_provider_set_yyrestart_fn(yyrestart_fn_t fn) {
	input_provider_set_lexer_ops(&(lexer_ops_t){
		.yyrestart_fn = fn ? fn : INPUT_PROVIDER_LEXER_OPS_DEFAULT.yyrestart_fn
	});
}

void input_provider_set_yy_switch_to_buffer_fn(yy_switch_to_buffer_fn_t fn) {
	input_provider_set_lexer_ops(&(lexer_ops_t){
		.yy_switch_to_buffer_fn = fn ? fn : INPUT_PROVIDER_LEXER_OPS_DEFAULT.yy_switch_to_buffer_fn
	});
}

yy_scan_bytes_fn_t input_provider_get_yy_scan_bytes_fn(void) {
	return g_input_provider_ctx.lexer_ops->yy_scan_bytes_fn;
}

yy_delete_buffer_fn_t input_provider_get_yy_delete_buffer_fn(void) {
	return g_input_provider_ctx.lexer_ops->yy_delete_buffer_fn;
}

yyrestart_fn_t input_provider_get_yyrestart_fn(void) {
	return g_input_provider_ctx.lexer_ops->yyrestart_fn;
}

yy_switch_to_buffer_fn_t input_provider_get_yy_switch_to_buffer_fn(void) {
	return g_input_provider_ctx.lexer_ops->yy_switch_to_buffer_fn;
}
