// src/adapters/stream_legacy/dynamic_buffer_stream_legacy/tests/test_dynamic_buffer_stream_legacy.c

#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "internal/dynamic_buffer_stream_legacy_internal.h"
#include "internal/stream_legacy_internal.h"
#include "fake_memory.h"
#include "osal.h"



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


// fakes

#define FAKABLE_MALLOC(n) (get_current_malloc()(n))
#define FAKABLE_FREE(p) (get_current_free()(p))
#define FAKABLE_STRDUP(s) (get_current_string_duplicate()(s))



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
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
	assert_true(fake_memory_no_leak());
	set_allocators(NULL, NULL);
	set_reallocator(NULL);
	fake_memory_reset();
	return 0;
}



//-----------------------------------------------------------------------------
// TESTS (through stream_legacy_create)
// stream_legacy *dynamic_buffer_stream_legacy_create(void);
//-----------------------------------------------------------------------------


/*
  - stream_legacy *dynamic_buffer_stream_legacy_create(void);

other elements of the isolated unit:
  - stream_legacy *stream_create(const stream_vtable *vtbl, void *ctx);
  - static int dynamic_buffer_stream_close(void *ctx);
  - int stream_close(stream *s);
  - void *stream_get_ctx(stream *s);
*/



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
//  - oom
// Expected:
//  - ret == NULL
static void create_ret_null_when_oom(void **state) {
	(void)state;
	fake_memory_fail_on_all_call();
	stream_legacy *ret = dynamic_buffer_stream_legacy_create();
	fake_memory_fail_on_calls(0, NULL);
	assert_null(ret);
}

// Given:
//  - no oom
// Expected:
//  - ret != NULL
static void create_ret_not_null_when_no_oom(void **state) {
	(void)state;
	stream_legacy *ret = dynamic_buffer_stream_legacy_create();
	assert_non_null(ret);

	int status = stream_legacy_close(ret);
	assert_int_equal(status, 0);
}

// Given:
//  - no oom
// Expected:
//  - ret != NULL
//  - denoting ctx the related dynamic_buffer_stream_ctx:
//    - ctx != NULL
//    - ctx->buf != NULL
//    - ctx->cap == DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY
//    - ctx->len == 0
//    - ctx->read_pos == 0
//    - ctx->autoclose == true
static void create_initialize_ctx_when_no_oom(void **state) {
	(void)state;
	stream_legacy *ret = dynamic_buffer_stream_legacy_create();

	assert_non_null(ret);
	dynamic_buffer_stream_legacy_ctx *ctx = (dynamic_buffer_stream_legacy_ctx*)stream_legacy_get_ctx(ret);
	assert_non_null(ctx);
	assert_non_null(ctx->buf);
	assert_int_equal(ctx->cap, DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY);
	assert_int_equal(ctx->len, 0);
	assert_int_equal(ctx->read_pos, 0);
	assert_true(ctx->autoclose);

	assert_int_equal(stream_legacy_close(ret), 0);
}



//-----------------------------------------------------------------------------
// TESTS (through stream_write)
// static size_t dynamic_buffer_stream_write(void *ctx, const void* buf, size_t n);
//-----------------------------------------------------------------------------


/*
  - static size_t dynamic_buffer_stream_write(void *ctx, const void* buf, size_t n);

other elements of the isolated unit:
  - size_t stream_write(stream *s, const void *buf, size_t n);
  - stream *dynamic_buffer_stream_create(void);
  - static int dynamic_buffer_stream_close(void *ctx);
  - int stream_close(stream *s);
  - void *stream_get_ctx(stream *s);
  - from the standard library:
	- strlen, memcpy
*/



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
//  - no oom
//  - a dynamic buffer stream dbs is created and initialized successfully
//  - during ACT stage, the string "I will be written in a dynamic buffer." is written through dbs via stream_write
// Expected:
//  - ret == strlen("I will be written in a dynamic buffer.")
//  - denoting ctx the related dynamic_buffer_stream_ctx:
//    - ctx->cap == DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY
//    - ctx->len == strlen("I will be written in a dynamic buffer.")
//    - ctx->read_pos == 0
//    - ctx->autoclose == true
static void write_success_when_default_cap_is_sufficient(void **state) {
	(void)state;

	// ARRANGE
	stream_legacy *dbs = dynamic_buffer_stream_legacy_create();
	assert_non_null(dbs);
	dynamic_buffer_stream_legacy_ctx *ctx = (dynamic_buffer_stream_legacy_ctx*)stream_legacy_get_ctx(dbs);
	assert_non_null(ctx);
	assert_non_null(ctx->buf);
	char *str_to_be_written = "I will be written in a dynamic buffer.";

	// ACT
	size_t ret = stream_legacy_write(dbs, str_to_be_written, strlen(str_to_be_written));

	// ASSERT
	assert_int_equal(ret, strlen(str_to_be_written));
	assert_int_equal(ctx->cap, DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY);
	assert_int_equal(ctx->len, strlen(str_to_be_written));
	assert_int_equal(ctx->read_pos, 0);
	assert_true(ctx->autoclose);
	assert_memory_equal(ctx->buf, str_to_be_written, strlen(str_to_be_written));

	// TEST INFRASTRUCTURE CLEANUP
	assert_int_equal(stream_legacy_close(dbs), 0);
}

