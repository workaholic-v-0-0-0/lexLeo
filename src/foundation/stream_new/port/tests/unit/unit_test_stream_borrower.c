/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_stream_borrower.c
 * @ingroup stream_unit_tests
 * @brief Unit tests implementation for stream_borrower.c.
 *
 * See also:
 * - @ref testing_foundation_stream_borrower_unit
 * "stream_borrower.c unit tests page"
 * - @ref specifications_stream "stream specifications"
 */

#include "stream/borrowers/stream_borrowers_api.h"

#include "stream/owners/stream_owners_api.h"
#include "stream/cr/stream_cr_api.h"

#include "stream/tests/stream_fake_adapter.h"
#include "stream/tests/stream_white_box_tests_access.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/osal_mem_ops.h"

#include "lexleo_cmocka.h"

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream_read()`.
 *
 * See contract:
 * - @ref specifications_stream_read "stream_read() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_unit_stream_read "stream_read() unit tests section"
 */
typedef enum {
	STREAM_READ_SCENARIO_N_NONZERO_S_NULL = 0,
	STREAM_READ_SCENARIO_N_NONZERO_BUF_NULL,
	STREAM_READ_SCENARIO_N_ZERO_ST_NULL,
	STREAM_READ_SCENARIO_N_ZERO_ST_NOT_NULL,
	STREAM_READ_SCENARIO_IO_ERROR_ST_NOT_NULL,
	STREAM_READ_SCENARIO_EOF_ST_NOT_NULL,
	STREAM_READ_SCENARIO_OK
} stream_read_scenario_t;

typedef struct {
	const char *name;
	stream_read_scenario_t scenario;
} test_stream_read_case_t;

typedef struct {
	stream_t *stream;
	stream_fake_adapter_backend_t fake_adapter_backend;
	const osal_mem_ops_t *mem;
	const test_stream_read_case_t *tc;
} test_stream_read_fixture_t;

static int setup_stream_read(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stream_read, state, tc, fx);

	stream_fake_adapter_init_backend(&fx->fake_adapter_backend);
	fx->mem = osal_mem_default_ops();
	stream_env_t env = stream_default_env(&stream_fake_adapter_vtbl, fx->mem);
	assert_int_equal(stream_create(&fx->stream, &env), STREAM_STATUS_OK);
	stream_inject_backend(fx->stream, (void *)&fx->fake_adapter_backend);
	assert_int_equal(stream_complete_default_init(fx->stream, NULL), STREAM_STATUS_OK);

	*state = fx;
	return 0;
}

static int teardown_stream_read(void **state)
{
	test_stream_read_fixture_t *fx = (test_stream_read_fixture_t *)(*state);
	stream_destroy(&fx->stream);
	osal_free(fx);
	return 0;
}

static void test_stream_read(void **state)
{
	test_stream_read_fixture_t *fx = (test_stream_read_fixture_t *)(*state);
	const test_stream_read_case_t *tc = fx->tc;

	// ARRANGE
	stream_t *s_arg = fx->stream;
	char buf[64] = {0};
	void *buf_arg = buf;
	size_t n_arg = 3;
	stream_status_t st = STREAM_STATUS_OK;
	stream_status_t *st_arg = &st;
	switch (tc->scenario) {
		case STREAM_READ_SCENARIO_N_NONZERO_S_NULL:
			s_arg = NULL;
			break;
		case STREAM_READ_SCENARIO_N_NONZERO_BUF_NULL:
			buf_arg = NULL;
			break;
		case STREAM_READ_SCENARIO_N_ZERO_ST_NULL:
			n_arg = 0;
			st_arg = NULL;
			break;
		case STREAM_READ_SCENARIO_IO_ERROR_ST_NOT_NULL:
			fx->fake_adapter_backend.next_read_st = STREAM_STATUS_IO_ERROR;
			fx->fake_adapter_backend.next_read_ret = 0;
			break;
		case STREAM_READ_SCENARIO_N_ZERO_ST_NOT_NULL:
			n_arg = 0;
			break;
		case STREAM_READ_SCENARIO_EOF_ST_NOT_NULL:
			fx->fake_adapter_backend.next_read_st = STREAM_STATUS_EOF;
			fx->fake_adapter_backend.next_read_ret = 2;
			break;
		case STREAM_READ_SCENARIO_OK:
			fx->fake_adapter_backend.next_read_st = STREAM_STATUS_OK;
			fx->fake_adapter_backend.next_read_ret = 2;
			break;
		default: fail();
	}

	// ACT
	size_t ret = stream_read(s_arg, buf_arg, n_arg, st_arg);

	// ASSERT
	switch (tc->scenario) {
		case STREAM_READ_SCENARIO_N_NONZERO_S_NULL:
		case STREAM_READ_SCENARIO_N_NONZERO_BUF_NULL:
			assert_int_equal(st, STREAM_STATUS_INVALID);
			assert_true(ret == 0);
			assert_true(fx->fake_adapter_backend.read_call_count == 0);
			break;
		case STREAM_READ_SCENARIO_N_ZERO_ST_NULL:
			assert_true(ret == 0);
			assert_true(fx->fake_adapter_backend.read_call_count == 0);
			break;
		case STREAM_READ_SCENARIO_N_ZERO_ST_NOT_NULL:
			assert_int_equal(st, STREAM_STATUS_OK);
			assert_true(ret == 0);
			assert_true(fx->fake_adapter_backend.read_call_count == 0);
			break;
		case STREAM_READ_SCENARIO_IO_ERROR_ST_NOT_NULL:
			assert_int_equal(st, STREAM_STATUS_IO_ERROR);
			assert_true(ret == 0);
			assert_true(fx->fake_adapter_backend.read_call_count == 1);
			assert_ptr_equal(fx->fake_adapter_backend.last_read_buf, buf_arg);
			assert_true(fx->fake_adapter_backend.last_read_n == n_arg);
			assert_ptr_equal(fx->fake_adapter_backend.last_read_backend, &fx->fake_adapter_backend);
			assert_true(fx->fake_adapter_backend.last_read_st == st_arg);
			break;
		case STREAM_READ_SCENARIO_EOF_ST_NOT_NULL:
			assert_int_equal(st, STREAM_STATUS_EOF);
			assert_true(ret == fx->fake_adapter_backend.next_read_ret);
			assert_true(fx->fake_adapter_backend.read_call_count == 1);
			assert_ptr_equal(fx->fake_adapter_backend.last_read_buf, buf_arg);
			assert_true(fx->fake_adapter_backend.last_read_n == n_arg);
			assert_ptr_equal(fx->fake_adapter_backend.last_read_backend, &fx->fake_adapter_backend);
			assert_true(fx->fake_adapter_backend.last_read_st == st_arg);
			break;
		case STREAM_READ_SCENARIO_OK:
			assert_int_equal(st, STREAM_STATUS_OK);
			assert_true(ret == fx->fake_adapter_backend.next_read_ret);
			assert_true(fx->fake_adapter_backend.read_call_count == 1);
			assert_ptr_equal(fx->fake_adapter_backend.last_read_buf, buf_arg);
			assert_true(fx->fake_adapter_backend.last_read_n == n_arg);
			assert_ptr_equal(fx->fake_adapter_backend.last_read_backend, &fx->fake_adapter_backend);
			assert_true(fx->fake_adapter_backend.last_read_st == st_arg);
			break;
		default: fail();
	}
}

static const test_stream_read_case_t CASE_STREAM_READ_N_NONZERO_S_NULL = {
	.name = "stream_read_n_nonzero_s_null",
	.scenario = STREAM_READ_SCENARIO_N_NONZERO_S_NULL,
};

static const test_stream_read_case_t CASE_STREAM_READ_N_NONZERO_BUF_NULL = {
	.name = "stream_read_n_nonzero_buf_null",
	.scenario = STREAM_READ_SCENARIO_N_NONZERO_BUF_NULL,
};

static const test_stream_read_case_t CASE_STREAM_READ_N_ZERO_ST_NULL = {
	.name = "stream_read_n_zero_st_null",
	.scenario = STREAM_READ_SCENARIO_N_ZERO_ST_NULL,
};

static const test_stream_read_case_t CASE_STREAM_READ_N_ZERO_ST_NOT_NULL = {
	.name = "stream_read_n_zero_st_not_null",
	.scenario = STREAM_READ_SCENARIO_N_ZERO_ST_NOT_NULL,
};

static const test_stream_read_case_t CASE_STREAM_READ_IO_ERROR_ST_NOT_NULL = {
	.name = "stream_read_io_error_st_not_null",
	.scenario = STREAM_READ_SCENARIO_IO_ERROR_ST_NOT_NULL,
};

static const test_stream_read_case_t CASE_STREAM_READ_EOF_ST_NOT_NULL = {
	.name = "stream_read_eof_st_not_null",
	.scenario = STREAM_READ_SCENARIO_EOF_ST_NOT_NULL,
};

static const test_stream_read_case_t CASE_STREAM_READ_OK = {
	.name = "stream_read_ok",
	.scenario = STREAM_READ_SCENARIO_OK,
};

#define STREAM_READ_CASES(X) \
X(CASE_STREAM_READ_N_NONZERO_S_NULL) \
X(CASE_STREAM_READ_N_NONZERO_BUF_NULL) \
X(CASE_STREAM_READ_N_ZERO_ST_NULL) \
X(CASE_STREAM_READ_N_ZERO_ST_NOT_NULL) \
X(CASE_STREAM_READ_IO_ERROR_ST_NOT_NULL) \
X(CASE_STREAM_READ_EOF_ST_NOT_NULL) \
X(CASE_STREAM_READ_OK)

#define MAKE_STREAM_READ_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_read, case_sym)

static const struct CMUnitTest stream_read_tests[] = {
	STREAM_READ_CASES(MAKE_STREAM_READ_TEST)
};

#undef STREAM_READ_CASES
#undef MAKE_STREAM_READ_TEST

/** @endcond */

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream_write()`.
 *
 * See contract:
 * - @ref specifications_stream_write "stream_write() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_unit_stream_write "stream_write() unit tests section"
 */
