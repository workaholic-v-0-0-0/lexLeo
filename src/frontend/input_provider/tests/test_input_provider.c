// src/io/input_provider/tests/test_input_provider.c

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



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


// dummies

static char *DUMMY_STRING = "dummy";
static size_t DUMMY_SIZE_T = 19;
static yyscan_t DUMMY_SCANNER = (yyscan_t) 0xDEADBEEF;
static yyscan_t DUMMY_SCANNER_2 = (yyscan_t)0xFEEDBEEF;
static char DUMMY_BUF[INPUT_PROVIDER_INITIAL_SIZE_OF_BUFFER];
static input_provider DUMMY_INPUT_PROVIDER = {
    .mode = INPUT_PROVIDER_MODE_CHUNKS,
    .dbuf = {
		.buf = DUMMY_BUF,
		.cap = INPUT_PROVIDER_INITIAL_SIZE_OF_BUFFER,
		.len = 0 },
    .file = NULL,
    .lexer_scanner = NULL,
    .lexer_buffer_state = NULL, };
static input_provider *DUMMY_INPUT_PROVIDER_P = &DUMMY_INPUT_PROVIDER;
static FILE *DUMMY_FILE_P = (FILE *)0xBAADF00D;
static YY_BUFFER_STATE DUMMY_YY_BUFFER_STATE = (YY_BUFFER_STATE)0xDEADC0DE;
static YY_BUFFER_STATE DUMMY_YY_BUFFER_STATE_2 = (YY_BUFFER_STATE)0xCAFEBABE;


// spies

bool yy_scan_bytes_is_called = false;
static const char *spy_scan_bytes_arg_bytes = NULL;
static int spy_scan_bytes_arg_len = 0;
static yyscan_t spy_scan_bytes_arg_scanner = NULL;
static YY_BUFFER_STATE scan_bytes_ret = NULL;
static YY_BUFFER_STATE spy_yy_scan_bytes(const char *bytes, int len, yyscan_t scanner) {
	yy_scan_bytes_is_called = true;
	spy_scan_bytes_arg_bytes = bytes;
	spy_scan_bytes_arg_len = len;
	spy_scan_bytes_arg_scanner = scanner;
	return scan_bytes_ret;
}
bool yy_delete_buffer_is_called = false;
static YY_BUFFER_STATE spy_delete_buffer_arg_b = NULL;
static yyscan_t spy_delete_buffer_arg_scanner = NULL;
static void spy_yy_delete_buffer(YY_BUFFER_STATE b, yyscan_t scanner) {
	yy_delete_buffer_is_called = true;
	spy_delete_buffer_arg_b = b;
	spy_delete_buffer_arg_scanner = scanner;
}
bool yyrestart_is_called = false;
static FILE *spy_restart_arg_input_file = NULL;
static yyscan_t spy_restart_arg_scanner = NULL;
static void spy_yyrestart(FILE *input_file, yyscan_t scanner) {
	yyrestart_is_called = true;
	spy_restart_arg_input_file = input_file;
	spy_restart_arg_scanner = scanner;
}



// fakes

#define FAKABLE_MALLOC(n) (get_current_malloc()(n))
#define FAKABLE_FREE(p) (get_current_free()(p))
#define FAKABLE_STRDUP(s) (get_current_string_duplicate()(s))



//-----------------------------------------------------------------------------
// GENERAL HELPERS
//-----------------------------------------------------------------------------

typedef struct test_input_provider_deep_snapshot {
  input_provider_mode mode;
  FILE *file;
  char *buf_address;
  char *buf_content_p;
  size_t cap;
  size_t len;
  yyscan_t lexer_scanner;
  YY_BUFFER_STATE lexer_buffer_state;
} test_input_provider_deep_snapshot;

static test_input_provider_deep_snapshot deep_snapshot(const input_provider *p) {
  test_input_provider_deep_snapshot ret = {
    .mode = p->mode,
    .file = p->file,
    .buf_address = p->dbuf.buf,
    .cap = p->dbuf.cap,
    .len = p->dbuf.len,
    .lexer_scanner = p->lexer_scanner,
    .lexer_buffer_state = p->lexer_buffer_state };
  ret.buf_content_p = INPUT_PROVIDER_MALLOC(sizeof(char) * p->dbuf.cap);
  memcpy(ret.buf_content_p, p->dbuf.buf, p->dbuf.cap);
  return ret;
}

static void reset_spy_args_and_ret(void) {
	yy_scan_bytes_is_called = false;
	spy_scan_bytes_arg_bytes = NULL;
	spy_scan_bytes_arg_len = 0;
	spy_scan_bytes_arg_scanner = NULL;
	scan_bytes_ret = NULL;
	yy_delete_buffer_is_called = false;
	spy_delete_buffer_arg_b = NULL;
	spy_delete_buffer_arg_scanner = NULL;
	yyrestart_is_called = false;
	spy_restart_arg_input_file = NULL;
	spy_restart_arg_scanner = NULL;
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
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
	assert_true(fake_memory_no_leak());
	set_allocators(NULL, NULL);
	set_reallocator(NULL);
	fake_memory_reset();
	return 0;
}

