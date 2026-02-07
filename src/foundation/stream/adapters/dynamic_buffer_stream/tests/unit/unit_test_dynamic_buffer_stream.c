/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 * File:
 * src/foundation/stream/adapters/dynamic_buffer_stream/tests/unit/
 * unit_test_dynamic_buffer_stream_state.c
 *
 */

#include "dynamic_buffer_stream_test_api.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "lexleo_cmocka_xmacro_helpers.h"
#include "osal_mem_fake_provider.h"

// value used to pre-fill buf when check_buf_unchanged
static const uint8_t BUF_FILL = (uint8_t)0xCC;

enum { BUF_SIZE = 256 };
enum { BIG_N = 8192 };

//-----------------------------------------------------------------------------
// CONTRACT — dynamic_buffer_stream_create_stream()
//
// stream_status_t dynamic_buffer_stream_create_stream(
//     stream_t **out,
//     const dynamic_buffer_stream_ctx_t *ctx);
//
// - invalid args (out == NULL || ctx == NULL) => STREAM_STATUS_ERROR
// - any failure => STREAM_STATUS_ERROR and *out == NULL
// - success => STREAM_STATUS_OK and produces a valid stream_t
//
// Doubles: osal_mem_ops_t via fake_memory (OOM + leak checks)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PARAMETRIC TESTS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CASE STRUCTURE
//-----------------------------------------------------------------------------

typedef enum {
	DBS_CREATE_STREAM_SCENARIO_OK = 0,
	DBS_CREATE_STREAM_SCENARIO_OUT_NULL,
	DBS_CREATE_STREAM_SCENARIO_CTX_NULL,
	DBS_CREATE_STREAM_SCENARIO_OOM,
} dbs_create_stream_scenario_t;

typedef struct {
	const char *name;

	// arrange
	dbs_create_stream_scenario_t scenario;
	size_t fail_call_idx; // 0 = no OOM, otherwise 1-based (scenario == DBS_CREATE_STREAM_SCENARIO_OOM)

	// assert
	stream_status_t expected_ret;
	bool check_out; // whether we can assert on *out
	bool expect_out_null; // valid only if check_out == true
} test_dynamic_buffer_stream_create_stream_case_t;

