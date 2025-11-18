// src/frontend/input_provider/tests/test_input_provider.c

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#include "internal/input_provider_internal.h"
#include "fake_memory.h"
#include "internal/stream_internal.h"
#include "internal/dynamic_buffer_stream_internal.h"



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


// dummies

static yyscan_t DUMMY_SCANNER = (yyscan_t) 0xDEADBEEF;
static yyscan_t DUMMY_SCANNER_2 = (yyscan_t)0xFEEDBEEF;
static input_provider DUMMY_INPUT_PROVIDER = {
	.mode = INPUT_PROVIDER_MODE_UNINITIALIZED,
	.borrowed_stream = NULL,
	.chunks_stream = NULL,
	.lexer_scanner = NULL
};
static input_provider *DUMMY_INPUT_PROVIDER_P = &DUMMY_INPUT_PROVIDER;
static stream *DUMMY_BORROWED_STREAM = (stream *)0xBAADF00D;
static char *DUMMY_STRING = "dummy";
static size_t DUMMY_SIZE_T = 19;


// fakes

#define FAKABLE_MALLOC(n) (get_current_malloc()(n))
#define FAKABLE_FREE(p) (get_current_free()(p))
#define FAKABLE_STRDUP(s) (get_current_string_duplicate()(s))



//-----------------------------------------------------------------------------
// GENERAL HELPERS
//-----------------------------------------------------------------------------

typedef struct test_input_provider_deep_snapshot {
	input_provider_mode mode;
	stream *borrowed_stream;
	stream *chunks_stream;
	char *buf_address;
	char *buf_content_p;
	size_t cap;
	size_t len;
	size_t read_pos;
	bool autoclose;
	yyscan_t lexer_scanner;
} test_input_provider_deep_snapshot;

static test_input_provider_deep_snapshot input_provider_deep_snapshot(const input_provider *p) {
	assert_non_null(p);
	test_input_provider_deep_snapshot ret = {
		.mode = p->mode,
		.borrowed_stream = p->borrowed_stream,
		.chunks_stream = p->chunks_stream,
		.buf_address = (p->chunks_stream) ? ((dynamic_buffer_stream_ctx*)stream_get_ctx(p->chunks_stream))->buf : NULL,
		.cap = (p->chunks_stream) ? ((dynamic_buffer_stream_ctx*)stream_get_ctx(p->chunks_stream))->cap : 0,
		.len = (p->chunks_stream) ? ((dynamic_buffer_stream_ctx*)stream_get_ctx(p->chunks_stream))->len : 0,
		.read_pos = (p->chunks_stream) ? ((dynamic_buffer_stream_ctx*)stream_get_ctx(p->chunks_stream))->read_pos : 0,
		.autoclose = (p->chunks_stream) ? ((dynamic_buffer_stream_ctx*)stream_get_ctx(p->chunks_stream))->autoclose : false,
		.lexer_scanner = p->lexer_scanner };
	if (ret.chunks_stream) {
		ret.buf_content_p = INPUT_PROVIDER_MALLOC(sizeof(char) * ((dynamic_buffer_stream_ctx*)stream_get_ctx(p->chunks_stream))->cap);
		assert_non_null(ret.buf_content_p);
		memcpy(ret.buf_content_p, ((dynamic_buffer_stream_ctx*)stream_get_ctx(p->chunks_stream))->buf, ((dynamic_buffer_stream_ctx*)stream_get_ctx(p->chunks_stream))->cap);
	} else {
		ret.buf_content_p = NULL;
	}
	return ret;
}

static void input_provider_deep_snapshot_destroy(test_input_provider_deep_snapshot *s) {
	if (!s) return;
	if (s->buf_content_p) {
		INPUT_PROVIDER_FREE(s->buf_content_p);
		s->buf_content_p = NULL;
	}
}