static int fake_memory_and_spy_lexer_setup(void **state) {
	fake_memory_setup(state);
	input_provider_set_lexer_ops(&(lexer_ops_t){
        .yy_scan_bytes_fn = spy_yy_scan_bytes,
		.yy_delete_buffer_fn = spy_yy_delete_buffer,
		.yyrestart_fn = spy_yyrestart } );
	reset_spy_args_and_ret();
	return 0;
}

static int fake_memory_and_spy_lexer_teardown(void **state) {
	fake_memory_teardown(state);
	input_provider_reset_lexer_ops();
	reset_spy_args_and_ret();
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
  - arg input_provider

other elements of the isolated unit:
  - typedef struct yy_buffer_state *YY_BUFFER_STATE;
  - #define INPUT_PROVIDER_INITIAL_SIZE_OF_BUFFER 256
  - typedef struct input_provider_dynamic_buffer {
      char *buf;
      size_t cap;
      size_t len;
    } input_provider_dynamic_buffer;
  - struct input_provider {
      input_provider_mode mode;
      FILE *file;
      input_provider_dynamic_buffer dbuf;
      yyscan_t lexer_scanner; // borrowed from lexer ;
      // if mode == INPUT_PROVIDER_MODE_CHUNKS, owns its YY_BUFFER_STATE field
      // if mode == INPUT_PROVIDER_MODE_FILE, doesn't use its YY_BUFFER_STATE field at all and hence doesn't own its YY_BUFFER_STATE field
      YY_BUFFER_STATE lexer_buffer_state;
    };



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
//    - ret->mode == INPUT_PROVIDER_MODE_CHUNKS (default mode)
//    - ret->file == NULL
//    - ret->dbuf.buf != NULL
//    - ret->dbuf.cap == INPUT_PROVIDER_INITIAL_SIZE_OF_BUFFER
//    - ret->dbuf.len == 0
//    - ret->lexer_buffer_state == NULL
//  - No leaks, no invalid or double frees
//  - input_provider_destroy() properly frees all owned resources
static void create_success_when_no_oom(void **state) {
  (void)state;
  input_provider *ret = input_provider_create();

  assert_non_null(ret);
  assert_int_equal(ret->mode, INPUT_PROVIDER_MODE_CHUNKS);
  assert_null(ret->file);
  assert_non_null(ret->dbuf.buf);
  assert_int_equal(ret->dbuf.cap, INPUT_PROVIDER_INITIAL_SIZE_OF_BUFFER);
  assert_int_equal(ret->dbuf.len, 0);
  assert_null(ret->lexer_buffer_state);

  // Cleanup phase
  // destroy() must free dbuf.buf and the struct itself, without side effects.
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
//  - input_provider_create(void)
//  - void input_provider_destroy(input_provider *p);
//  - all args input_provider (p)


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
//  - scanner == NULL
// Expected:
//  - returns false
static void bind_to_scanner_returns_false_when_scanner_null(void **state) {
  (void)state;
  bool ret = input_provider_bind_to_scanner(DUMMY_INPUT_PROVIDER_P, NULL);
  assert_false(ret);
}

// Given:
//  - p is valid
//  - p->lexer_scanner == NULL
//  - scanner != NULL
// Expected:
//  - returns true
//  - p->lexer_scanner == scanner
//  - does not touch other fields (mode, file, dbuf, lexer_buffer_state)
static void bind_to_scanner_succeeds_when_first_bind(void **state) {
  (void)state;
  input_provider *p = input_provider_create();
  assert_non_null(p);
  assert_null(p->lexer_scanner);
  test_input_provider_deep_snapshot snapshot = deep_snapshot(p);

  bool ret = input_provider_bind_to_scanner(p, DUMMY_SCANNER);

  assert_true(ret);
  assert_ptr_equal(p->lexer_scanner, DUMMY_SCANNER);
  assert_int_equal(p->mode, snapshot.mode);
  assert_ptr_equal(p->file, snapshot.file);
  assert_ptr_equal(p->dbuf.buf, snapshot.buf_address);
  assert_memory_equal(snapshot.buf_content_p, p->dbuf.buf, snapshot.cap);
  assert_int_equal(p->dbuf.cap, snapshot.cap);
  assert_int_equal(p->dbuf.len, snapshot.len);
  assert_ptr_equal(p->lexer_buffer_state, snapshot.lexer_buffer_state);

  input_provider_destroy(p);
  INPUT_PROVIDER_FREE(snapshot.buf_content_p);
}

// Given:
//  - p is valid
//  - p->lexer_scanner != NULL
//  - scanner != NULL
// Expected:
//  - returns false
//  - p is unchanged
static void bind_to_scanner_fails_when_second_bind(void **state) {
  (void)state;
  input_provider *p = input_provider_create();
  assert_non_null(p);
  assert_null(p->lexer_scanner);
  p->lexer_scanner = DUMMY_SCANNER_2;
  test_input_provider_deep_snapshot snapshot = deep_snapshot(p);

  bool ret = input_provider_bind_to_scanner(p, DUMMY_SCANNER);

  assert_false(ret);
  assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);
  assert_int_equal(p->mode, snapshot.mode);
  assert_ptr_equal(p->file, snapshot.file);
  assert_ptr_equal(p->dbuf.buf, snapshot.buf_address);
  assert_memory_equal(snapshot.buf_content_p, p->dbuf.buf, snapshot.cap);
  assert_int_equal(p->dbuf.cap, snapshot.cap);
  assert_int_equal(p->dbuf.len, snapshot.len);
  assert_ptr_equal(p->lexer_buffer_state, snapshot.lexer_buffer_state);

  input_provider_destroy(p);
  INPUT_PROVIDER_FREE(snapshot.buf_content_p);
}



//-----------------------------------------------------------------------------
// TESTS
// bool input_provider_set_mode_chunks(input_provider *p);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


//  - bool input_provider_set_mode_chunks(input_provider *p);
//  - input_provider_create(void)
//  - void input_provider_destroy(input_provider *p);
//  - arg p


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
// Expected:
//  - returns true
//  - p->mode == INPUT_PROVIDER_MODE_CHUNKS
//  - does not touch other fields
static void set_mode_chunks_success_when_p_not_null(void **state) {
  (void)state;
  input_provider *p = input_provider_create();
  assert_non_null(p);
  p->mode = INPUT_PROVIDER_MODE_FILE;
  assert_null(p->lexer_scanner);
  p->lexer_scanner = DUMMY_SCANNER;
  test_input_provider_deep_snapshot snapshot = deep_snapshot(p);

  bool ret = input_provider_set_mode_chunks(p);

  assert_true(ret);
  assert_int_equal(p->mode, INPUT_PROVIDER_MODE_CHUNKS);
  assert_ptr_equal(p->file, snapshot.file);
  assert_ptr_equal(p->dbuf.buf, snapshot.buf_address);
  assert_memory_equal(snapshot.buf_content_p, p->dbuf.buf, snapshot.cap);
  assert_int_equal(p->dbuf.cap, snapshot.cap);
  assert_int_equal(p->dbuf.len, snapshot.len);
  assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);
  assert_ptr_equal(p->lexer_buffer_state, snapshot.lexer_buffer_state);

  input_provider_destroy(p);
  INPUT_PROVIDER_FREE(snapshot.buf_content_p);
}