typedef enum {
	STREAM_WRITE_SCENARIO_N_NONZERO_S_NULL = 0,
	STREAM_WRITE_SCENARIO_N_NONZERO_BUF_NULL,
	STREAM_WRITE_SCENARIO_N_ZERO_ST_NULL,
	STREAM_WRITE_SCENARIO_N_ZERO_ST_NOT_NULL,
	STREAM_WRITE_SCENARIO_IO_ERROR_ST_NOT_NULL,
	STREAM_WRITE_SCENARIO_OK
} stream_write_scenario_t;

typedef struct {
	const char *name;
	stream_write_scenario_t scenario;
} test_stream_write_case_t;

typedef struct {
	stream_t *stream;
	stream_fake_adapter_backend_t fake_adapter_backend;
	const osal_mem_ops_t *mem;
	const test_stream_write_case_t *tc;
} test_stream_write_fixture_t;

static int setup_stream_write(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stream_write, state, tc, fx);

	stream_fake_adapter_init_backend(&fx->fake_adapter_backend);
	fx->mem = osal_mem_default_ops();
	stream_env_t env = stream_default_env(&stream_fake_adapter_vtbl, fx->mem);
	assert_int_equal(stream_create(&fx->stream, &env), STREAM_STATUS_OK);
	stream_inject_backend(fx->stream, (void *)&fx->fake_adapter_backend);
	assert_int_equal(stream_complete_default_init(fx->stream, NULL), STREAM_STATUS_OK);

	*state = fx;
	return 0;
}