static void assert_same_input_provider_as_snapshot(const input_provider *p, const test_input_provider_deep_snapshot *s) {
	assert_non_null(p);
	assert_non_null(s);
	assert_int_equal(s->mode, p->mode);
	assert_ptr_equal(s->borrowed_stream, p->borrowed_stream);
	assert_ptr_equal(s->chunks_stream, p->chunks_stream);
	if (p->chunks_stream) {
		dynamic_buffer_stream_ctx *dbs = ((dynamic_buffer_stream_ctx*)stream_get_ctx(p->chunks_stream));
		assert_ptr_equal(s->buf_address, dbs->buf);
		assert_int_equal(s->cap, dbs->cap);
		assert_int_equal(s->len, dbs->len);
		assert_int_equal(s->read_pos, dbs->read_pos);
		assert_int_equal(s->autoclose, dbs->autoclose);
		assert_memory_equal(s->buf_content_p, dbs->buf, s->cap);
	}
	assert_ptr_equal(s->lexer_scanner, p->lexer_scanner);
}



//-----------------------------------------------------------------------------
// GENERAL FIXTURES
//-----------------------------------------------------------------------------


static int fake_memory_setup(void **state) {
	(void)state;

	// fake
	set_allocators(fake_malloc, fake_free);
	set_reallocator(fake_realloc);
	fake_memory_reset();

	return 0;
}

static int fake_memory_teardown(void **state) {
	(void)state;
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
	assert_true(fake_memory_no_leak());
	set_allocators(NULL, NULL);
	set_reallocator(NULL);
	fake_memory_reset();
	return 0;
}



//-----------------------------------------------------------------------------
// TESTS
// input_provider *input_provider_create(void);
// void input_provider_destroy(input_provider *p);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
  - input_provider *input_provider_create(void);
  - void input_provider_destroy(input_provider *p);
  - int stream_close(stream *s);


//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