//-----------------------------------------------------------------------------
// TESTS
// bool input_provider_set_mode_file(input_provider *p);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


//  - bool input_provider_set_mode_file(input_provider *p);
//  - input_provider_create(void)
//  - void input_provider_destroy(input_provider *p);
//  - arg p


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
static void set_mode_file_returns_false_when_p_null(void **state) {
  (void)state;
  bool ret = input_provider_set_mode_file(NULL);
  assert_false(ret);
}

// Given:
//  - p != NULL
// Expected:
//  - returns true
//  - p->mode == INPUT_PROVIDER_MODE_FILE
//  - does not touch other fields
static void set_mode_file_success_when_p_not_null(void **state) {
  (void)state;
  input_provider *p = input_provider_create();
  assert_non_null(p);
  p->mode = INPUT_PROVIDER_MODE_CHUNKS;
  assert_null(p->lexer_scanner);
  p->lexer_scanner = DUMMY_SCANNER;
  test_input_provider_deep_snapshot snapshot = deep_snapshot(p);

  bool ret = input_provider_set_mode_file(p);

  assert_true(ret);
  assert_int_equal(p->mode, INPUT_PROVIDER_MODE_FILE);
  assert_ptr_equal(p->file, snapshot.file);
  assert_ptr_equal(p->dbuf.buf, snapshot.buf_address);
  assert_memory_equal(snapshot.buf_content_p, p->dbuf.buf, snapshot.cap);
  assert_int_equal(p->dbuf.cap, snapshot.cap);
  assert_int_equal(p->dbuf.len, snapshot.len);
  assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);
  assert_ptr_equal(p->lexer_buffer_state, snapshot.lexer_buffer_state);

  input_provider_destroy(p);
  INPUT_PROVIDER_FREE(snapshot.buf_content_p);
}



//-----------------------------------------------------------------------------
// TESTS
// bool input_provider_buffer_reset(input_provider *p);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