static int teardown_stream_write(void **state)
{
	test_stream_write_fixture_t *fx = (test_stream_write_fixture_t *)(*state);
	stream_destroy(&fx->stream);
	osal_free(fx);
	return 0;
}

static void test_stream_write(void **state)
{
	test_stream_write_fixture_t *fx = (test_stream_write_fixture_t *)(*state);
	const test_stream_write_case_t *tc = fx->tc;

	// ARRANGE
	stream_t *s_arg = fx->stream;
	char buf[64] = {0};
	void *buf_arg = buf;
	size_t n_arg = 3;
	stream_status_t st = STREAM_STATUS_OK;
	stream_status_t *st_arg = &st;
	switch (tc->scenario) {
		case STREAM_WRITE_SCENARIO_N_NONZERO_S_NULL:
			s_arg = NULL;
			break;
		case STREAM_WRITE_SCENARIO_N_NONZERO_BUF_NULL:
			buf_arg = NULL;
			break;
		case STREAM_WRITE_SCENARIO_N_ZERO_ST_NULL:
			n_arg = 0;
			st_arg = NULL;
			break;
		case STREAM_WRITE_SCENARIO_N_ZERO_ST_NOT_NULL:
			n_arg = 0;
			break;
		case STREAM_WRITE_SCENARIO_IO_ERROR_ST_NOT_NULL:
			fx->fake_adapter_backend.next_write_st = STREAM_STATUS_IO_ERROR;
			fx->fake_adapter_backend.next_write_ret = 0;
			break;
		case STREAM_WRITE_SCENARIO_OK:
			fx->fake_adapter_backend.next_write_st = STREAM_STATUS_OK;
			fx->fake_adapter_backend.next_write_ret = 2;
			break;
		default: fail();
	}

	// ACT
	size_t ret = stream_write(s_arg, buf_arg, n_arg, st_arg);

	// ASSERT
	switch (tc->scenario) {
		case STREAM_WRITE_SCENARIO_N_NONZERO_S_NULL:
		case STREAM_WRITE_SCENARIO_N_NONZERO_BUF_NULL:
			assert_int_equal(st, STREAM_STATUS_INVALID);
			assert_true(ret == 0);
			assert_true(fx->fake_adapter_backend.write_call_count == 0);
			break;
		case STREAM_WRITE_SCENARIO_N_ZERO_ST_NULL:
			assert_true(ret == 0);
			assert_true(fx->fake_adapter_backend.write_call_count == 0);
			break;
		case STREAM_WRITE_SCENARIO_N_ZERO_ST_NOT_NULL:
			assert_int_equal(st, STREAM_STATUS_OK);
			assert_true(ret == 0);
			assert_true(fx->fake_adapter_backend.write_call_count == 0);
			break;
		case STREAM_WRITE_SCENARIO_IO_ERROR_ST_NOT_NULL:
			assert_int_equal(st, STREAM_STATUS_IO_ERROR);
			assert_true(ret == fx->fake_adapter_backend.next_write_ret);
			assert_true(fx->fake_adapter_backend.write_call_count == 1);
			assert_ptr_equal(fx->fake_adapter_backend.last_write_buf, buf_arg);
			assert_true(fx->fake_adapter_backend.last_write_n == n_arg);
			assert_ptr_equal(fx->fake_adapter_backend.last_write_backend, &fx->fake_adapter_backend);
			assert_true(fx->fake_adapter_backend.last_write_st == st_arg);
			break;
		case STREAM_WRITE_SCENARIO_OK:
			assert_int_equal(st, STREAM_STATUS_OK);
			assert_true(ret == fx->fake_adapter_backend.next_write_ret);
			assert_true(fx->fake_adapter_backend.write_call_count == 1);
			assert_ptr_equal(fx->fake_adapter_backend.last_write_buf, buf_arg);
			assert_true(fx->fake_adapter_backend.last_write_n == n_arg);
			assert_ptr_equal(fx->fake_adapter_backend.last_write_backend, &fx->fake_adapter_backend);
			assert_true(fx->fake_adapter_backend.last_write_st == st_arg);
			break;
		default: fail();
	}
}