// Given:
//  - no oom
//  - a dynamic buffer stream dbs is created and initialized successfully
//  - during ACT stage, a string exceeding DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY
//    but not exceeding 2 * DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY is written
//    through dbs via stream_write
// Expected:
//  - denoting long_str the written string:
//    - ret == strlen(long_str)
//  - denoting ctx the related dynamic_buffer_stream_ctx:
//    - ctx->cap == 2 * DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY
//    - ctx->len == strlen(long_str)
//    - ctx->read_pos == 0
//    - ctx->autoclose == true
static void write_success_when_default_cap_is_not_sufficient(void **state) {
	(void)state;

	// ARRANGE
	stream_legacy *dbs = dynamic_buffer_stream_legacy_create();
	assert_non_null(dbs);
	dynamic_buffer_stream_legacy_ctx *ctx = (dynamic_buffer_stream_legacy_ctx*)stream_legacy_get_ctx(dbs);
	assert_non_null(ctx);
	assert_non_null(ctx->buf);
	const size_t len = DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY + 1;
	char str_to_be_written[DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY + 1] = {0};
	for (size_t i = 0; i <= DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY; i++)
		str_to_be_written[i] = 'a';

	// ACT
	size_t ret = stream_legacy_write(dbs, str_to_be_written, len);

	// ASSERT
	assert_int_equal(ret, len);
	assert_int_equal(ctx->cap, 2 * DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY);
	assert_int_equal(ctx->len, len);
	assert_int_equal(ctx->read_pos, 0);
	assert_true(ctx->autoclose);
	assert_memory_equal(ctx->buf, str_to_be_written, len);

	// TEST INFRASTRUCTURE CLEANUP
	assert_int_equal(stream_legacy_close(dbs), 0);
}



//-----------------------------------------------------------------------------
// TESTS (through stream_read)
// static size_t dynamic_buffer_stream_read(void *ctx, void* buf, size_t n);
//-----------------------------------------------------------------------------


/*
  - static size_t dynamic_buffer_stream_read(void *ctx, void* buf, size_t n);

other elements of the isolated unit:
  - size_t stream_read (stream *s, void *buf, size_t n);
  - stream *dynamic_buffer_stream_create(void);
  - static size_t dynamic_buffer_stream_write(void *ctx, const void* buf, size_t n);
  - size_t stream_write(stream *s, const void *buf, size_t n);
  - static int dynamic_buffer_stream_close(void *ctx);
  - int stream_close(stream *s);
  - void *stream_get_ctx(stream *s);
  - from the standard library:
	- memcpy, strlen
*/



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