//  - bool input_provider_buffer_reset(input_provider *p);
//  - input_provider_create(void)
//  - void input_provider_destroy(input_provider *p);
//  - arg p
//  - memcpy, strlen


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
static void buffer_reset_returns_false_when_p_null(void **state) {
  (void)state;
  bool ret = input_provider_buffer_reset(NULL);
  assert_false(ret);
}

// Given:
//  - p is valid
// Expected:
//  - returns true
//  - p->dbuf.len == 0
//  - 0 is written from p->dbuf.buf to p->dbuf.buf + p->dbuf.cap
//  - does not touch other fields
static void buffer_reset_success_when_p_valid(void **state) {
  (void)state;
  input_provider *p = input_provider_create();
  assert_non_null(p);
  INPUT_PROVIDER_FREE(p->dbuf.buf);
  size_t cap = 4 * INPUT_PROVIDER_INITIAL_SIZE_OF_BUFFER;
  p->dbuf.buf = INPUT_PROVIDER_MALLOC(sizeof(char) * cap);
  const char *s = "I'm written in the dynamic buffer of an input_provider!";
  size_t len = strlen(s);
  p->dbuf.len = len;
  memcpy(p->dbuf.buf, s, len);
  test_input_provider_deep_snapshot snapshot = deep_snapshot(p);
  char clean_space[INPUT_PROVIDER_INITIAL_SIZE_OF_BUFFER];
  memset(clean_space, 0, p->dbuf.cap);

  bool ret = input_provider_buffer_reset(p);

  assert_true(ret);
  assert_int_equal(p->mode, snapshot.mode);
  assert_ptr_equal(p->file, snapshot.file);
  assert_ptr_equal(p->dbuf.buf, snapshot.buf_address);
  assert_int_equal(p->dbuf.cap, snapshot.cap);
  assert_int_equal(p->dbuf.len, 0);
  assert_memory_equal(p->dbuf.buf, clean_space, p->dbuf.cap);
  assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);
  assert_ptr_equal(p->lexer_buffer_state, snapshot.lexer_buffer_state);

  input_provider_destroy(p);
  INPUT_PROVIDER_FREE(snapshot.buf_content_p);
}



//-----------------------------------------------------------------------------
// TESTS
// bool input_provider_buffer_append(
//     input_provider *p,
//     const char *bytes,
//     size_t len );
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


//  - bool input_provider_buffer_append(
//       input_provider *p,
//       const char *bytes,
//       size_t len );
//  - input_provider_create(void)
//  - void input_provider_destroy(input_provider *p);
//  - args p, bytes, len



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
static void buffer_append_returns_false_when_p_null(void **state) {
  (void)state;
  bool ret = input_provider_buffer_append(NULL, DUMMY_STRING, DUMMY_SIZE_T);
  assert_false(ret);
}

// Given:
//  - bytes == NULL
// Expected:
//  - returns false
static void buffer_append_returns_false_when_bytes_null(void **state) {
  (void)state;
  input_provider *p = input_provider_create();

  bool ret = input_provider_buffer_append(p, NULL, DUMMY_SIZE_T);

  assert_false(ret);

  input_provider_destroy(p);
}

// Given:
//  - p is valid
//  - at bytes, len bytes or more are allocated
//  - no '\0' in the first len bytes from bytes
//  - p->dbuf.len + len <= p->dbuf.cap
// Expected:
//  - returns true
//  - from p->dbuf.buf + p->dbuf.len to p->dbuf.buf + p->dbuf.len + len,
//    is written a copy of the first len bytes of bytes
//  - p->dbuf.len += len
//  - the first p->dbuf.len bytes of p->dbuf.buf are unchanged
//  - does not touch other fields
static void buffer_append_success_when_p_is_valid_and_no_cap_exceeding(void **state) {
  (void)state;
  input_provider *p = input_provider_create();
  assert_non_null(p);
  INPUT_PROVIDER_FREE(p->dbuf.buf);
  size_t cap = 4 * INPUT_PROVIDER_INITIAL_SIZE_OF_BUFFER;
  p->dbuf.buf = INPUT_PROVIDER_MALLOC(sizeof(char) * cap);
  const char *s = "I'm written in the dynamic buffer of an input_provider!";
  p->dbuf.len = strlen(s);
  memcpy(p->dbuf.buf, s, p->dbuf.len);
  test_input_provider_deep_snapshot snapshot = deep_snapshot(p);
  assert_true(strlen(DUMMY_STRING) - 2 >= 0);
  size_t len = strlen(DUMMY_STRING) - 2;
  char *bytes = INPUT_PROVIDER_MALLOC(sizeof(char) * strlen(DUMMY_STRING));
  memcpy(bytes, DUMMY_STRING, strlen(DUMMY_STRING)); // hence do not copy '\0'
  assert_true(p->dbuf.cap >= strlen(s) + len);

  bool ret = input_provider_buffer_append(p, bytes, len);

  assert_true(ret);
  assert_int_equal(p->mode, snapshot.mode);
  assert_ptr_equal(p->file, snapshot.file);
  assert_ptr_equal(p->dbuf.buf, snapshot.buf_address);
  assert_int_equal(p->dbuf.cap, snapshot.cap);
  assert_int_equal(p->dbuf.len, snapshot.len + len);
  assert_memory_equal(p->dbuf.buf, snapshot.buf_content_p, snapshot.len);
  assert_memory_equal(p->dbuf.buf + snapshot.len, bytes, len);
  assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);
  assert_ptr_equal(p->lexer_buffer_state, snapshot.lexer_buffer_state);

  input_provider_destroy(p);
  INPUT_PROVIDER_FREE(bytes);
  INPUT_PROVIDER_FREE(snapshot.buf_content_p);
}

