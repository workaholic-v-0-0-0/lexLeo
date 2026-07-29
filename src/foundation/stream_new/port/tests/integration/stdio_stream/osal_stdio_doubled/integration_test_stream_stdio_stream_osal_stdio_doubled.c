/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file integration_test_stream_stdio_stream_osal_stdio_doubled.c
 * @ingroup stream_integration_tests
 * @brief Integration tests for `stream` with the `stdio_stream` adapter and a doubled OSAL stdio dependency.
 *
 * See also:
 * - @ref testing_foundation_stream_integration_stream_stdio_stream_osal_stdio_doubled
 *   "`stream` / `stdio_stream` integration tests with doubled OSAL stdio dependency page"
 * - @ref specifications_stream "stream specifications"
 * - @ref specifications_stdio_stream "stdio_stream specifications"
 */

#include "stdio_stream/cr/stdio_stream_cr_api.h"
#include "stream/cr/stream_cr_api.h"
#include "stream/borrowers/stream_borrowers_api.h"
#include "stream/owners/stream_owners_api.h"

#include "stdio_stream/tests/stdio_stream_white_box_tests_access.h"

#include "osal/mem/osal_mem.h"
#include "osal/stdio/test/osal_stdio_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/******************************************************************************************************************************************
 * @brief Smoke test for a `stream_t` backed by the `stdio_stream` adapter.
 *
 * See contracts:
 * - @ref specifications_stdio_stream_create_adapter_provider "stdio_stream_create_adapter_provider() specifications".
 * - @ref specifications_stream_create_factory "stream_create_factory() specifications".
 * - @ref specifications_stream_factory_add_adapter "stream_factory_add_adapter() specifications".
 * - @ref specifications_stream_create_standard_stream_creator "stream_create_standard_stream_creator() specifications".
 * - @ref specifications_stream_standard_stream_creator_create "stream_standard_stream_creator_create() specifications".
 * - @ref specifications_stream_read "stream_read() specifications".
 * - @ref specifications_stream_destroy "stream_destroy() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_integration_stream_stdio_stream_osal_stdio_doubled_smoke "`stream` / `stdio_stream` smoke tests section".
 */