/*
  - dummy:
    - arg p
  - fake:
    - malloc, free, realloc
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


// fake_memory_setup
// fake_memory_teardown



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - oom
// Expected:
//  - returns NULL
//  - No leaks, no invalid or double frees
//  - input_provider_destroy(NULL) is safe (no crash)
static void create_returns_null_when_oom(void **state) {
	(void)state;
	fake_memory_fail_on_all_call();

	input_provider *ret = input_provider_create();
	fake_memory_fail_on_calls(0, NULL);

	assert_null(ret);

	// Cleanup phase
	// Even though ret == NULL, destroy() must handle it gracefully.
	input_provider_destroy(ret);
}

// Given:
//  - no oom
// Expected:
//  - Default state:
//    - ret != NULL
//    - ret->mode == INPUT_PROVIDER_MODE_UNINITIALIZED
//    - ret->borrowed_stream == NULL
//    - ret->chunks_stream == NULL
//    - ret->lexer_scanner == NULL
//  - No leaks, no invalid or double frees
//  - input_provider_destroy(ret) properly frees all owned resources
static void create_success_when_no_oom(void **state) {
	(void)state;
	input_provider *ret = input_provider_create();

	assert_non_null(ret);
	assert_int_equal(ret->mode, INPUT_PROVIDER_MODE_UNINITIALIZED);
	assert_null(ret->borrowed_stream);
	assert_null(ret->chunks_stream);
	assert_null(ret->lexer_scanner);

	// test infrastructure cleanup
	input_provider_destroy(ret);
}



//-----------------------------------------------------------------------------
// TESTS
// bool input_provider_bind_to_scanner(
//     input_provider *p,
//     yyscan_t scanner );
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


//  - bool input_provider_bind_to_scanner(
//       input_provider *p,
//       yyscan_t scanner );
//  - input_provider *input_provider_create(void);
//  - void input_provider_destroy(input_provider *p);
//  - int stream_close(stream *s);
//  - arg p when scanner is not NULL


//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


/*
  - dummies:
    - arg p when scanner is NULL
    - arg scanner
  - fake:
    - malloc, free, realloc
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


// fake_memory_setup
// fake_memory_teardown



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - p == NULL
// Expected:
//  - returns false
static void bind_to_scanner_returns_false_when_p_null(void **state) {
  (void)state;
  bool ret = input_provider_bind_to_scanner(NULL, DUMMY_SCANNER);
  assert_false(ret);
}

// Given:
//  - p != NULL
//  - scanner == NULL
// Expected:
//  - returns false
static void bind_to_scanner_returns_false_when_scanner_null(void **state) {
  (void)state;
  bool ret = input_provider_bind_to_scanner(DUMMY_INPUT_PROVIDER_P, NULL);
  assert_false(ret);
}

// Given:
//  - p != NULL
//  - scanner != NULL
//  - p->lexer_scanner != NULL
// Expected:
//  - returns false
static void bind_to_scanner_returns_false_when_already_bound(void **state) {
	(void)state;
	DUMMY_INPUT_PROVIDER_P->lexer_scanner = DUMMY_SCANNER_2;
	bool ret = input_provider_bind_to_scanner(DUMMY_INPUT_PROVIDER_P, DUMMY_SCANNER);
	assert_false(ret);

	DUMMY_INPUT_PROVIDER_P->lexer_scanner = NULL;
}

// Given:
//  - p != NULL
//  - scanner != NULL
//  - p->lexer_scanner == NULL
// Expected:
//  - returns true
//  - p->lexer_scanner == scanner
//  - does not touch other fields (mode, borrowed_stream, chunks_stream, lexer_buffer_state)
static void bind_to_scanner_succeeds_when_first_bind(void **state) {
	(void)state;
	input_provider *p = input_provider_create();
	assert_non_null(p);
	assert_null(p->lexer_scanner);
	test_input_provider_deep_snapshot snapshot = input_provider_deep_snapshot(p);

	bool ret = input_provider_bind_to_scanner(p, DUMMY_SCANNER);

	assert_true(ret);
	assert_ptr_equal(p->lexer_scanner, DUMMY_SCANNER);
	assert_int_equal(p->mode, snapshot.mode);
	assert_ptr_equal(p->borrowed_stream, snapshot.borrowed_stream);
	assert_ptr_equal(p->chunks_stream, snapshot.chunks_stream);

	input_provider_destroy(p);
	input_provider_deep_snapshot_destroy(&snapshot);
}



//-----------------------------------------------------------------------------
// TESTS
// bool input_provider_set_mode_chunks(input_provider *p);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


//  - bool input_provider_set_mode_chunks(input_provider *p);
//  - input_provider *input_provider_create(void);
//  - void input_provider_destroy(input_provider *p);
//  - arg p
//  - int stream_close(stream *s);
//  - stream *dynamic_buffer_stream_create(void);
//  - int stream_close(stream *s);
//  - void *stream_get_ctx(stream *s);



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


/*
  - fake:
    - malloc, free, realloc
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


// fake_memory_setup
// fake_memory_teardown



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - p == NULL
// Expected:
//  - returns false
static void set_mode_chunks_returns_false_when_p_null(void **state) {
	(void)state;
	bool ret = input_provider_set_mode_chunks(NULL);
	assert_false(ret);
}

// Given:
//  - p != NULL
//  - p->mode == INPUT_PROVIDER_MODE_UNINITIALIZED
//  - p->chunks_stream == NULL
// Expected:
//  - returns true
//  - p->mode == INPUT_PROVIDER_MODE_CHUNKS
//  - denoting dbs := (dynamic_buffer_stream_ctx *) stream_get_ctx(p->chunks_stream):
//    - dbs != NULL
//    - dbs->buf != NULL
//    - dbs->len = 0
//    - dbs->read_pos = 0
//    - dbs->autoclose = true
//  - does not touch other fields
static void set_mode_chunks_success_when_p_not_null_and_chunks_stream_null(void **state) {
	(void)state;
	input_provider *p = input_provider_create();
	assert_non_null(p);
	assert_int_equal(p->mode, INPUT_PROVIDER_MODE_UNINITIALIZED);
	assert_null(p->chunks_stream);
	test_input_provider_deep_snapshot snapshot = input_provider_deep_snapshot(p);

	bool ret = input_provider_set_mode_chunks(p);

	assert_true(ret);
	assert_int_equal(p->mode, INPUT_PROVIDER_MODE_CHUNKS);
	assert_non_null(p->chunks_stream);
	dynamic_buffer_stream_ctx *dbs = (dynamic_buffer_stream_ctx *) stream_get_ctx(p->chunks_stream);
	assert_non_null(dbs);
	assert_non_null(dbs->buf);
	assert_int_equal(dbs->len, 0);
	assert_int_equal(dbs->read_pos, 0);
	assert_true(dbs->autoclose);
	assert_ptr_equal(p->borrowed_stream, snapshot.borrowed_stream);
	assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);

	input_provider_destroy(p);
	input_provider_deep_snapshot_destroy(&snapshot);
}

// Given:
//  - p != NULL
//  - p->mode == INPUT_PROVIDER_MODE_CHUNKS
//  - p->chunks_stream != NULL
// Expected:
//  - returns true
//  - p->mode == INPUT_PROVIDER_MODE_CHUNKS
//  - denoting dbs := (dynamic_buffer_stream_ctx *) stream_get_ctx(p->chunks_stream):
//    - dbs != NULL
//    - dbs->buf != NULL
//    - dbs->len = 0
//    - dbs->read_pos = 0
//    - dbs->autoclose = true
//  - does not touch other fields
static void set_mode_chunks_success_when_p_not_null_and_chunks_stream_not_null(void **state) {
	(void)state;
	input_provider *p = input_provider_create();
	assert_non_null(p);
	assert_true(input_provider_set_mode_chunks(p));
	assert_int_equal(p->mode, INPUT_PROVIDER_MODE_CHUNKS);
	assert_non_null(p->chunks_stream);
	test_input_provider_deep_snapshot snapshot = input_provider_deep_snapshot(p);

	bool ret = input_provider_set_mode_chunks(p);

	assert_true(ret);
	assert_int_equal(p->mode, INPUT_PROVIDER_MODE_CHUNKS);
	assert_non_null(p->chunks_stream);
	dynamic_buffer_stream_ctx *dbs = (dynamic_buffer_stream_ctx *) stream_get_ctx(p->chunks_stream);
	assert_non_null(dbs);
	assert_non_null(dbs->buf);
	assert_int_equal(dbs->len, 0);
	assert_int_equal(dbs->read_pos, 0);
	assert_true(dbs->autoclose);
	assert_ptr_equal(p->borrowed_stream, snapshot.borrowed_stream);
	assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);

	input_provider_destroy(p);
	input_provider_deep_snapshot_destroy(&snapshot);
}



//-----------------------------------------------------------------------------
// TESTS
// bool input_provider_set_mode_borrowed_stream(input_provider *p, stream *s);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


//  - bool input_provider_set_mode_borrowed_stream(input_provider *p, stream *s);
//  - input_provider *input_provider_create(void);
//  - void input_provider_destroy(input_provider *p);
//  - int stream_close(stream *s);
//  - arg p


//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


/*
  - dummy:
    - arg s
  - fake:
    - malloc, free, realloc
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


// fake_memory_setup
// fake_memory_teardown



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - p == NULL
// Expected:
//  - returns false
static void set_mode_borrowed_stream_returns_false_when_p_null(void **state) {
  (void)state;
  bool ret = input_provider_set_mode_borrowed_stream(NULL, DUMMY_BORROWED_STREAM);
  assert_false(ret);
}

// Given:
//  - p != NULL
//  - s == NULL
// Expected:
//  - returns false
//  - p is unchanged
static void set_mode_borrowed_stream_returns_false_and_no_side_effect_when_s_null(void **state) {
	(void)state;
	input_provider *p = input_provider_create();
	assert_non_null(p);
	test_input_provider_deep_snapshot snapshot = input_provider_deep_snapshot(p);

	bool ret = input_provider_set_mode_borrowed_stream(p, NULL);

	assert_false(ret);
	assert_same_input_provider_as_snapshot(p, &snapshot);

	input_provider_destroy(p);
	input_provider_deep_snapshot_destroy(&snapshot);
}

// Given:
//  - p != NULL
//  - s != NULL
// Expected:
//  - returns true
//  - p->mode == INPUT_PROVIDER_MODE_BORROWED_STREAM
//  - p->borrowed_stream == s
//  - does not touch other fields
static void set_mode_borrowed_stream_success_when_p_not_null_and_s_not_null(void **state) {
	(void)state;
	input_provider *p = input_provider_create();
	assert_non_null(p);
	assert_null(p->borrowed_stream);
	assert_null(p->chunks_stream);
	assert_int_equal(p->mode, INPUT_PROVIDER_MODE_UNINITIALIZED);
	test_input_provider_deep_snapshot snapshot = input_provider_deep_snapshot(p);

	bool ret = input_provider_set_mode_borrowed_stream(p, DUMMY_BORROWED_STREAM);

	assert_true(ret);
	assert_int_equal(p->mode, INPUT_PROVIDER_MODE_BORROWED_STREAM);
	assert_ptr_equal(p->borrowed_stream, DUMMY_BORROWED_STREAM);
	assert_null(p->chunks_stream);
	assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);

	input_provider_destroy(p);
	input_provider_deep_snapshot_destroy(&snapshot);
}



//-----------------------------------------------------------------------------
// TESTS
// bool input_provider_append(
//     input_provider *p,
//     const char *bytes,
//     size_t len );
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


//  - bool input_provider_append(
//       input_provider *p,
//       const char *bytes,
//       size_t len );
//  - input_provider *input_provider_create(void);
//  - void input_provider_destroy(input_provider *p);
//  - arg p
//  - bytes, len when p != NULL
//  - stream *dynamic_buffer_stream_create(void);
//  - void *stream_get_ctx(stream *s);
//  - size_t stream_write(stream *s, const void *buf, size_t n);
//  - static size_t dynamic_buffer_stream_write(void *ctx, const void* buf, size_t n);
//  - int stream_close(stream *s);
//  - from the standard library:
//    - memcpy, strlen



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


/*
  - dummy:
    - bytes, len when p == NULL
  - fake:
    - malloc, free, realloc
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


// fake_memory_setup
// fake_memory_teardown



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - p == NULL
// Expected:
//  - returns false
static void append_returns_false_when_p_null(void **state) {
	(void)state;
	bool ret = input_provider_append(NULL, DUMMY_STRING, DUMMY_SIZE_T);
	assert_false(ret);
}

// Given:
//  - p != NULL
//  - bytes == NULL
// Expected:
//  - returns false
//  - p is unchanged
static void append_returns_false_when_bytes_null(void **state) {
	(void)state;
	input_provider *p = input_provider_create();
	assert_non_null(p);
	test_input_provider_deep_snapshot snapshot = input_provider_deep_snapshot(p);

	bool ret = input_provider_append(p, NULL, DUMMY_SIZE_T);

	assert_false(ret);
	assert_same_input_provider_as_snapshot(p, &snapshot);

	input_provider_destroy(p);
	input_provider_deep_snapshot_destroy(&snapshot);
}

// Given:
//  - p is valid
//  - p->mode == INPUT_PROVIDER_MODE_CHUNKS
//  - at bytes, len bytes or more are allocated
//  - no '\0' in the first len bytes from bytes
//  - denoting dbs := (dynamic_buffer_stream_ctx *) stream_get_ctx(p->chunks_stream):
//    - dbs->len + len <= dbs->cap
// Expected:
//  - returns true
//  - from dbs->buf + dbs->len to dbs->buf + dbs->len + len,
//    is written a copy of the first len bytes of bytes
//  - dbs->len += len
//  - does not touch other fields
static void append_success_when_p_is_valid_and_no_cap_exceeding(void **state) {
	(void)state;
	input_provider *p = input_provider_create();
	assert_non_null(p);
	assert_true(input_provider_set_mode_chunks(p));
	assert_non_null(p->chunks_stream);
	dynamic_buffer_stream_ctx *dbs = (dynamic_buffer_stream_ctx *)stream_get_ctx(p->chunks_stream);
	assert_non_null(dbs);
	assert_non_null(dbs->buf);
	assert_int_equal(dbs->len, 0);
	assert_int_equal(dbs->read_pos, 0);
	assert_true(dbs->autoclose);
	const char *cst_str = "I'm written in the dynamic buffer of an input_provider!";
	size_t len = strlen(cst_str);
	char bytes[DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY] = {0};
	memcpy(bytes, "I'm written in the dynamic buffer of an input_provider!", len);
	assert_true(dbs->len + len <= dbs->cap);
	test_input_provider_deep_snapshot snapshot = input_provider_deep_snapshot(p);

	bool ret = input_provider_append(p, bytes, len);

	assert_true(ret);
	assert_memory_equal(dbs->buf + snapshot.len, bytes, len);
	assert_int_equal(dbs->len, snapshot.len + len);
	assert_int_equal(p->mode, snapshot.mode);
	assert_ptr_equal(p->borrowed_stream, snapshot.borrowed_stream);
	assert_ptr_equal(dbs->buf, snapshot.buf_address);
	assert_int_equal(dbs->cap, snapshot.cap);
	assert_int_equal(dbs->read_pos, snapshot.read_pos);
	assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);

	input_provider_destroy(p);
	input_provider_deep_snapshot_destroy(&snapshot);
}



//-----------------------------------------------------------------------------
// TESTS
// size_t input_provider_read(input_provider *p, void *buf, size_t n);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


//  - size_t input_provider_read(input_provider *p, void *buf, size_t n);
//  - input_provider *input_provider_create(void);
//  - void input_provider_destroy(input_provider *p);
//  - arg p
//  - args buf, n when every other args are not NULL
//  - stream *dynamic_buffer_stream_create(void);
//  - void *stream_get_ctx(stream *s);
//  - size_t stream_write(stream *s, const void *buf, size_t n);
//  - static size_t dynamic_buffer_stream_write(void *ctx, const void* buf, size_t n);
//  - int stream_close(stream *s);
//  - size_t stream_read(stream *s, void *b, size_t n);
//  - static size_t dynamic_buffer_stream_read(void *ctx, void* buf, size_t n)
//  - from the standard library:
//  - bool input_provider_append(input_provider *p, const char *bytes, size_t len);
//  - from the standard library:
//    - memcpy, strlen



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


/*
  - dummy:
    - args buf, n when another arg is NULL
  - fake:
	- malloc, free, realloc
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


// fake_memory_setup
// fake_memory_teardown



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - p == NULL
// Expected:
//  - returns 0
static void read_returns_0_when_p_null(void **state) {
	(void)state;
	size_t ret = input_provider_read(NULL, DUMMY_STRING, DUMMY_SIZE_T);
	assert_int_equal(ret, 0);
}

// Given:
//  - p != NULL
//  - buf == NULL
// Expected:
//  - returns 0
//  - p is unchanged
static void read_returns_0_when_buf_null(void **state) {
	(void)state;
	input_provider *p = input_provider_create();
	assert_non_null(p);
	test_input_provider_deep_snapshot snapshot = input_provider_deep_snapshot(p);

	size_t ret = input_provider_read(p, NULL, DUMMY_SIZE_T);

	assert_int_equal(ret, 0);
	assert_same_input_provider_as_snapshot(p, &snapshot);

	input_provider_destroy(p);
	input_provider_deep_snapshot_destroy(&snapshot);
}

// Given:
//  - p != NULL
//  - n == 0
// Expected:
//  - returns 0
//  - p is unchanged
static void read_returns_0_when_n_0(void **state) {
	(void)state;
	input_provider *p = input_provider_create();
	assert_non_null(p);
	test_input_provider_deep_snapshot snapshot = input_provider_deep_snapshot(p);

	size_t ret = input_provider_read(p, DUMMY_STRING, 0);

	assert_int_equal(ret, 0);
	assert_same_input_provider_as_snapshot(p, &snapshot);

	input_provider_destroy(p);
	input_provider_deep_snapshot_destroy(&snapshot);
}

// Given:
//  - p is valid
//  - p->mode == INPUT_PROVIDER_MODE_CHUNKS
//  - denoting dbs := (dynamic_buffer_stream_ctx *) stream_get_ctx(p->chunks_stream):
//    - dbs->cap = DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY == 256
//    - dbs->len == 7
//    - dbs->read_pos == 3
//    - at dbs->buf is written "abcdefg"
//  - n == 10
// Expected:
//  - returns 4
//  - dbs->read_pos == 7
//  - at buf is written "defg"
//  - does not touch other fields of p
static void read_success_when_no_cap_exceeding(void **state) {
	(void)state;
	input_provider *p = input_provider_create();
	assert_non_null(p);
	assert_true(input_provider_set_mode_chunks(p));
	assert_non_null(p->chunks_stream);
	assert_true(input_provider_append(p, "abcdefg", 7));
	dynamic_buffer_stream_ctx *dbs = (dynamic_buffer_stream_ctx *)stream_get_ctx(p->chunks_stream);
	assert_non_null(dbs);
	assert_non_null(dbs->buf);
	dbs->read_pos = 3;
	assert_int_equal(dbs->len, 7);
	char buf[DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY] = {0};
	test_input_provider_deep_snapshot snapshot = input_provider_deep_snapshot(p);

	size_t ret = input_provider_read(p, buf, 10);

	assert_int_equal(ret, 4);
	assert_int_equal(dbs->read_pos, snapshot.read_pos + 4);
	assert_memory_equal(buf, "defg", 4);
	assert_int_equal(p->mode, snapshot.mode);
	assert_ptr_equal(p->borrowed_stream, snapshot.borrowed_stream);
	assert_ptr_equal(p->chunks_stream, snapshot.chunks_stream);
	assert_ptr_equal(dbs->buf, snapshot.buf_address);
	assert_int_equal(dbs->cap, snapshot.cap);
	assert_int_equal(dbs->len, snapshot.len);
	assert_int_equal(dbs->autoclose, snapshot.autoclose);
	assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);
	assert_memory_equal(dbs->buf, "abcdefg", 7);

	input_provider_destroy(p);
	input_provider_deep_snapshot_destroy(&snapshot);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
  const struct CMUnitTest create_tests[] = {
    cmocka_unit_test_setup_teardown(
        create_returns_null_when_oom,
        fake_memory_setup, fake_memory_teardown),
    cmocka_unit_test_setup_teardown(
        create_success_when_no_oom,
        fake_memory_setup, fake_memory_teardown),
  };

	const struct CMUnitTest bind_to_scanner_tests[] = {
		cmocka_unit_test_setup_teardown(
			bind_to_scanner_returns_false_when_p_null,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			bind_to_scanner_returns_false_when_scanner_null,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			bind_to_scanner_returns_false_when_already_bound,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			bind_to_scanner_succeeds_when_first_bind,
			fake_memory_setup, fake_memory_teardown),
	};

	const struct CMUnitTest set_mode_chunks_tests[] = {
		cmocka_unit_test_setup_teardown(
			set_mode_chunks_returns_false_when_p_null,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			set_mode_chunks_success_when_p_not_null_and_chunks_stream_null,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			set_mode_chunks_success_when_p_not_null_and_chunks_stream_not_null,
			fake_memory_setup, fake_memory_teardown),
	};

  const struct CMUnitTest set_mode_borrowed_stream_tests[] = {
    cmocka_unit_test_setup_teardown(
        set_mode_borrowed_stream_returns_false_when_p_null,
        fake_memory_setup, fake_memory_teardown),
  	cmocka_unit_test_setup_teardown(
		  set_mode_borrowed_stream_returns_false_and_no_side_effect_when_s_null,
		  fake_memory_setup, fake_memory_teardown),
  	cmocka_unit_test_setup_teardown(
		  set_mode_borrowed_stream_success_when_p_not_null_and_s_not_null,
		  fake_memory_setup, fake_memory_teardown),
  };

  const struct CMUnitTest append_tests[] = {
    cmocka_unit_test_setup_teardown(
        append_returns_false_when_p_null,
        fake_memory_setup, fake_memory_teardown),
    cmocka_unit_test_setup_teardown(
        append_returns_false_when_bytes_null,
        fake_memory_setup, fake_memory_teardown),
    cmocka_unit_test_setup_teardown(
        append_success_when_p_is_valid_and_no_cap_exceeding,
        fake_memory_setup, fake_memory_teardown),
  };

	const struct CMUnitTest read_tests[] = {
		cmocka_unit_test_setup_teardown(
			read_returns_0_when_p_null,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			read_returns_0_when_buf_null,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			read_returns_0_when_n_0,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			read_success_when_no_cap_exceeding,
			fake_memory_setup, fake_memory_teardown),
	  };

	int failed = 0;
	failed += cmocka_run_group_tests(create_tests, NULL, NULL);
	failed += cmocka_run_group_tests(bind_to_scanner_tests, NULL, NULL);
	failed += cmocka_run_group_tests(set_mode_chunks_tests, NULL, NULL);
	failed += cmocka_run_group_tests(append_tests, NULL, NULL);
	failed += cmocka_run_group_tests(read_tests, NULL, NULL);

	return failed;
}