// Arrange:
//  - no oom
//  - a dynamic buffer stream dbs is created and initialized successfully
// Act:
//  - try to read one byte from dbs
// Assert:
//  - ret == 0
//  - denoting ctx the related dynamic_buffer_stream_ctx:
//    - ctx is unchanged
static void read_ret_0_and_no_side_effect_when_buf_empty(void **state) {
	(void)state;

	// ARRANGE
	stream_legacy *dbs = dynamic_buffer_stream_legacy_create();
	assert_non_null(dbs);
	dynamic_buffer_stream_legacy_ctx *ctx = (dynamic_buffer_stream_legacy_ctx*)stream_legacy_get_ctx(dbs);
	assert_non_null(ctx);
	assert_non_null(ctx->buf);
	assert_int_equal(ctx->cap, DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY);
	assert_int_equal(ctx->len, 0);
	assert_int_equal(ctx->read_pos, 0);
	assert_true(ctx->autoclose);
	char read_char = '\0';

	// ACT
	size_t ret = stream_legacy_read(dbs, &read_char, 1);

	// ASSERT
	assert_int_equal(ret, 0);
	assert_non_null(ctx);
	assert_non_null(ctx->buf);
	assert_int_equal(ctx->cap, DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY);
	assert_int_equal(ctx->len, 0);
	assert_int_equal(ctx->read_pos, 0);
	assert_true(ctx->autoclose);

	// TEST INFRASTRUCTURE CLEANUP
	assert_int_equal(stream_legacy_close(dbs), 0);
}

// Arrange:
//  - no oom
//  - a dynamic buffer stream dbs is created and initialized successfully
//  - the string "I will be written in a dynamic buffer and then read." is written through dbs via stream_write
// Act:
//  - try to read the string from dbs
// Assert:
//  - ret == strlen("I will be written in a dynamic buffer and then read.") + 1
//  - denoting
//        ctx the related dynamic_buffer_stream_ctx,
//        n := strlen("I will be written in a dynamic buffer and then read.") + 1
//    - ctx->read_pos == n
//    - ctx->len == n
//    - ctx->cap == DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY
//    - ctx->autoclose == true
static void read_read_a_string_when_it_is_written_in_buf(void **state) {
	(void)state;

	// ARRANGE
	stream_legacy *dbs = dynamic_buffer_stream_legacy_create();
	assert_non_null(dbs);
	dynamic_buffer_stream_legacy_ctx *ctx = (dynamic_buffer_stream_legacy_ctx*)stream_legacy_get_ctx(dbs);
	assert_non_null(ctx);
	assert_non_null(ctx->buf);
	assert_int_equal(ctx->cap, DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY);
	assert_true(ctx->autoclose);
	char *str_to_be_written = "I will be written in a dynamic buffer and then read.";
	size_t n = strlen(str_to_be_written) + 1;
	assert_int_equal(stream_legacy_write(dbs, str_to_be_written, n), n);
	assert_int_equal(ctx->len, n);
	assert_int_equal(ctx->read_pos, 0);
	char read_buf[DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY] = {0};

	// ACT
	size_t ret = stream_legacy_read(dbs, read_buf, n);

	// ASSERT
	assert_int_equal(ret, n);
	assert_non_null(ctx);
	assert_non_null(ctx->buf);
	assert_int_equal(ctx->cap, DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY);
	assert_int_equal(ctx->len, n);
	assert_int_equal(ctx->read_pos, n);
	assert_true(ctx->autoclose);

	// TEST INFRASTRUCTURE CLEANUP
	assert_int_equal(stream_legacy_close(dbs), 0);
}

// Arrange:
//  - no oom
//  - a dynamic buffer stream dbs is created and initialized successfully
//  - the string "I will be written in a dynamic buffer and then read." is written through dbs via stream_writen
//  - the 10th first bytes are already read
// Act:
//  - try to read 1000 bytes
// Assert:
//  - ret == strlen("I will be written in a dynamic buffer and then read.") + 1 - 10
//  - denoting
//        ctx the related dynamic_buffer_stream_ctx,
//        n := strlen("I will be written in a dynamic buffer and then read.") + 1
//    - ctx->read_pos == n
//    - ctx->len == n
//    - ctx->cap == DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY
//    - ctx->autoclose == true
static void read_try_to_1000_bytes_when_10_are_already_read(void **state) {
	(void)state;

	// ARRANGE
	stream_legacy *dbs = dynamic_buffer_stream_legacy_create();
	assert_non_null(dbs);
	dynamic_buffer_stream_legacy_ctx *ctx = (dynamic_buffer_stream_legacy_ctx*)stream_legacy_get_ctx(dbs);
	assert_non_null(ctx);
	assert_non_null(ctx->buf);
	assert_int_equal(ctx->cap, DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY);
	assert_true(ctx->autoclose);
	char *str_to_be_written = "I will be written in a dynamic buffer and then read.";
	char read_buf[DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY] = {0};
	size_t n = strlen(str_to_be_written) + 1;
	assert_int_equal(stream_legacy_write(dbs, str_to_be_written, n), n);
	assert_int_equal(stream_legacy_read(dbs, read_buf, 10), 10);
	assert_int_equal(ctx->len, n);
	assert_int_equal(ctx->read_pos, 10);

	// ACT
	size_t ret = stream_legacy_read(dbs, read_buf, 1000);

	// ASSERT
	assert_int_equal(ret, strlen("I will be written in a dynamic buffer and then read.") + 1 - 10);
	assert_non_null(ctx);
	assert_non_null(ctx->buf);
	assert_int_equal(ctx->cap, DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY);
	assert_int_equal(ctx->len, n);
	assert_int_equal(ctx->read_pos, n);
	assert_true(ctx->autoclose);

	// TEST INFRASTRUCTURE CLEANUP
	assert_int_equal(stream_legacy_close(dbs), 0);
}