static void test_stream_stdio_stream_smoke(void **state) {
	(void)state;

	// ARRANGE

	fake_stdio_reset();
	uint8_t message_to_read[FAKE_STDIO_BUF_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
	size_t message_to_read_len = 6;
	fake_stdio_set_buffered_backing(&g_fake_stdio_stdin, message_to_read, message_to_read_len);
	const osal_stdio_ops_t *stdio_ops = osal_stdio_test_fake_ops();

	const osal_mem_ops_t *mem_ops = osal_mem_default_ops();

	stream_adapter_id_t stdio_stream_id = "stdio";

	stream_adapter_provider_t *stdio_stream_adapter_provider = NULL;

	stream_t *stream_backed_by_stdio_stream = NULL;

	size_t stream_read_ret = 0;
	uint8_t buf_read_arg[FAKE_STDIO_BUF_SIZE] = {0};
	size_t n_read_arg = 5;
	uint8_t expected_read_buffer_content[FAKE_STDIO_BUF_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05};

	stream_factory_t *stream_factory = NULL;
	stream_standard_stream_creator_t *stream_standard_stream_creator = NULL;

	stdio_stream_status_t stdio_stream_status = STDIO_STREAM_STATUS_OK;
	stream_factory_status_t stream_factory_status = STREAM_FACTORY_STATUS_OK;
	stream_status_t stream_status = STREAM_STATUS_OK;


	// ACT and ASSERT

	stdio_stream_cfg_t stdio_stream_cfg = stdio_stream_default_cfg();
	stdio_stream_env_t stdio_stream_env = stdio_stream_default_env(stdio_ops, mem_ops);
	stdio_stream_status = stdio_stream_create_adapter_provider(&stdio_stream_adapter_provider, &stdio_stream_cfg, &stdio_stream_env);
	assert_int_equal(stdio_stream_status, STDIO_STREAM_STATUS_OK);

	stream_factory_cfg_t stream_factory_cfg = stream_default_factory_cfg();
	stream_factory_status = stream_create_factory(&stream_factory, &stream_factory_cfg, mem_ops);
	assert_int_equal(stream_factory_status, STREAM_FACTORY_STATUS_OK);
	stream_factory_status = stream_factory_add_adapter(stream_factory, stdio_stream_id, stdio_stream_adapter_provider);
	assert_int_equal(stream_factory_status, STREAM_FACTORY_STATUS_OK);

	stream_factory_status =
		stream_create_standard_stream_creator(
			&stream_standard_stream_creator,
			stream_factory,
			stdio_stream_id,
			mem_ops
		);
	assert_int_equal(stream_factory_status, STREAM_FACTORY_STATUS_OK);

	stream_status =
		stream_standard_stream_creator_create(
			stream_standard_stream_creator,
			STREAM_STANDARD_STREAM_KIND_STDIN,
			&stream_backed_by_stdio_stream
		);
	assert_int_equal(stream_status, STREAM_STATUS_OK);
	assert_true(get_stdin_call_count == 1);

	stream_read_ret =
		stream_read(
			stream_backed_by_stdio_stream,
			buf_read_arg,
			n_read_arg,
			&stream_status
		);
	assert_int_equal(stream_status, STREAM_STATUS_OK);
	assert_true(stream_read_ret == n_read_arg);
	assert_int_equal(g_fake_stdio_stdin.read_call_count, 1);
	assert_ptr_equal(g_fake_stdio_stdin.last_read_ptr, buf_read_arg);
	assert_int_equal(g_fake_stdio_stdin.last_read_size, 1);
	assert_int_equal(g_fake_stdio_stdin.last_read_nmemb, n_read_arg);
	assert_ptr_equal(g_fake_stdio_stdin.last_read_stream, fake_stdio_to_osal_stdio_stream(&g_fake_stdio_stdin));
	assert_memory_equal(buf_read_arg, expected_read_buffer_content, n_read_arg);

	stream_destroy(&stream_backed_by_stdio_stream);
	assert_null(stream_backed_by_stdio_stream);

	stream_destroy_standard_stream_creator(&stream_standard_stream_creator);
	stream_destroy_factory(&stream_factory);
}

/******************************************************************************************************************************************
 * @brief Test scenarios for `stream_read()` with a `stream_t` backed by the `stdio_stream` adapter.
 *
 * See contracts:
 * - @ref specifications_stream_read "stream_read() specifications".
 * - @ref specifications_stdio_stream_read "stdio_stream_read() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_integration_stream_stdio_stream_osal_stdio_doubled_read "`stream_read()` / `stdio_stream` integration tests section".
 */
typedef enum {
	STREAM_READ_STDIO_STREAM_SCENARIO_N_NONZERO_S_NULL = 0,
	STREAM_READ_STDIO_STREAM_SCENARIO_N_NONZERO_BUF_NULL,
	STREAM_READ_STDIO_STREAM_SCENARIO_STDOUT_IO_ERROR,
	STREAM_READ_STDIO_STREAM_SCENARIO_OK_N_ZERO_S_NULL,
	STREAM_READ_STDIO_STREAM_SCENARIO_OK_N_EXCEEDING,
	STREAM_READ_STDIO_STREAM_SCENARIO_OK_N_FIT,
	STREAM_READ_STDIO_STREAM_SCENARIO_OK_NO_MORE_TO_READ,
	STREAM_READ_STDIO_STREAM_SCENARIO_OK_STILL_TO_READ,
	STREAM_READ_STDIO_STREAM_SCENARIO_OK_TWO_READS_STILL_TO_READ,
	STREAM_READ_STDIO_STREAM_SCENARIO_OK_TWO_READS_EXACTLY_TO_EOF,
	STREAM_READ_STDIO_STREAM_SCENARIO_OK_TWO_READS_SECOND_EXCEEDING,
	STREAM_READ_STDIO_STREAM_SCENARIO_OK_TWO_READS_SECOND_AT_EOF,
} stream_read_stdio_stream_scenario_t;

/** @cond INTERNAL */

typedef struct {

	const char *name;
	stream_read_stdio_stream_scenario_t scenario;

	stream_standard_stream_kind_t kind;

	const uint8_t fake_stdio_buffered_content[FAKE_STDIO_BUF_SIZE];
	size_t fake_stdio_buffered_len;
	size_t fake_stdio_initial_read_pos;

	bool arg_s_is_null;
	bool arg_buf_is_null;
	const uint8_t initial_arg_buf_content[FAKE_STDIO_BUF_SIZE];
	size_t first_read_arg_n;
	stream_status_t initial_arg_st_content;

	size_t expected_first_read_ret;
	uint8_t expected_buf_after_first_read[FAKE_STDIO_BUF_SIZE];
	stream_status_t expected_stream_status_after_first_read;

	bool second_read;
	size_t second_read_arg_n;

	size_t expected_second_read_ret;
	uint8_t expected_buf_after_second_read[FAKE_STDIO_BUF_SIZE];
	stream_status_t expected_stream_status_after_second_read;

} test_stream_read_stdio_stream_case_t;

typedef struct {
	stream_t *stream;
	fake_stdio_t *fake_stdio;
	uint8_t arg_buf[FAKE_STDIO_BUF_SIZE];
	stream_status_t arg_st_content;

	const test_stream_read_stdio_stream_case_t *tc;
} test_stream_read_stdio_stream_fixture_t;

static int setup_stream_read_stdio_stream(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(stream_read_stdio_stream, state, tc, fx);

	fake_stdio_reset();
	switch (tc->kind) {
		case STREAM_STANDARD_STREAM_KIND_STDIN:
			fx->fake_stdio = &g_fake_stdio_stdin;
			break;
		case STREAM_STANDARD_STREAM_KIND_STDOUT:
			fx->fake_stdio = &g_fake_stdio_stdout;
			break;
		case STREAM_STANDARD_STREAM_KIND_STDERR:
			fx->fake_stdio = &g_fake_stdio_stderr;
			break;
		default:
			fail();
	}
	const osal_stdio_ops_t *fake_stdio_ops = osal_stdio_test_fake_ops();
	const osal_mem_ops_t *mem_ops = osal_mem_default_ops();
	stdio_stream_t *stdio_stream = NULL;
	stdio_stream_env_t stdio_stream_env = stdio_stream_default_env(fake_stdio_ops, mem_ops);
	assert_int_equal(stdio_stream_create(&stdio_stream, &stdio_stream_env), STDIO_STREAM_STATUS_OK);
	/* No stdio_stream_inject_stdio() call is needed because the fake stdio_ops::get_std*() functions initialize the stdio field with the
	 * corresponding fake standard stream. */
	stdio_stream_cfg_t stdio_stream_cfg = stdio_stream_default_cfg();
	stream_standard_stream_creator_args_t stream_standard_stream_creator_args = { .kind = tc->kind };
	assert_int_equal(stdio_stream_complete_default_init(stdio_stream, &stdio_stream_cfg, &stream_standard_stream_creator_args), STDIO_STREAM_STATUS_OK);
	const stream_env_t stream_env = stream_default_env(stdio_stream_vtbl(), mem_ops);
	assert_int_equal(stream_create(&fx->stream, &stream_env), STREAM_STATUS_OK);
	assert_int_equal(stream_complete_default_init(fx->stream, (void *)stdio_stream), STREAM_STATUS_OK);
	fake_stdio_set_buffered_backing(
		fx->fake_stdio,
		tc->fake_stdio_buffered_content,
		tc->fake_stdio_buffered_len);
	fx->fake_stdio->read_pos = tc->fake_stdio_initial_read_pos;

	*state = fx;
	return 0;
}

static int teardown_stream_read_stdio_stream(void **state)
{
	test_stream_read_stdio_stream_fixture_t *fx = (test_stream_read_stdio_stream_fixture_t *)(*state);
	stream_destroy(&fx->stream);
	osal_free(fx);
	return 0;
}

static void test_stream_read_stdio_stream(void **state) {
	test_stream_read_stdio_stream_fixture_t *fx = (test_stream_read_stdio_stream_fixture_t *)(*state);
	const test_stream_read_stdio_stream_case_t *tc = fx->tc;

	// ARRANGE

	size_t ret = 0;
	osal_memcpy(fx->arg_buf, tc->initial_arg_buf_content, FAKE_STDIO_BUF_SIZE);
	fx->arg_st_content = tc->initial_arg_st_content;
	stream_t *arg_s = fx->stream;
	if (tc->arg_s_is_null) {
		arg_s = NULL;
	}
	void *arg_buf = fx->arg_buf;
	if (tc->arg_buf_is_null) {
		arg_buf = NULL;
	}

	// ACT
	ret = stream_read(arg_s, arg_buf, tc->first_read_arg_n, &fx->arg_st_content);

	// ASSERT
	assert_int_equal(ret, tc->expected_first_read_ret);
	assert_int_equal(fx->arg_st_content, tc->expected_stream_status_after_first_read);
	assert_memory_equal(fx->arg_buf, tc->expected_buf_after_first_read, FAKE_STDIO_BUF_SIZE);

	// ARRANGE
	if (!tc->second_read) {
		return;
	}
	osal_memset(fx->arg_buf, 0, FAKE_STDIO_BUF_SIZE);

	// ACT
	ret = stream_read(fx->stream, fx->arg_buf, tc->second_read_arg_n, &fx->arg_st_content);

	// ASSERT
	assert_int_equal(ret, tc->expected_second_read_ret);
	assert_int_equal(fx->arg_st_content, tc->expected_stream_status_after_second_read);
	assert_memory_equal(fx->arg_buf, tc->expected_buf_after_second_read, FAKE_STDIO_BUF_SIZE);
}

static const test_stream_read_stdio_stream_case_t CASE_STREAM_READ_STDIO_STREAM_N_NONZERO_S_NULL = {

	.name = "stream_read_stdio_stream_n_non_zero_s_null",
	.scenario = STREAM_READ_STDIO_STREAM_SCENARIO_N_NONZERO_S_NULL,

	.kind = STREAM_STANDARD_STREAM_KIND_STDIN,

	.fake_stdio_buffered_content ={0x0a, 0x0b, 0x0c},
	.fake_stdio_buffered_len = 3,
	.fake_stdio_initial_read_pos = 2,

	.arg_s_is_null = true,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0x01, 0x02, 0x03},
	.first_read_arg_n = 1,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 0,
	.expected_buf_after_first_read = {0x01, 0x02, 0x03},
	.expected_stream_status_after_first_read = STREAM_STATUS_INVALID,

	.second_read = false
};