static const test_stream_write_case_t CASE_STREAM_WRITE_N_NONZERO_S_NULL = {
	.name = "stream_write_n_nonzero_s_null",
	.scenario = STREAM_WRITE_SCENARIO_N_NONZERO_S_NULL,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_N_NONZERO_BUF_NULL = {
	.name = "stream_write_n_nonzero_buf_null",
	.scenario = STREAM_WRITE_SCENARIO_N_NONZERO_BUF_NULL,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_N_ZERO_ST_NULL = {
	.name = "stream_write_n_zero_st_null",
	.scenario = STREAM_WRITE_SCENARIO_N_ZERO_ST_NULL,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_N_ZERO_ST_NOT_NULL = {
	.name = "stream_write_n_zero_st_not_null",
	.scenario = STREAM_WRITE_SCENARIO_N_ZERO_ST_NOT_NULL,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_IO_ERROR_ST_NOT_NULL = {
	.name = "stream_write_io_error_st_not_null",
	.scenario = STREAM_WRITE_SCENARIO_IO_ERROR_ST_NOT_NULL,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_OK = {
	.name = "stream_write_ok",
	.scenario = STREAM_WRITE_SCENARIO_OK,
};

#define STREAM_WRITE_CASES(X) \
X(CASE_STREAM_WRITE_N_NONZERO_S_NULL) \
X(CASE_STREAM_WRITE_N_NONZERO_BUF_NULL) \
X(CASE_STREAM_WRITE_N_ZERO_ST_NULL) \
X(CASE_STREAM_WRITE_N_ZERO_ST_NOT_NULL) \
X(CASE_STREAM_WRITE_IO_ERROR_ST_NOT_NULL) \
X(CASE_STREAM_WRITE_OK)

#define MAKE_STREAM_WRITE_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_write, case_sym)

static const struct CMUnitTest stream_write_tests[] = {
	STREAM_WRITE_CASES(MAKE_STREAM_WRITE_TEST)
};

#undef STREAM_WRITE_CASES
#undef MAKE_STREAM_WRITE_TEST

/** @endcond */

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream_flush()`.
 *
 * See contract:
 * - @ref specifications_stream_flush "stream_flush() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_unit_stream_flush "stream_flush() unit tests section"
 */
typedef enum {
	STREAM_FLUSH_SCENARIO_S_NULL = 0,
	STREAM_FLUSH_SCENARIO_BACKEND_RETURNS_ERROR,
	STREAM_FLUSH_SCENARIO_OK
} stream_flush_scenario_t;

typedef struct {
	const char *name;
	stream_flush_scenario_t scenario;
} test_stream_flush_case_t;

typedef struct {
	stream_t *stream;
	stream_fake_adapter_backend_t fake_adapter_backend;
	const osal_mem_ops_t *mem;
	const test_stream_flush_case_t *tc;
} test_stream_flush_fixture_t;

static int setup_stream_flush(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stream_flush, state, tc, fx);

	stream_fake_adapter_init_backend(&fx->fake_adapter_backend);
	fx->mem = osal_mem_default_ops();
	stream_env_t env = stream_default_env(&stream_fake_adapter_vtbl, fx->mem);
	assert_int_equal(stream_create(&fx->stream, &env), STREAM_STATUS_OK);
	stream_inject_backend(fx->stream, (void *)&fx->fake_adapter_backend);
	assert_int_equal(stream_complete_default_init(fx->stream, NULL), STREAM_STATUS_OK);

	*state = fx;
	return 0;
}

static int teardown_stream_flush(void **state)
{
	test_stream_flush_fixture_t *fx = (test_stream_flush_fixture_t *)(*state);
	stream_destroy(&fx->stream);
	osal_free(fx);
	return 0;
}

static void test_stream_flush(void **state)
{
	test_stream_flush_fixture_t *fx = (test_stream_flush_fixture_t *)(*state);
	const test_stream_flush_case_t *tc = fx->tc;

	// ARRANGE
	stream_t *s_arg = fx->stream;
	switch (tc->scenario) {
		case STREAM_FLUSH_SCENARIO_S_NULL:
			s_arg = NULL;
			break;
		case STREAM_FLUSH_SCENARIO_BACKEND_RETURNS_ERROR:
			fx->fake_adapter_backend.next_flush_ret = STREAM_STATUS_IO_ERROR;
			break;
		case STREAM_FLUSH_SCENARIO_OK:
			fx->fake_adapter_backend.next_flush_ret = STREAM_STATUS_OK;
			break;
		default: fail();
	}

	// ACT
	stream_status_t ret = stream_flush(s_arg);

	// ASSERT
	switch (tc->scenario) {
		case STREAM_FLUSH_SCENARIO_S_NULL:
			assert_true(ret == STREAM_STATUS_INVALID);
			assert_true(fx->fake_adapter_backend.flush_call_count == 0);
			break;
		case STREAM_FLUSH_SCENARIO_BACKEND_RETURNS_ERROR:
			assert_true(ret == STREAM_STATUS_IO_ERROR);
			assert_true(fx->fake_adapter_backend.flush_call_count == 1);
			assert_ptr_equal(fx->fake_adapter_backend.last_flush_backend, &fx->fake_adapter_backend);
			break;
		case STREAM_FLUSH_SCENARIO_OK:
			assert_true(ret == STREAM_STATUS_OK);
			assert_true(fx->fake_adapter_backend.flush_call_count == 1);
			assert_ptr_equal(fx->fake_adapter_backend.last_flush_backend, &fx->fake_adapter_backend);
			break;
		default: fail();
	}
}

static const test_stream_flush_case_t CASE_STREAM_FLUSH_S_NULL = {
	.name = "stream_flush_s_null",
	.scenario = STREAM_FLUSH_SCENARIO_S_NULL,
};

static const test_stream_flush_case_t CASE_STREAM_BACKEND_RETURNS_ERROR = {
	.name = "stream_flush_backend_returns_error",
	.scenario = STREAM_FLUSH_SCENARIO_BACKEND_RETURNS_ERROR,
};

static const test_stream_flush_case_t CASE_STREAM_FLUSH_OK = {
	.name = "stream_flush_ok",
	.scenario = STREAM_FLUSH_SCENARIO_OK,
};

#define STREAM_FLUSH_CASES(X) \
X(CASE_STREAM_FLUSH_S_NULL) \
X(CASE_STREAM_BACKEND_RETURNS_ERROR) \
X(CASE_STREAM_FLUSH_OK)

#define MAKE_STREAM_FLUSH_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_flush, case_sym)

static const struct CMUnitTest stream_flush_tests[] = {
	STREAM_FLUSH_CASES(MAKE_STREAM_FLUSH_TEST)
};

#undef STREAM_FLUSH_CASES
#undef MAKE_STREAM_FLUSH_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	int failed = 0;
	failed += cmocka_run_group_tests(stream_read_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stream_write_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stream_flush_tests, NULL, NULL);
	return failed;
}
/** @endcond */