// Given:
//  - p is valid
//  - at bytes, len bytes or more are allocated
//  - no '\0' in the first len bytes from bytes
//  - p->dbuf.len + len > p->dbuf.cap
//  - oom
// Expected:
//  - returns false
//  - p is unchanged
static void buffer_append_return_false_when_cap_exceeding_and_oom(void **state) {
  (void)state;
  input_provider *p = input_provider_create();
  assert_non_null(p);
  INPUT_PROVIDER_FREE(p->dbuf.buf);
  p->dbuf.cap = 4 * INPUT_PROVIDER_INITIAL_SIZE_OF_BUFFER;
  p->dbuf.buf = INPUT_PROVIDER_MALLOC(sizeof(char) * p->dbuf.cap);
  char *s = "I'm written in the dynamic buffer of an input_provider!";
  p->dbuf.len = strlen(s);
  memcpy(p->dbuf.buf, s, p->dbuf.len);
  test_input_provider_deep_snapshot snapshot = deep_snapshot(p);
  char *more_bytes_than_cap = INPUT_PROVIDER_MALLOC(sizeof(char) * (p->dbuf.cap + 1));
  memset(more_bytes_than_cap, 'a', 4 * INPUT_PROVIDER_INITIAL_SIZE_OF_BUFFER + 1);
  size_t len = p->dbuf.cap + 1;
  assert_true(p->dbuf.cap < strlen(s) + len);

  fake_memory_fail_on_all_call();

  bool ret = input_provider_buffer_append(p, more_bytes_than_cap, p->dbuf.cap + 1);

  fake_memory_fail_on_calls(0, NULL);

  assert_false(ret);
  assert_int_equal(p->mode, snapshot.mode);
  assert_ptr_equal(p->file, snapshot.file);
  assert_ptr_equal(p->dbuf.buf, snapshot.buf_address);
  assert_int_equal(p->dbuf.cap, snapshot.cap);
  assert_int_equal(p->dbuf.len, snapshot.len);
  assert_memory_equal(p->dbuf.buf, snapshot.buf_content_p, snapshot.cap);
  assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);
  assert_ptr_equal(p->lexer_buffer_state, snapshot.lexer_buffer_state);

  input_provider_destroy(p);
  INPUT_PROVIDER_FREE(more_bytes_than_cap);
  INPUT_PROVIDER_FREE(snapshot.buf_content_p);
}



//-----------------------------------------------------------------------------
// TESTS
// bool input_provider_set_file(input_provider *p, FILE *f);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


//  - bool input_provider_set_file(input_provider *p, FILE *f);
//  - input_provider_create(void)
//  - void input_provider_destroy(input_provider *p);
//  - arg p



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


/*
  - dummy:
    - arg f
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
static void set_file_returns_false_when_p_null(void **state) {
  (void)state;
  bool ret = input_provider_set_file(NULL, DUMMY_FILE_P);
  assert_false(ret);
}

// Given:
//  - f == NULL
// Expected:
//  - returns false
static void set_file_returns_false_when_f_null(void **state) {
  (void)state;
  bool ret = input_provider_set_file(DUMMY_INPUT_PROVIDER_P, NULL);
  assert_false(ret);
}

// Given:
//  - p is valid
// Expected:
//  - returns true
//  - p->file == f
//  - does not touch other fields
static void set_file_success_when_p_valid(void **state) {
  (void)state;
  input_provider *p = input_provider_create();
  assert_non_null(p);
  assert_null(p->lexer_scanner);
  test_input_provider_deep_snapshot snapshot = deep_snapshot(p);

  bool ret = input_provider_set_file(p, DUMMY_FILE_P);

  assert_true(ret);
  assert_ptr_equal(p->file, DUMMY_FILE_P);
  assert_int_equal(p->mode, snapshot.mode);
  assert_ptr_equal(p->dbuf.buf, snapshot.buf_address);
  assert_memory_equal(p->dbuf.buf, snapshot.buf_content_p, snapshot.cap);
  assert_int_equal(p->dbuf.cap, snapshot.cap);
  assert_int_equal(p->dbuf.len, snapshot.len);
  assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);
  assert_ptr_equal(p->lexer_buffer_state, snapshot.lexer_buffer_state);

  input_provider_destroy(p);
  INPUT_PROVIDER_FREE(snapshot.buf_content_p);
}



//-----------------------------------------------------------------------------
// TESTS
// bool input_provider_publish(input_provider *p);
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// ISOLATED UNIT
//-----------------------------------------------------------------------------


//  - bool input_provider_publish(input_provider *p);
//  - input_provider_create(void)
//  - void input_provider_destroy(input_provider *p);
//  - arg p



//-----------------------------------------------------------------------------
// DOUBLES
//-----------------------------------------------------------------------------


/*
  - spy:
    - yy_scan_bytes
    - yy_delete_buffer
    - yyrestart
  - fake:
    - malloc, free, realloc
*/