typedef struct {
	// runtime resources
	stream_t *out;

	// injection
	const osal_mem_ops_t *mem;
	dynamic_buffer_stream_ctx_t ctx;

	// reference to test case
	const test_dynamic_buffer_stream_create_stream_case_t *tc;
} test_dynamic_buffer_stream_create_stream_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_create_stream(void **state) {
	const test_dynamic_buffer_stream_create_stream_case_t *tc =
		(const test_dynamic_buffer_stream_create_stream_case_t *)*state;

	test_dynamic_buffer_stream_create_stream_fixture_t *fx = malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	// fake memory initialization
	fake_memory_reset();
	if (tc->scenario == DBS_CREATE_STREAM_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	// fake memory injection (ARRANGE INFRASTRUCTURE)
	fx->mem = osal_mem_test_fake_ops();
	fx->ctx = dynamic_buffer_stream_default_ctx(fx->mem);

	*state = fx;

	return 0;
}

static int teardown_create_stream(void **state) {
	test_dynamic_buffer_stream_create_stream_fixture_t *fx =
		(test_dynamic_buffer_stream_create_stream_fixture_t *) (*state);

	if (fx->out) {
		stream_destroy(fx->out);
		fx->out = NULL;
	}

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

static void test_create_stream(void **state) {
	test_dynamic_buffer_stream_create_stream_fixture_t *fx =
		(test_dynamic_buffer_stream_create_stream_fixture_t *) (*state);
	const test_dynamic_buffer_stream_create_stream_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t ret = STREAM_STATUS_ERROR;
	stream_t **out_arg = &fx->out;
	const dynamic_buffer_stream_ctx_t *ctx_arg = &fx->ctx;
	if (tc->scenario == DBS_CREATE_STREAM_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == DBS_CREATE_STREAM_SCENARIO_CTX_NULL) ctx_arg = NULL;
	if (out_arg) fx->out = (stream_t *)(uintptr_t)0xDEADC0DEu;

	// ACT
	ret = dynamic_buffer_stream_create_stream(out_arg, ctx_arg);

	// ASSERT
	assert_int_equal(ret, tc->expected_ret);
	if (tc->check_out) {
		if (tc->expect_out_null) assert_null(fx->out);
		else assert_non_null(fx->out);
	}
	if (tc->scenario == DBS_CREATE_STREAM_SCENARIO_OK) {
		const char msg[] = "hello";
		char buf[8] = {0};
		assert_true((size_t)sizeof(msg)-1 == stream_write(fx->out, msg, sizeof(msg)-1));
		assert_true((size_t)sizeof(msg)-1 == stream_read(fx->out, buf, sizeof(msg)-1));
		assert_memory_equal(buf, msg, sizeof(msg)-1);
		assert_int_equal(stream_flush(fx->out), 0);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_dynamic_buffer_stream_create_stream_case_t CASE_CREATE_STREAM_OOM_1 = {
	.name = "create_stream_error_when_oom_1",
	.scenario = DBS_CREATE_STREAM_SCENARIO_OOM,
	.fail_call_idx = 1,

	.expected_ret = STREAM_STATUS_ERROR,
	.check_out = true,
	.expect_out_null = true
};

static const test_dynamic_buffer_stream_create_stream_case_t CASE_CREATE_STREAM_OOM_2 = {
	.name = "create_stream_error_when_oom_2",
	.scenario = DBS_CREATE_STREAM_SCENARIO_OOM,
	.fail_call_idx = 2,

	.expected_ret = STREAM_STATUS_ERROR,
	.check_out = true,
	.expect_out_null = true
};

static const test_dynamic_buffer_stream_create_stream_case_t CASE_CREATE_STREAM_OOM_3 = {
	.name = "create_stream_error_when_oom_3",
	.scenario = DBS_CREATE_STREAM_SCENARIO_OOM,
	.fail_call_idx = 3,

	.expected_ret = STREAM_STATUS_ERROR,
	.check_out = true,
	.expect_out_null = true
};

static const test_dynamic_buffer_stream_create_stream_case_t CASE_CREATE_STREAM_OUT_NULL = {
	.name = "create_stream_error_when_out_null",
	.scenario = DBS_CREATE_STREAM_SCENARIO_OUT_NULL,

	.expected_ret = STREAM_STATUS_ERROR,
	.check_out = false,
};

static const test_dynamic_buffer_stream_create_stream_case_t CASE_CREATE_STREAM_CTX_NULL = {
	.name = "create_stream_error_when_ctx_null",
	.scenario = DBS_CREATE_STREAM_SCENARIO_CTX_NULL,

	.expected_ret = STREAM_STATUS_ERROR,
	.check_out = false,
};

static const test_dynamic_buffer_stream_create_stream_case_t CASE_CREATE_STREAM_OK = {
	.name = "create_stream_success_when_ok",
	.scenario = DBS_CREATE_STREAM_SCENARIO_OK,

	.expected_ret = STREAM_STATUS_OK,
	.check_out = true,
	.expect_out_null = false
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define DYNAMIC_BUFFER_STREAM_CREATE_STREAM_CASES(X) \
X(CASE_CREATE_STREAM_OOM_1) \
X(CASE_CREATE_STREAM_OOM_2) \
X(CASE_CREATE_STREAM_OOM_3) \
X(CASE_CREATE_STREAM_OUT_NULL) \
X(CASE_CREATE_STREAM_CTX_NULL) \
X(CASE_CREATE_STREAM_OK)

#define DYNAMIC_BUFFER_STREAM_MAKE_CREATE_STREAM_TEST(case_sym) \
LEXLEO_MAKE_TEST(create_stream, case_sym)

static const struct CMUnitTest create_stream_tests[] = {
	DYNAMIC_BUFFER_STREAM_CREATE_STREAM_CASES(DYNAMIC_BUFFER_STREAM_MAKE_CREATE_STREAM_TEST)
};

#undef DYNAMIC_BUFFER_STREAM_CREATE_STREAM_CASES
#undef DYNAMIC_BUFFER_STREAM_MAKE_CREATE_STREAM_TEST

//-----------------------------------------------------------------------------
// CONTRACT — dynamic_buffer_stream (read behavior via stream port)
//
// size_t stream_read(stream_t *s, void *buf, size_t n)
//
// - If s == NULL OR buf == NULL OR n == 0:
//     returns 0 and does not touch `buf`.
// - If the stream currently has no readable bytes:
//     returns 0 and does not touch `buf`.
// - Otherwise:
//     reads up to `n` bytes, returns the number of bytes actually read,
//     and writes exactly that many bytes into `buf`.
// - Successive reads consume the stream data in order until empty.
//
// Notes (test oracle):
// - stream_write() is used only to preload bytes (ARRANGE).
// - stream_read() is the behavior under test (ACT/ASSERT).
//
// Doubles:
// - osal_mem_ops_t injected via fake_memory for memory-safety guardrails.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PARAMETRIC TESTS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CASE STRUCTURE
//-----------------------------------------------------------------------------

typedef struct {
	const char *name;

	// arrange
	bool s_null;
	bool buf_null;
	size_t n;

	const void *preload; // bytes to write before read (NULL => no preload)
	size_t preload_len;

	// assert
	size_t expected_ret;
	bool check_buf;
	const void *expected; // expected bytes read (often == preload)
	size_t expected_len; // usually == expected_ret
	bool check_buf_unchanged;
} test_dynamic_buffer_stream_read_case_t;

typedef struct {
	// runtime resources
	stream_t *stream;
	void *buf;
	uint8_t buf_storage[BUF_SIZE];

	// injection
	const osal_mem_ops_t *mem;
	dynamic_buffer_stream_ctx_t ctx;

	const test_dynamic_buffer_stream_read_case_t *tc;
} test_dynamic_buffer_stream_read_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_read(void **state) {
	const test_dynamic_buffer_stream_read_case_t *tc =
		(const test_dynamic_buffer_stream_read_case_t *)*state;

	test_dynamic_buffer_stream_read_fixture_t *fx = malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	// fake memory initialization
	fake_memory_reset();

	// fake memory injection (arrange infrastructure)
	fx->mem = osal_mem_test_fake_ops();
	fx->ctx = dynamic_buffer_stream_default_ctx(fx->mem);

	// assert infrastructure
	assert_true(tc->n <= sizeof(fx->buf_storage));
	if (tc->buf_null) assert_true(tc->n > 0 && !tc->check_buf && !tc->check_buf_unchanged);
	if (tc->preload && tc->preload_len > 0) assert_false(tc->s_null);
	if (tc->check_buf) assert_true(tc->expected_len <= sizeof(fx->buf_storage));
	assert_false(tc->check_buf && tc->check_buf_unchanged);
	if (tc->check_buf_unchanged) assert_false(tc->buf_null);

	*state = fx;

	return 0;
}

static int teardown_read(void **state) {
	test_dynamic_buffer_stream_read_fixture_t *fx =
		(test_dynamic_buffer_stream_read_fixture_t *) (*state);

	if (fx->stream) {
		stream_destroy(fx->stream);
		fx->stream = NULL;
	}

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

static void test_read(void **state) {
	test_dynamic_buffer_stream_read_fixture_t *fx =
		(test_dynamic_buffer_stream_read_fixture_t *) (*state);
	const test_dynamic_buffer_stream_read_case_t *tc = fx->tc;

	// ARRANGE
	size_t ret = (size_t)0xDEADC0DEu;
	uint8_t expected_fill[BUF_SIZE];
	if (!tc->s_null) {
		assert_int_equal(
			STREAM_STATUS_OK,
			dynamic_buffer_stream_create_stream(&fx->stream, &fx->ctx)
		);
	}
	if (!tc->buf_null) {
		fx->buf = fx->buf_storage;
		assert_non_null(fx->buf);
		memset(fx->buf, BUF_FILL, BUF_SIZE);
		if (tc->check_buf_unchanged) memset(expected_fill, BUF_FILL, BUF_SIZE);
	}
	if (tc->preload && tc->preload_len > 0) {
		assert_true(
			tc->preload_len
			==
			stream_write(fx->stream, tc->preload, tc->preload_len) );
	}

	// ACT
	ret = stream_read(fx->stream, fx->buf, tc->n);

	// ASSERT
	assert_true(ret == tc->expected_ret);
	if (tc->check_buf) {
		assert_memory_equal(fx->buf, tc->expected, tc->expected_len);
	}
	if (tc->check_buf_unchanged)
		assert_memory_equal(fx->buf, expected_fill, BUF_SIZE);
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_dynamic_buffer_stream_read_case_t CASE_READ_NULL = {
	.name = "read_ret_0_when_stream_null",
	.s_null = true,
	.buf_null = false,
	.n = 1,

	.preload = NULL,
	.preload_len = 0,

	.expected_ret = 0,
	.check_buf = false,
	.expected = "",
	.expected_len = 0,
	.check_buf_unchanged = true
};

static const test_dynamic_buffer_stream_read_case_t CASE_READ_BUF_NULL = {
	.name = "read_ret_0_when_buf_null",
	.s_null = false,
	.buf_null = true,
	.n = 1,

	.preload = NULL,
	.preload_len = 0,

	.expected_ret = 0,
	.check_buf = false,
	.expected = "",
	.expected_len = 0,
	.check_buf_unchanged = false
};

static const test_dynamic_buffer_stream_read_case_t CASE_READ_EMPTY = {
	.name = "read_ret_0_when_dbs_empty",
	.s_null = false,
	.buf_null = false,
	.n = 1,

	.preload = NULL,
	.preload_len = 0,

	.expected_ret = 0,
	.check_buf = false,
	.expected = "",
	.expected_len = 0,
	.check_buf_unchanged = true
};

static const test_dynamic_buffer_stream_read_case_t CASE_READ_HELLO_READ_2 = {
	.name = "read_read_2_when_hello_in_dbs",
	.s_null = false,
	.buf_null = false,
	.n = 2,

	.preload = "hello",
	.preload_len = 5,

	.expected_ret = 2,
	.check_buf = true,
	.expected = "he",
	.expected_len = 2,
	.check_buf_unchanged = false
};

static const test_dynamic_buffer_stream_read_case_t CASE_READ_HELLO_READ_6 = {
	.name = "read_ret_5_when_hello_in_dbs_and_read_6",
	.s_null = false,
	.buf_null = false,
	.n = 6,

	.preload = "hello",
	.preload_len = 5,

	.expected_ret = 5,
	.check_buf = true,
	.expected = "hello",
	.expected_len = 5,
	.check_buf_unchanged = false
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define DYNAMIC_BUFFER_STREAM_READ_CASES(X) \
X(CASE_READ_NULL) \
X(CASE_READ_BUF_NULL) \
X(CASE_READ_EMPTY) \
X(CASE_READ_HELLO_READ_2) \
X(CASE_READ_HELLO_READ_6)

#define DYNAMIC_BUFFER_STREAM_MAKE_READ_TEST(case_sym) \
LEXLEO_MAKE_TEST(read, case_sym)

static const struct CMUnitTest read_tests[] = {
	DYNAMIC_BUFFER_STREAM_READ_CASES(DYNAMIC_BUFFER_STREAM_MAKE_READ_TEST)
};

#undef DYNAMIC_BUFFER_STREAM_READ_CASES
#undef DYNAMIC_BUFFER_STREAM_MAKE_READ_TEST

//-----------------------------------------------------------------------------
// STATEFUL TESTS
//-----------------------------------------------------------------------------

static void test_read_consumes_progressively(void **state) {
	(void)state;

	// Arrange
	fake_memory_reset();
	const osal_mem_ops_t *mem = osal_mem_test_fake_ops();
	dynamic_buffer_stream_ctx_t ctx = dynamic_buffer_stream_default_ctx(mem);

	stream_t *s = NULL;
	assert_int_equal(
		STREAM_STATUS_OK,
		dynamic_buffer_stream_create_stream(&s, &ctx)
	);

	assert_true((size_t)5 == stream_write(s, "hello", 5));

	uint8_t buf[BUF_SIZE];
	memset(buf, BUF_FILL, sizeof(buf));

	// Act + Assert #1
	assert_true((size_t)2 == stream_read(s, buf, 2));
	assert_memory_equal(buf, "he", 2);

	// Act + Assert #2
	memset(buf, BUF_FILL, sizeof(buf));
	assert_true((size_t)3 == stream_read(s, buf, 10));
	assert_memory_equal(buf, "llo", 3);

	// Cleanup
	stream_destroy(s);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static void test_read_drains_then_returns_0(void **state) {
	(void)state;

	fake_memory_reset();
	const osal_mem_ops_t *mem = osal_mem_test_fake_ops();
	dynamic_buffer_stream_ctx_t ctx = dynamic_buffer_stream_default_ctx(mem);

	stream_t *s = NULL;
	assert_int_equal(STREAM_STATUS_OK, dynamic_buffer_stream_create_stream(&s, &ctx));
	assert_true((size_t)2 == stream_write(s, "hi", 2));

	uint8_t buf[BUF_SIZE];
	memset(buf, BUF_FILL, sizeof(buf));

	assert_true((size_t)2 == stream_read(s, buf, 2));
	assert_memory_equal(buf, "hi", 2);

	memset(buf, BUF_FILL, sizeof(buf));
	assert_true((size_t)0 == stream_read(s, buf, 1));

	stream_destroy(s);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

//-----------------------------------------------------------------------------
// STATEFUL TESTS REGISTRY
//-----------------------------------------------------------------------------

static const struct CMUnitTest read_stateful_tests[] = {
	cmocka_unit_test(test_read_consumes_progressively),
	cmocka_unit_test(test_read_drains_then_returns_0),
};

//-----------------------------------------------------------------------------
// CONTRACT — dynamic_buffer_stream (write behavior via stream port)
//
// size_t stream_write(stream_t *s, const void *buf, size_t n)
//
// - If s == NULL OR (buf == NULL AND n > 0) OR n == 0:
//     returns 0 and does not modify stream state.
// - Otherwise:
//     appends up to `n` bytes from `buf` to the stream.
//     returns the number of bytes actually written.
// - After a successful write, the written bytes become readable via stream_read()
//   in FIFO order (preserves byte order).
// - If n is so large that (current_len + n) would overflow size_t
//   (i.e., n > SIZE_MAX - current_len), or if allocation fails:
//     returns 0 and does not modify stream contents.
//
// Notes (test oracle):
// - Use stream_read() to observe what was written (round-trip).
//
// Doubles:
// - osal_mem_ops_t injected via fake_memory for memory-safety guardrails.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PARAMETRIC TESTS
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CASE STRUCTURE
//-----------------------------------------------------------------------------

typedef struct {
	const char *name;

	// arrange
	bool s_null;
	size_t n;

	const void *buf;
	size_t buf_len; // usually == n, but kept explicit for binary cases

	// assert
	size_t expected_ret;
	bool check_roundtrip; // if true: read back and compare
	const void *expected_readback; // expected readable bytes after write
	size_t expected_len;
} test_dynamic_buffer_stream_write_case_t;

typedef struct {
	// runtime resources
    stream_t *stream;
    uint8_t readback[BUF_SIZE];

	// injection
    const osal_mem_ops_t *mem;
    dynamic_buffer_stream_ctx_t ctx;

    const test_dynamic_buffer_stream_write_case_t *tc;
} test_dynamic_buffer_stream_write_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_write(void **state) {
	const test_dynamic_buffer_stream_write_case_t *tc =
		(const test_dynamic_buffer_stream_write_case_t *)*state;

	test_dynamic_buffer_stream_write_fixture_t *fx = malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	// fake memory initialization
	fake_memory_reset();

	// fake memory injection (arrange infrastructure)
	fx->mem = osal_mem_test_fake_ops();
	fx->ctx = dynamic_buffer_stream_default_ctx(fx->mem);

	// assert infrastructure
	if (tc->n == 0) assert_true(tc->expected_ret == 0);
	if (tc->check_roundtrip) {
		assert_non_null(tc->buf);
		assert_true(tc->n > 0);
		assert_true(tc->expected_len <= BUF_SIZE);
		assert_non_null(tc->expected_readback);
		assert_false(tc->s_null);
	} else {
		assert_true(tc->expected_len == 0);
	}
	if (tc->buf == NULL && tc->n > 0) {
		assert_false(tc->check_roundtrip);
	}
	if (tc->buf != NULL) assert_true(tc->buf_len >= tc->n);
	if (tc->n == 0) {
		assert_false(tc->check_roundtrip);
	}

	*state = fx;

	return 0;
}

static int teardown_write(void **state) {
	test_dynamic_buffer_stream_write_fixture_t *fx =
		(test_dynamic_buffer_stream_write_fixture_t *) (*state);

	if (fx->stream) {
		stream_destroy(fx->stream);
		fx->stream = NULL;
	}

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

static void test_write(void **state) {
	test_dynamic_buffer_stream_write_fixture_t *fx =
		(test_dynamic_buffer_stream_write_fixture_t *) (*state);
	const test_dynamic_buffer_stream_write_case_t *tc = fx->tc;

	// ARRANGE
	size_t ret = (size_t)0xDEADC0DEu;
	memset(fx->readback, BUF_FILL, sizeof(fx->readback));
	if (!tc->s_null) {
		assert_int_equal(
			STREAM_STATUS_OK,
			dynamic_buffer_stream_create_stream(&fx->stream, &fx->ctx)
		);
	}

	// ACT
	ret = stream_write(fx->stream, tc->buf, tc->n);

	// ASSERT
	assert_true(ret == tc->expected_ret);
	if (tc->check_roundtrip) {
		assert_true(
			tc->expected_len
			==
			stream_read(fx->stream, fx->readback, tc->expected_len) );
		assert_memory_equal(fx->readback, tc->expected_readback, tc->expected_len);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_dynamic_buffer_stream_write_case_t CASE_WRITE_STREAM_NULL = {
	.name = "write_ret_0_when_stream_null",
	.s_null = true,
	.buf = "dummy",
	.buf_len = 5,
	.n = 1,

	.expected_ret = 0,
	.check_roundtrip = false,
	.expected_readback = NULL,
	.expected_len = 0,
};

static const test_dynamic_buffer_stream_write_case_t CASE_WRITE_BUF_NULL = {
	.name = "write_ret_0_when_buf_null",
	.s_null = false,
	.buf = NULL,
	.buf_len = 0,
	.n = 1,

	.expected_ret = 0,
	.check_roundtrip = false,
	.expected_readback = NULL,
	.expected_len = 0,
};

static const test_dynamic_buffer_stream_write_case_t CASE_WRITE_N_0 = {
	.name = "write_ret_0_when_n_0",
	.s_null = false,
	.buf = "dummy",
	.buf_len = 4,
	.n = 0,

	.expected_ret = 0,
	.check_roundtrip = false,
	.expected_readback = NULL,
	.expected_len = 0,
};

static const test_dynamic_buffer_stream_write_case_t CASE_WRITE_N_2_BUFLEN_5 = {
	.name = "write_ret_2_when_n_2_buflen_5",
	.s_null = false,
	.buf = "hello",
	.buf_len = 5,
	.n = 2,

	.expected_ret = 2,
	.check_roundtrip = true,
	.expected_readback = "he",
	.expected_len = 2,
};

static const test_dynamic_buffer_stream_write_case_t CASE_WRITE_N_5_BUFLEN_5 = {
	.name = "write_ret_5_when_n_5_buflen_5",
	.s_null = false,
	.buf = "hello",
	.buf_len = 5,
	.n = 5,

	.expected_ret = 5,
	.check_roundtrip = true,
	.expected_readback = "hello",
	.expected_len = 5,
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define DYNAMIC_BUFFER_STREAM_WRITE_CASES(X) \
X(CASE_WRITE_STREAM_NULL) \
X(CASE_WRITE_BUF_NULL) \
X(CASE_WRITE_N_0) \
X(CASE_WRITE_N_2_BUFLEN_5) \
X(CASE_WRITE_N_5_BUFLEN_5)

#define DYNAMIC_BUFFER_STREAM_MAKE_WRITE_TEST(case_sym) \
LEXLEO_MAKE_TEST(write, case_sym)

static const struct CMUnitTest write_tests[] = {
	DYNAMIC_BUFFER_STREAM_WRITE_CASES(DYNAMIC_BUFFER_STREAM_MAKE_WRITE_TEST)
};

#undef DYNAMIC_BUFFER_STREAM_WRITE_CASES
#undef DYNAMIC_BUFFER_STREAM_MAKE_WRITE_TEST

//-----------------------------------------------------------------------------
// STATEFUL TESTS
//-----------------------------------------------------------------------------

static void test_write_appends_progressively(void **state) {
	(void)state;

	// Arrange
	fake_memory_reset();
	const osal_mem_ops_t *mem = osal_mem_test_fake_ops();
	dynamic_buffer_stream_ctx_t ctx = dynamic_buffer_stream_default_ctx(mem);

	stream_t *s = NULL;
	assert_int_equal(
		STREAM_STATUS_OK,
		dynamic_buffer_stream_create_stream(&s, &ctx)
	);

	const uint8_t *all = "hello";
	const uint8_t *start = "he";
	const uint8_t *end = (const uint8_t *)"llo";
	uint8_t readbuf[BUF_SIZE];
	memset(readbuf, BUF_FILL, sizeof(readbuf));

	// Act
	assert_true((size_t)2 == stream_write(s, start, 2));
	assert_true((size_t)2 == stream_write(s, end, 2));
	assert_true((size_t)1 == stream_write(s, end + 2, 1));

	// Assert
	assert_true((size_t)5 == stream_read(s, readbuf, 5));
	assert_memory_equal(readbuf, all, 5);

	// Cleanup
	stream_destroy(s);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static void test_write_grows_buffer_for_large_write(void **state) {
	(void)state;

	// Arrange
	fake_memory_reset();
	const osal_mem_ops_t *mem = osal_mem_test_fake_ops();
	dynamic_buffer_stream_ctx_t ctx = dynamic_buffer_stream_default_ctx(mem);

	stream_t *s = NULL;
	assert_int_equal(
		STREAM_STATUS_OK,
		dynamic_buffer_stream_create_stream(&s, &ctx)
	);

	uint8_t buf[BIG_N];
	uint8_t readbuf[BIG_N];
	uint8_t expected[BIG_N];

	for (size_t i = 0; i < BIG_N; i++) {
		buf[i] = (uint8_t)0xAB;
		expected[i] = buf[i];
	}

	// Act
	assert_true((size_t)BIG_N == stream_write(s, buf, BIG_N));

	// Assert
	assert_true((size_t)BIG_N == stream_read(s, readbuf, BIG_N));
	assert_memory_equal(readbuf, expected, BIG_N);

	// Cleanup
	stream_destroy(s);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static void test_write_fails_on_oom_and_preserves_existing_data(void **state) {
	(void)state;

	// Arrange
	fake_memory_reset();
	const osal_mem_ops_t *mem = osal_mem_test_fake_ops();
	dynamic_buffer_stream_ctx_t ctx = dynamic_buffer_stream_default_ctx(mem);

	stream_t *s = NULL;
	assert_int_equal(
		STREAM_STATUS_OK,
		dynamic_buffer_stream_create_stream(&s, &ctx)
	);

	const uint8_t *small = (const uint8_t *)"hello";
	uint8_t large[BIG_N];
	for (size_t i = 0; i < BIG_N; i++) large[i] = (uint8_t)0xAB;
	uint8_t readbuf[BUF_SIZE];
	memset(readbuf, BUF_FILL, sizeof(readbuf));
	uint8_t extra[1];
	memset(extra, BUF_FILL, sizeof(extra));

	// Act
	assert_true((size_t)5 == stream_write(s, small, 5));
	fake_memory_fail_only_on_call(1);
	assert_true((size_t)0 == stream_write(s, large, BIG_N));

	// Assert
	assert_true((size_t)5 == stream_read(s, readbuf, 5));
	assert_memory_equal(readbuf, small, 5);
	assert_true((size_t)0 == stream_read(s, extra, 1));

	// Cleanup
	stream_destroy(s);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

//-----------------------------------------------------------------------------
// STATEFUL TESTS REGISTRY
//-----------------------------------------------------------------------------

static const struct CMUnitTest write_stateful_tests[] = {
	cmocka_unit_test(test_write_appends_progressively),
	cmocka_unit_test(test_write_grows_buffer_for_large_write),
	cmocka_unit_test(test_write_fails_on_oom_and_preserves_existing_data),
};

//-----------------------------------------------------------------------------
// CONTRACT — dynamic_buffer_stream (flush behavior via stream port)
//
// int stream_flush(stream_t *s);
//
// - no-op
// - return 0
//
// Notes (test oracle):
// - stream_write() is used only to preload bytes (ARRANGE).
// - stream_read() is the behavior under test (ACT/ASSERT).
//
// Doubles:
// - osal_mem_ops_t injected via fake_memory for memory-safety guardrails.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// STATEFUL TESTS
//-----------------------------------------------------------------------------

static void test_flush_no_op(void **state) {
	(void)state;

	// Arrange
	fake_memory_reset();
	const osal_mem_ops_t *mem = osal_mem_test_fake_ops();
	dynamic_buffer_stream_ctx_t ctx = dynamic_buffer_stream_default_ctx(mem);

	stream_t *s = NULL;
	assert_int_equal(
		STREAM_STATUS_OK,
		dynamic_buffer_stream_create_stream(&s, &ctx)
	);

	uint8_t readbuf[BUF_SIZE];
	memset(readbuf, BUF_FILL, sizeof(readbuf));

	assert_true((size_t)5 == stream_write(s, "hello", 5));

	// Act and assert
	assert_int_equal(0, stream_flush(s));
	assert_true((size_t)5 == stream_read(s, readbuf, 5));
	assert_memory_equal(readbuf, "hello", 5);
	assert_true((size_t)0 == stream_read(s, readbuf, 1));

	// Cleanup
	stream_destroy(s);
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

//-----------------------------------------------------------------------------
// STATEFUL TESTS REGISTRY
//-----------------------------------------------------------------------------

static const struct CMUnitTest flush_stateful_tests[] = {
	cmocka_unit_test(test_flush_no_op)
};

//-----------------------------------------------------------------------------
// CONTRACT — dynamic_buffer_stream (destroy/close behavior via stream port)
//
// void stream_destroy(stream_t *s);
//
// - releases the stream object and its backend resources
// - safe on NULL (no-op)
// - after destroy, the stream handle is invalid (no further operations are
//   defined)
//
// Notes (test oracle):
// - correctness is defined by memory-safety invariants (no leak / no
//   invalid free / no double free)
// - no post-destroy behavior is tested (out of contract)
//
// Doubles:
// - osal_mem_ops_t injected via fake_memory for memory-safety guardrails.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// STATEFUL TESTS
//-----------------------------------------------------------------------------

static void test_destroy_no_op_when_null(void **state) {
	(void)state;

	// Arrange
	fake_memory_reset();

	// Act
	stream_destroy(NULL);

	// Assert
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

static void test_destroy_after_partial_io(void **state) {
	(void)state;

	// Arrange
	fake_memory_reset();
	const osal_mem_ops_t *mem = osal_mem_test_fake_ops();
	dynamic_buffer_stream_ctx_t ctx = dynamic_buffer_stream_default_ctx(mem);

	stream_t *s = NULL;
	assert_int_equal(
		STREAM_STATUS_OK,
		dynamic_buffer_stream_create_stream(&s, &ctx)
	);

	uint8_t readbuf[BUF_SIZE];
	memset(readbuf, BUF_FILL, sizeof(readbuf));

	assert_true((size_t)5 == stream_write(s, "hello", 5));
	assert_true((size_t)2 == stream_read(s, readbuf, 2));
	assert_memory_equal(readbuf, "he", 2);

	// Act
	stream_destroy(s);

	// Assert
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

//-----------------------------------------------------------------------------
// STATEFUL TESTS REGISTRY
//-----------------------------------------------------------------------------

static const struct CMUnitTest destroy_stateful_tests[] = {
	cmocka_unit_test(test_destroy_no_op_when_null),
	cmocka_unit_test(test_destroy_after_partial_io)
};

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	int failed = 0;
	failed += cmocka_run_group_tests(create_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(read_tests, NULL, NULL);
	failed += cmocka_run_group_tests(read_stateful_tests, NULL, NULL);
	failed += cmocka_run_group_tests(write_tests, NULL, NULL);
	failed += cmocka_run_group_tests(write_stateful_tests, NULL, NULL);
	failed += cmocka_run_group_tests(flush_stateful_tests, NULL, NULL);
	failed += cmocka_run_group_tests(destroy_stateful_tests, NULL, NULL);
	return failed;
}
