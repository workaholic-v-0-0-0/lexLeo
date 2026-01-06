// src/adapters/stream_legacy/stdio_stream_legacy/tests/test_stdio_stream_legacy.c

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

#include "internal/stdio_stream_legacy_internal.h"
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
// TESTS
// stream_legacy *stdio_stream_legacy_from_stdin(void);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
  - stream *stdio_stream_from_stdin(void);

other elements of the isolated unit:
  - stream *stream_create(const stream_vtable *vtbl, void *ctx);
  - int stream_close(stream *s);
  - from the standard library:
	- fread, fwrite, fflush, fclose, fopen, fputs, fseek, freopen, fclose
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
static void from_stdin_ret_null_when_oom(void **state) {
	(void)state;
	fake_memory_fail_on_all_call();
	stream_legacy *ret = stdio_stream_legacy_from_stdin();
	fake_memory_fail_on_calls(0, NULL);
	assert_null(ret);
}

// Given:
//  - no oom
// Expected:
//  - ret != NULL
static void from_stdin_ret_not_null_when_no_oom(void **state) {
	(void)state;
	stream_legacy *ret = stdio_stream_legacy_from_stdin();
	assert_non_null(ret);

	stream_legacy_close(ret);
}

// Given:
//  - stdin redirected to a file containing "hello"
// Expected:
//  - stdio_stream_read reads "hello"
static void from_stdin_reads_from_stdin(void **state) {
	(void)state;
	FILE *f = fopen("tmp_input.txt", "w+");
	assert_non_null(f);
	fputs("hello", f);
	fflush(f);
	fseek(f, 0, SEEK_SET);
	FILE *stdin_reopened = freopen("tmp_input.txt", "r", stdin);
	assert_non_null(stdin_reopened);
	char buf[16] = {0};

	stream_legacy *s = stdio_stream_legacy_from_stdin();
	assert_non_null(s);
	size_t n = stream_legacy_read(s, buf, 16);

	assert_int_equal(n, 5);
	assert_memory_equal(buf, "hello", 5);

	stream_legacy_close(s);
	fclose(f);
	remove("tmp_input.txt");
}



//-----------------------------------------------------------------------------
// TESTS
// stream *stdio_stream_to_stdout(void);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
  - stream *stdio_stream_to_stdout(void);

other elements of the isolated unit:
  - stream *stream_create(const stream_vtable *vtbl, void *ctx);
  - int stream_close(stream *s);
  - stream *stdio_stream_from_fp_for_tests(FILE *fp, bool autoclose);
  - from the standard library:
	- fread, fwrite, fflush, fclose, fopen, fputs, fseek, freopen, fclose
*/



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


/*
  - fake:
	- malloc
    - free (except for fake_stdout in the last test)
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
static void to_stdout_ret_null_when_oom(void **state) {
	(void)state;
	fake_memory_fail_on_all_call();
	stream_legacy *ret = stdio_stream_legacy_to_stdout();
	fake_memory_fail_on_calls(0, NULL);
	assert_null(ret);
}

// Given:
//  - no oom
// Expected:
//  - ret != NULL
static void to_stdout_ret_not_null_when_no_oom(void **state) {
	(void)state;
	stream_legacy *ret = stdio_stream_legacy_to_stdout();
	assert_non_null(ret);

	stream_legacy_close(ret);
}

// Given:
//  - a memory-backed FILE* opened by osal_open_memstream
// Expected:
//  - stdio_stream_write writes "hello" into that memory buffer
static void to_stdout_write_to_stdout(void **state) {
	(void)state;
	char *mem_as_file = NULL;
	size_t len = 0;
	FILE *fake_stdout = osal_open_memstream(&mem_as_file, &len);
	assert_non_null(fake_stdout);
	const char *buf = "hello";

	stream_legacy *s = stdio_stream_legacy_from_fp_for_tests(fake_stdout, true);
	assert_non_null(s);
	size_t n = stream_legacy_write(s, buf, 5);

	stream_legacy_close(s);

	assert_int_equal(n, 5);
	assert_non_null(mem_as_file);
	assert_memory_equal(mem_as_file, "hello", 5);

	free(mem_as_file);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
	const struct CMUnitTest from_stdin_tests[] = {
		cmocka_unit_test_setup_teardown(
			from_stdin_ret_null_when_oom,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			from_stdin_ret_not_null_when_no_oom,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			from_stdin_reads_from_stdin,
			fake_memory_setup, fake_memory_teardown),
	};

	const struct CMUnitTest from_stdout_tests[] = {
		cmocka_unit_test_setup_teardown(
			to_stdout_ret_null_when_oom,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			to_stdout_ret_not_null_when_no_oom,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			to_stdout_write_to_stdout,
			fake_memory_setup, fake_memory_teardown),
	};

	int failed = 0;
	failed += cmocka_run_group_tests(from_stdin_tests, NULL, NULL);
	failed += cmocka_run_group_tests(from_stdout_tests, NULL, NULL);

	return failed;
}