// Arrange:
//  - no oom
//  - a dynamic buffer stream dbs is created and initialized successfully
//  - the string "I will be written in a dynamic buffer and then read." is written through dbs via stream_writen
//  - all the string is already read
// Act:
//  - try to read 1000 bytes
// Assert:
//  - ret == 0
//  - denoting
//        ctx the related dynamic_buffer_stream_ctx,
//        n := strlen("I will be written in a dynamic buffer and then read.") + 1
//    - ctx->read_pos == n
//    - ctx->len == n
//    - ctx->cap == DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY
//    - ctx->autoclose == true
static void read_try_to_1000_bytes_when_all_is_already_read(void **state) {
	(void)state;

	// ARRANGE
	stream_legacy *dbs = dynamic_buffer_stream_legacy_create();
	assert_non_null(dbs);
	dynamic_buffer_stream_legacy_ctx *ctx = (dynamic_buffer_stream_legacy_ctx*)stream_legacy_get_ctx(dbs);
	assert_non_null(ctx);
	assert_non_null(ctx->buf);
	assert_int_equal(ctx->cap, DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY);
	assert_true(ctx->autoclose);
	char *str_to_be_written = "I will be written in a dynamic buffer and then read.";
	char read_buf[DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY] = {0};
	size_t n = strlen(str_to_be_written) + 1;
	assert_int_equal(stream_legacy_write(dbs, str_to_be_written, n), n);
	assert_int_equal(stream_legacy_read(dbs, read_buf, n), n);
	assert_int_equal(ctx->len, n);
	assert_int_equal(ctx->read_pos, n);

	// ACT
	size_t ret = stream_legacy_read(dbs, read_buf, 1000);

	// ASSERT
	assert_int_equal(ret, 0);
	assert_non_null(ctx);
	assert_non_null(ctx->buf);
	assert_int_equal(ctx->cap, DYNAMIC_BUFFER_STREAM_DEFAULT_CAPACITY);
	assert_int_equal(ctx->len, n);
	assert_int_equal(ctx->read_pos, n);
	assert_true(ctx->autoclose);

	// TEST INFRASTRUCTURE CLEANUP
	assert_int_equal(stream_legacy_close(dbs), 0);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
	const struct CMUnitTest create_tests[] = {
		cmocka_unit_test_setup_teardown(
			create_ret_null_when_oom,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			create_ret_not_null_when_no_oom,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			create_initialize_ctx_when_no_oom,
			fake_memory_setup, fake_memory_teardown),
	};

	const struct CMUnitTest write_tests[] = {
		cmocka_unit_test_setup_teardown(
			write_success_when_default_cap_is_sufficient,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			write_success_when_default_cap_is_not_sufficient,
			fake_memory_setup, fake_memory_teardown),
	};

	const struct CMUnitTest read_tests[] = {
		cmocka_unit_test_setup_teardown(
			read_ret_0_and_no_side_effect_when_buf_empty,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			read_read_a_string_when_it_is_written_in_buf,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			read_try_to_1000_bytes_when_10_are_already_read,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			read_try_to_1000_bytes_when_all_is_already_read,
			fake_memory_setup, fake_memory_teardown),
	};

	int failed = 0;
	failed += cmocka_run_group_tests(create_tests, NULL, NULL);
	failed += cmocka_run_group_tests(write_tests, NULL, NULL);
	failed += cmocka_run_group_tests(read_tests, NULL, NULL);

	return failed;
}