//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------


// fake_memory_and_spy_lexer_setup
// fake_memory_and_spy_lexer_teardown



//-----------------------------------------------------------------------------
// TESTS
//-----------------------------------------------------------------------------


// Given:
//  - p == NULL
// Expected:
//  - returns false
static void publish_returns_false_when_p_null(void **state) {
  (void)state;
  bool ret = input_provider_publish(NULL);
  assert_false(ret);
}

// Given:
//  - p is valid
//  - p->lexer_scanner == NULL
// Expected:
//  - returns false
//  - p is unchanged
static void publish_returns_false_when_scanner_null(void **state) {
	(void)state;
  	input_provider *p = input_provider_create();
	assert_null(p->lexer_scanner);
	test_input_provider_deep_snapshot snapshot = deep_snapshot(p);

	bool ret = input_provider_publish(NULL);

	assert_false(ret);
	assert_int_equal(p->mode, snapshot.mode);
	assert_ptr_equal(p->file, snapshot.file);
	assert_ptr_equal(p->dbuf.buf, snapshot.buf_address);
	assert_memory_equal(snapshot.buf_content_p, p->dbuf.buf, snapshot.cap);
	assert_int_equal(p->dbuf.cap, snapshot.cap);
	assert_int_equal(p->dbuf.len, snapshot.len);
	assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);
	assert_ptr_equal(p->lexer_buffer_state, snapshot.lexer_buffer_state);

	input_provider_destroy(p);
	INPUT_PROVIDER_FREE(snapshot.buf_content_p);
}

// Given:
//  - p is valid (hence p->dbuf is valid)
//  - p->lexer_scanner != NULL
//  - p->mode == INPUT_PROVIDER_MODE_CHUNKS
//  - yy_scan_bytes fails (returns NULL)
// Expected:
//  - yy_delete_buffer is not called
//  - yyrestart is not called
//  - yy_scan_bytes is called with:
//    - const char *bytes: p->dbuf.buf
//    - int len: (int) p->dbuf.len
// 	  - yyscan_t scanner: p->lexer_scanner
//  - returns false
//  - p is unchanged
static void publish_returns_false_and_no_side_effect_when_chunks_mode_and_scan_bytes_fails(void **state) {
	(void)state;
  	input_provider *p = input_provider_create();
	p->mode = INPUT_PROVIDER_MODE_CHUNKS;
	p->lexer_scanner = DUMMY_SCANNER;
	p->lexer_buffer_state = DUMMY_YY_BUFFER_STATE;
	const char *input = "a string that won't be scanned!";
	size_t len = strlen(input);
	assert_true(p->dbuf.len == 0);
	assert_true(len <= p->dbuf.cap);
	memcpy(p->dbuf.buf, input, len);
	p->dbuf.len = len;
	assert_non_null(p);
	assert_non_null(p->lexer_scanner);
	test_input_provider_deep_snapshot snapshot = deep_snapshot(p);
	scan_bytes_ret = NULL;

	bool ret = input_provider_publish(p);

	assert_false(yyrestart_is_called);
	assert_false(yy_delete_buffer_is_called);
	assert_true(yy_scan_bytes_is_called);
	assert_ptr_equal(spy_scan_bytes_arg_bytes, p->dbuf.buf);
	assert_int_equal(spy_scan_bytes_arg_len, (int)p->dbuf.len);
	assert_ptr_equal(spy_scan_bytes_arg_scanner, p->lexer_scanner);

	assert_false(ret);
	assert_int_equal(p->mode, snapshot.mode);
	assert_ptr_equal(p->file, snapshot.file);
	assert_ptr_equal(p->dbuf.buf, snapshot.buf_address);
	assert_memory_equal(snapshot.buf_content_p, p->dbuf.buf, snapshot.cap);
	assert_int_equal(p->dbuf.cap, snapshot.cap);
	assert_int_equal(p->dbuf.len, snapshot.len);
	assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);
	assert_ptr_equal(p->lexer_buffer_state, snapshot.lexer_buffer_state);

	input_provider_destroy(p);
	INPUT_PROVIDER_FREE(snapshot.buf_content_p);
}