static const test_stream_read_stdio_stream_case_t CASE_STREAM_READ_STDIO_STREAM_N_NONZERO_BUF_NULL = {

	.name = "stream_read_stdio_stream_n_non_zero_buf_null",
	.scenario = STREAM_READ_STDIO_STREAM_SCENARIO_N_NONZERO_BUF_NULL,

	.kind = STREAM_STANDARD_STREAM_KIND_STDIN,

	.fake_stdio_buffered_content ={0x0a, 0x0b, 0x0c},
	.fake_stdio_buffered_len = 3,
	.fake_stdio_initial_read_pos = 2,

	.arg_s_is_null = false,
	.arg_buf_is_null = true,
	.initial_arg_buf_content = {0x01, 0x02, 0x03},
	.first_read_arg_n = 1,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 0,
	.expected_buf_after_first_read = {0x01, 0x02, 0x03},
	.expected_stream_status_after_first_read = STREAM_STATUS_INVALID,

	.second_read = false
};

static const test_stream_read_stdio_stream_case_t CASE_STREAM_READ_STDIO_STREAM_STDOUT_IO_ERROR = {

	.name = "stream_read_stdio_stream_stdout_io_error",
	.scenario = STREAM_READ_STDIO_STREAM_SCENARIO_STDOUT_IO_ERROR,

	.kind = STREAM_STANDARD_STREAM_KIND_STDOUT,

	.fake_stdio_buffered_content ={0x0a, 0x0b, 0x0c},
	.fake_stdio_buffered_len = 3,
	.fake_stdio_initial_read_pos = 2,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0x01, 0x02, 0x03},
	.first_read_arg_n = 1,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 0,
	.expected_buf_after_first_read = {0x01, 0x02, 0x03},
	.expected_stream_status_after_first_read = STREAM_STATUS_IO_ERROR,

	.second_read = false
};

