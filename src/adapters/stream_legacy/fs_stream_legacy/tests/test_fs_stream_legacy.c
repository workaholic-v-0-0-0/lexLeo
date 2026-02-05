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

#include "internal/fs_stream_legacy_internal.h"
#include "internal/stream_legacy_internal.h"
#include "lexleo/test/fake_memory.h"
#include "osal.h"



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


//dummies

static const char *DUMMY_FILE_PATH = "dummy/file/path";


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
// stream_legacy *fs_stream_from_file(const char *path);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


/*
  - stream *fs_stream_from_file(const char *path);

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
static void from_file_ret_null_when_oom(void **state) {
	(void)state;
	fake_memory_fail_on_all_call();
	stream_legacy *ret = fs_stream_legacy_from_file(DUMMY_FILE_PATH);
	fake_memory_fail_on_calls(0, NULL);
	assert_null(ret);
}

// Given:
//  - no oom
//  - path is not a path of an existing file
// Expected:
//  - ret == NULL
static void from_file_ret_null_when_file_does_not_exist(void **state) {
	(void)state;
	stream_legacy *ret = fs_stream_legacy_from_file(DUMMY_FILE_PATH);
	assert_null(ret);
}

// Given:
//  - no oom
//  - path is a path of an existing file
// Expected:
//  - ret != NULL
static void from_file_ret_not_null_when_file_exists(void **state) {
	(void)state;
	FILE *fp = fopen("an_existing_file.txt", "w");
	assert_non_null(fp);
	fclose(fp);

	stream_legacy *ret = fs_stream_legacy_from_file("an_existing_file.txt");
	assert_non_null(ret);

	stream_legacy_close(ret);
	int rm = remove("an_existing_file.txt");
	assert_int_equal(rm, 0);
}

// Given:
//  - no oom
//  - path is a path of an existing file
// Expected:
//  - fs_stream_read reads the 16 first characters of the file
static void from_file_reads_from_file(void **state) {
	(void)state;
	FILE *fp = fopen("an_existing_file.txt", "w");
	assert_non_null(fp);
	fwrite("a string written in an an_existing_file.txt", 16, 1, fp);
	fclose(fp);
	char buf[16] = {0};

	stream_legacy *ret = fs_stream_legacy_from_file("an_existing_file.txt");
	assert_non_null(ret);
	size_t n = stream_legacy_read(ret, buf, 16);

	stream_legacy_close(ret);

	assert_int_equal(n, 16);
	assert_memory_equal(buf, "a string written", 16);
	int rm = remove("an_existing_file.txt");
	assert_int_equal(rm, 0);
}



//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------


int main(void) {
	const struct CMUnitTest from_file_tests[] = {
		cmocka_unit_test_setup_teardown(
			from_file_ret_null_when_oom,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			from_file_ret_null_when_file_does_not_exist,
			fake_memory_setup, fake_memory_teardown),
		cmocka_unit_test_setup_teardown(
			from_file_ret_not_null_when_file_exists,
			fake_memory_setup, fake_memory_teardown),
#if OSAL_HAVE_OPEN_MEMSTREAM || OSAL_HAVE_FOPENCOOKIE || defined(__APPLE__)
		cmocka_unit_test_setup_teardown(
			from_file_reads_from_file,
			fake_memory_setup, fake_memory_teardown),
#endif
	};

	int failed = 0;
	failed += cmocka_run_group_tests(from_file_tests, NULL, NULL);

	return failed;
}