// Given:
//  - p is valid (hence p->dbuf is valid)
//  - p->lexer_scanner != NULL
//  - p->mode == INPUT_PROVIDER_MODE_CHUNKS
//  - yy_scan_bytes succeeds
// Expected:
//  - yyrestart is not called
//  - yy_scan_bytes is called with:
//    - const char *bytes: p->dbuf.buf
//    - int len: (int) p->dbuf.len
// 	  - yyscan_t scanner: p->lexer_scanner
//  - yy_delete_buffer is called with:
//    - YY_BUFFER_STATE b: p->lexer_buffer_state
// 	  - yyscan_t scanner: p->lexer_scanner
//  - p->lexer_buffer_state == <ret of yy_scan_bytes>
//  - returns true
//  - the other fields of p are unchanged
static void publish_success_when_chunks_mode_and_scan_bytes_succeeds(void **state) {
	(void)state;
  	input_provider *p = input_provider_create();
	p->mode = INPUT_PROVIDER_MODE_CHUNKS;
	p->lexer_scanner = DUMMY_SCANNER;
	p->lexer_buffer_state = DUMMY_YY_BUFFER_STATE;
	const char *input = "a string that will be scanned!";
	size_t len = strlen(input);
	assert_true(p->dbuf.len == 0);
	assert_true(len <= p->dbuf.cap);
	memcpy(p->dbuf.buf, input, len);
	p->dbuf.len = len;
	assert_non_null(p);
	assert_non_null(p->lexer_scanner);
	test_input_provider_deep_snapshot snapshot = deep_snapshot(p);
	scan_bytes_ret = DUMMY_YY_BUFFER_STATE_2;

	bool ret = input_provider_publish(p);

	assert_false(yyrestart_is_called);
	assert_true(yy_scan_bytes_is_called);
	assert_ptr_equal(spy_scan_bytes_arg_bytes, p->dbuf.buf);
	assert_int_equal(spy_scan_bytes_arg_len, (int)p->dbuf.len);
	assert_ptr_equal(spy_scan_bytes_arg_scanner, p->lexer_scanner);
	assert_true(yy_delete_buffer_is_called);
	assert_ptr_equal(spy_delete_buffer_arg_b, snapshot.lexer_buffer_state);
	assert_ptr_equal(spy_delete_buffer_arg_scanner, p->lexer_scanner);
	assert_ptr_equal(p->lexer_buffer_state, DUMMY_YY_BUFFER_STATE_2);
	assert_true(ret);
	assert_int_equal(p->mode, snapshot.mode);
	assert_ptr_equal(p->file, snapshot.file);
	assert_ptr_equal(p->dbuf.buf, snapshot.buf_address);
	assert_memory_equal(snapshot.buf_content_p, p->dbuf.buf, snapshot.cap);
	assert_int_equal(p->dbuf.cap, snapshot.cap);
	assert_int_equal(p->dbuf.len, snapshot.len);
	assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);

	input_provider_destroy(p);
	INPUT_PROVIDER_FREE(snapshot.buf_content_p);
}