static const test_stream_read_stdio_stream_case_t CASE_STREAM_READ_STDIO_STREAM_OK_N_ZERO_S_NULL = {

	.name = "stream_read_stdio_stream_ok_n_zero_s_null",
	.scenario = STREAM_READ_STDIO_STREAM_SCENARIO_OK_N_ZERO_S_NULL,

	.kind = STREAM_STANDARD_STREAM_KIND_STDIN,

	.fake_stdio_buffered_content ={0x0a, 0x0b, 0x0c},
	.fake_stdio_buffered_len = 3,
	.fake_stdio_initial_read_pos = 2,

	.arg_s_is_null = true,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0x01, 0x02, 0x03},
	.first_read_arg_n = 0,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 0,
	.expected_buf_after_first_read = {0x01, 0x02, 0x03},
	.expected_stream_status_after_first_read = STREAM_STATUS_OK,

	.second_read = false
};

static const test_stream_read_stdio_stream_case_t CASE_STREAM_READ_STDIO_STREAM_OK_N_EXCEEDING = {

	.name = "stream_read_stdio_stream_ok_n_exceeding",
	.scenario = STREAM_READ_STDIO_STREAM_SCENARIO_OK_N_EXCEEDING,

	.kind = STREAM_STANDARD_STREAM_KIND_STDIN,

	.fake_stdio_buffered_content ={0x0a, 0x0b, 0x0c},
	.fake_stdio_buffered_len = 3,
	.fake_stdio_initial_read_pos = 2,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0},
	.first_read_arg_n = 2,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 1,
	.expected_buf_after_first_read = {0x0c},
	.expected_stream_status_after_first_read = STREAM_STATUS_OK,

	.second_read = false
};