// Given:
//  - p is valid (hence p->dbuf is valid)
//  - p->lexer_scanner != NULL
//  - p->mode == INPUT_PROVIDER_MODE_FILE
// Expected:
//  - yy_scan_bytes is not called
//  - yy_delete_buffer is not called:
//  - yyrestart is called with:
//    - FILE *input_file: p->file
//    - yyscan_t scanner: p->lexer_scanner
//  - returns true
//  - p is unchanged
static void publish_success_when_file_mode(void **state) {
	(void)state;
  	input_provider *p = input_provider_create();
	p->mode = INPUT_PROVIDER_MODE_FILE;
	p->file = DUMMY_FILE_P;
	p->lexer_scanner = DUMMY_SCANNER;
	p->lexer_buffer_state = DUMMY_YY_BUFFER_STATE;
	const char *input = "a string that won't be scanned!";
	size_t len = strlen(input);
	assert_true(p->dbuf.len == 0);
	assert_true(len <= p->dbuf.cap);
	memcpy(p->dbuf.buf, input, len);
	p->dbuf.len = len;
	assert_non_null(p);
	assert_non_null(p->lexer_scanner);
	test_input_provider_deep_snapshot snapshot = deep_snapshot(p);

	bool ret = input_provider_publish(p);

	assert_false(yy_scan_bytes_is_called);
	assert_false(yy_delete_buffer_is_called);
	assert_true(yyrestart_is_called);
	assert_ptr_equal(spy_restart_arg_input_file, p->file);
	assert_ptr_equal(spy_restart_arg_scanner, p->lexer_scanner);
	assert_true(ret);
	assert_int_equal(p->mode, snapshot.mode);
	assert_ptr_equal(p->file, snapshot.file);
	assert_ptr_equal(p->dbuf.buf, snapshot.buf_address);
	assert_memory_equal(snapshot.buf_content_p, p->dbuf.buf, snapshot.cap);
	assert_int_equal(p->dbuf.cap, snapshot.cap);
	assert_int_equal(p->dbuf.len, snapshot.len);
	assert_ptr_equal(p->lexer_scanner, snapshot.lexer_scanner);
	assert_ptr_equal(p->lexer_buffer_state, snapshot.lexer_buffer_state);

	input_provider_destroy(p);
	INPUT_PROVIDER_FREE(snapshot.buf_content_p);
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
        bind_to_scanner_succeeds_when_first_bind,
        fake_memory_setup, fake_memory_teardown),
    cmocka_unit_test_setup_teardown(
        bind_to_scanner_fails_when_second_bind,
        fake_memory_setup, fake_memory_teardown),
  };
  const struct CMUnitTest set_mode_chunks_tests[] = {
    cmocka_unit_test_setup_teardown(
        set_mode_chunks_returns_false_when_p_null,
        fake_memory_setup, fake_memory_teardown),
    cmocka_unit_test_setup_teardown(
        set_mode_chunks_success_when_p_not_null,
        fake_memory_setup, fake_memory_teardown),
  };
  const struct CMUnitTest set_mode_file_tests[] = {
    cmocka_unit_test_setup_teardown(
        set_mode_file_returns_false_when_p_null,
        fake_memory_setup, fake_memory_teardown),
    cmocka_unit_test_setup_teardown(
        set_mode_file_success_when_p_not_null,
        fake_memory_setup, fake_memory_teardown),
  };
  const struct CMUnitTest buffer_reset_tests[] = {
    cmocka_unit_test_setup_teardown(
        buffer_reset_returns_false_when_p_null,
        fake_memory_setup, fake_memory_teardown),
    cmocka_unit_test_setup_teardown(
        buffer_reset_success_when_p_valid,
        fake_memory_setup, fake_memory_teardown),
  };
  const struct CMUnitTest buffer_append_tests[] = {
    cmocka_unit_test_setup_teardown(
        buffer_append_returns_false_when_p_null,
        fake_memory_setup, fake_memory_teardown),
    cmocka_unit_test_setup_teardown(
        buffer_append_returns_false_when_bytes_null,
        fake_memory_setup, fake_memory_teardown),
    cmocka_unit_test_setup_teardown(
        buffer_append_success_when_p_is_valid_and_no_cap_exceeding,
        fake_memory_setup, fake_memory_teardown),
    cmocka_unit_test_setup_teardown(
        buffer_append_return_false_when_cap_exceeding_and_oom,
        fake_memory_setup, fake_memory_teardown),
  };
  const struct CMUnitTest set_file_tests[] = {
    cmocka_unit_test_setup_teardown(
        set_file_returns_false_when_p_null,
        fake_memory_setup, fake_memory_teardown),
    cmocka_unit_test_setup_teardown(
        set_file_returns_false_when_f_null,
        fake_memory_setup, fake_memory_teardown),
    cmocka_unit_test_setup_teardown(
        set_file_success_when_p_valid,
        fake_memory_setup, fake_memory_teardown),
  };
  const struct CMUnitTest publish_tests[] = {
    cmocka_unit_test_setup_teardown(
        publish_returns_false_when_p_null,
        fake_memory_and_spy_lexer_setup, fake_memory_and_spy_lexer_teardown),
    cmocka_unit_test_setup_teardown(
        publish_returns_false_when_scanner_null,
        fake_memory_and_spy_lexer_setup, fake_memory_and_spy_lexer_teardown),
    cmocka_unit_test_setup_teardown(
        publish_returns_false_and_no_side_effect_when_chunks_mode_and_scan_bytes_fails,
        fake_memory_and_spy_lexer_setup, fake_memory_and_spy_lexer_teardown),
    cmocka_unit_test_setup_teardown(
        publish_success_when_chunks_mode_and_scan_bytes_succeeds,
        fake_memory_and_spy_lexer_setup, fake_memory_and_spy_lexer_teardown),
    cmocka_unit_test_setup_teardown(
        publish_success_when_file_mode,
        fake_memory_and_spy_lexer_setup, fake_memory_and_spy_lexer_teardown),
  };

  int failed = 0;
  failed += cmocka_run_group_tests(create_tests, NULL, NULL);
  failed += cmocka_run_group_tests(bind_to_scanner_tests, NULL, NULL);
  failed += cmocka_run_group_tests(set_mode_chunks_tests, NULL, NULL);
  failed += cmocka_run_group_tests(set_mode_file_tests, NULL, NULL);
  failed += cmocka_run_group_tests(buffer_reset_tests, NULL, NULL);
  failed += cmocka_run_group_tests(buffer_append_tests, NULL, NULL);
  failed += cmocka_run_group_tests(publish_tests, NULL, NULL);

  return failed;
}