static const test_stream_read_stdio_stream_case_t CASE_STREAM_READ_STDIO_STREAM_OK_N_FIT = {

	.name = "stream_read_stdio_stream_ok_n_fit",
	.scenario = STREAM_READ_STDIO_STREAM_SCENARIO_OK_N_FIT,

	.kind = STREAM_STANDARD_STREAM_KIND_STDIN,

	.fake_stdio_buffered_content ={0x0a, 0x0b, 0x0c},
	.fake_stdio_buffered_len = 3,
	.fake_stdio_initial_read_pos = 2,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0},
	.first_read_arg_n = 1,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 1,
	.expected_buf_after_first_read = {0x0c},
	.expected_stream_status_after_first_read = STREAM_STATUS_OK,

	.second_read = false
};

static const test_stream_read_stdio_stream_case_t CASE_STREAM_READ_STDIO_STREAM_OK_NO_MORE_TO_READ = {

	.name = "stream_read_stdio_stream_ok_no_more_to_read",
	.scenario = STREAM_READ_STDIO_STREAM_SCENARIO_OK_NO_MORE_TO_READ,

	.kind = STREAM_STANDARD_STREAM_KIND_STDIN,

	.fake_stdio_buffered_content ={0x0a, 0x0b, 0x0c},
	.fake_stdio_buffered_len = 3,
	.fake_stdio_initial_read_pos = 3,

	.arg_s_is_null = false,
	.arg_buf_is_null = false,
	.initial_arg_buf_content = {0x01, 0x02, 0x03},
	.first_read_arg_n = 7,
	.initial_arg_st_content = STREAM_STATUS_OOM,

	.expected_first_read_ret = 0,
	.expected_buf_after_first_read = {0x01, 0x02, 0x03},
	.expected_stream_status_after_first_read = STREAM_STATUS_OK,

	.second_read = false
};

//<here>This is wrong because of eof behavior that is not the one of stdin of standard library ; so adjust and adjust the tests too

#define STREAM_READ_STDIO_STREAM_CASES(X) \
X(CASE_STREAM_READ_STDIO_STREAM_N_NONZERO_S_NULL) \
X(CASE_STREAM_READ_STDIO_STREAM_N_NONZERO_BUF_NULL) \
X(CASE_STREAM_READ_STDIO_STREAM_STDOUT_IO_ERROR) \
X(CASE_STREAM_READ_STDIO_STREAM_OK_N_ZERO_S_NULL) \
X(CASE_STREAM_READ_STDIO_STREAM_OK_N_EXCEEDING) \
X(CASE_STREAM_READ_STDIO_STREAM_OK_NO_MORE_TO_READ)

#define MAKE_STREAM_READ_STDIO_STREAM_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_read_stdio_stream, case_sym)

static const struct CMUnitTest stream_read_stdio_stream_tests[] = {
	STREAM_READ_STDIO_STREAM_CASES(MAKE_STREAM_READ_STDIO_STREAM_TEST)
};

#undef STREAM_READ_STDIO_STREAM_CASES
#undef MAKE_STREAM_READ_STDIO_STREAM_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	static const struct CMUnitTest stream_stdio_stream_integration_tests_non_parametric[] = {
		cmocka_unit_test(test_stream_stdio_stream_smoke)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(stream_stdio_stream_integration_tests_non_parametric, NULL, NULL);
	failed += cmocka_run_group_tests(stream_read_stdio_stream_tests, NULL, NULL);

	return failed;
}
/** @endcond */
