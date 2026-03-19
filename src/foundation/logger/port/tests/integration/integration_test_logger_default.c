/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file integration_test_logger_default.c
 * @ingroup logger_integration_tests
 * @brief Integration tests for the `logger` port with the `logger_default` adapter.
 *
 * @details
 * This file implements integration-level validation of the `logger` public API
 * when wired with the concrete `logger_default` adapter.
 *
 * Covered surfaces:
 * - `logger_log()`
 * - `logger_destroy()`
 *
 * Test strategy:
 * - create a concrete logger through `logger_default_create_logger()`
 * - exercise `logger_log()` through the public `logger` API
 * - observe runtime behavior through the injected fake stream
 * - release created logger handles through `logger_destroy()`
 *
 * See also:
 * - @ref testing_foundation_logger_integration "logger integration tests page"
 * - @ref specifications_logger "logger specifications"
 * - @ref specifications_logger_default "logger_default specifications"
 */

#include "logger_default/cr/logger_default_cr_api.h"
#include "logger/borrowers/logger.h"
#include "logger/borrowers/logger_types.h"
#include "logger/lifecycle/logger_lifecycle.h"
#include "logger/cr/logger_cr_api.h"
#include "stream/borrowers/stream_types.h"
#include "stream/test/stream_fake_provider.h"
#include "osal/time/test/osal_time_fake_provider.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "lexleo_cmocka_xmacro_helpers.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Scenarios for `logger_log()` with a `logger_default`-backed logger.
 *
 * logger_status_t logger_log(logger_t *logger, const char *msg);
 *
 * Doubles:
 * - fake_stream
 * - fake_time
 * - fake_memory
 *
 * Isolation:
 * - the public `logger` API is exercised end-to-end
 * - runtime stream behavior is controlled through `fake_stream`
 *
 * See also:
 * - @ref testing_foundation_logger_integration_logger_default "logger/log with logger_default integration section"
 * - @ref specifications_logger_log "logger_log() specifications"
 * - @ref specifications_logger_default_create_logger "logger_default_create_logger() specifications"
 *
 * The scenarios below define the integration-test oracle for `logger_log()`
 * when used with the `logger_default` adapter.
 */
typedef enum {
	/**
	 * WHEN a `logger_default`-backed logger is created with `append_newline == false`
	 * AND the injected time service returns epoch time `0`
	 * AND `logger_log(logger, "abc")` is called
	 * EXPECT:
	 * - returns `LOGGER_STATUS_OK`
	 * - the target stream receives `"[1970-01-01 00:00:00 UTC+0] abc"`
	 */
	LOGGER_LOGGER_DEFAULT_SCENARIO_OK_NO_NEWLINE = 0,

	/**
	 * WHEN a `logger_default`-backed logger is created
	 * AND the injected time service returns epoch time `0`
	 * AND `logger_log(logger, NULL)` is called
	 * EXPECT:
	 * - returns `LOGGER_STATUS_INVALID`
	 */
	LOGGER_LOGGER_DEFAULT_SCENARIO_MESSAGE_NULL,

	/**
	 * WHEN a `logger_default`-backed logger is created with `append_newline == true`
	 * AND the injected time service returns epoch time `0`
	 * AND `logger_log(logger, "abc")` is called
	 * EXPECT:
	 * - returns `LOGGER_STATUS_OK`
	 * - the target stream receives `"[1970-01-01 00:00:00 UTC+0] abc\n"`
	 */
	LOGGER_LOGGER_DEFAULT_SCENARIO_OK_APPEND_NEWLINE,

	/**
	 * WHEN the underlying stream write operation returns `STREAM_STATUS_IO_ERROR`
	 * during `logger_log()`
	 * EXPECT:
	 * - returns `LOGGER_STATUS_IO_ERROR`
	 */
	LOGGER_LOGGER_DEFAULT_SCENARIO_STREAM_WRITE_FAIL,

	/**
	 * WHEN the injected time service returns `OSAL_TIME_STATUS_ERROR`
	 * during `logger_log()`
	 * AND `logger_log(logger, "abc")` is called
	 * EXPECT:
	 * - returns `LOGGER_STATUS_OK`
	 * - the target stream receives `"[timestamp error] abc"`
	 */
	LOGGER_LOGGER_DEFAULT_SCENARIO_TIME_OPS_FAIL,
} logger_logger_default_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;

	// arrange
	logger_logger_default_scenario_t scenario;
	const char *message;
	stream_status_t write_ret;
	osal_time_status_t time_ops_status;

	// assert
	logger_status_t expected_ret;
	bool write_call;
	size_t expected_written_len;
	const char *expected_written_message;
} test_logger_default_log_case_t;

typedef struct {
	logger_t *logger;
	logger_env_t logger_env;
	logger_default_env_t logger_default_env;
	stream_fake_t *fake_stream_adapter;
	stream_t *fake_stream;
	const test_logger_default_log_case_t *tc;
} test_logger_default_log_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_logger_default_log(void **state)
{
	const test_logger_default_log_case_t *tc =
		(const test_logger_default_log_case_t *) (*state);

	test_logger_default_log_fixture_t *fx = malloc(sizeof(*fx));
	if (!fx) return -1;
	memset(fx, 0, sizeof(*fx));

	fx->tc = tc;

	fake_memory_reset();
	fake_time_reset();

	fake_time_set_now_status(OSAL_TIME_STATUS_OK);
	fake_time_set_now_out((osal_time_t) { .epoch_seconds = 0 });

	assert_int_equal(
		stream_fake_create(
			&fx->fake_stream_adapter,
			&fx->fake_stream,
			osal_mem_test_fake_ops()),
		STREAM_STATUS_OK
	);

	stream_fake_reset(fx->fake_stream_adapter);

	bool append_newline =
		(tc->scenario == LOGGER_LOGGER_DEFAULT_SCENARIO_OK_APPEND_NEWLINE);
	const logger_default_cfg_t logger_default_cfg =
		(logger_default_cfg_t) { .append_newline = append_newline };

	fx->logger_env.mem = osal_mem_test_fake_ops();
	fx->logger_default_env.stream = fx->fake_stream;
	fx->logger_default_env.time_ops = osal_time_test_fake_ops();
	fx->logger_default_env.adapter_mem = osal_mem_test_fake_ops();
	fx->logger_default_env.port_env = fx->logger_env;

	assert_int_equal(
		logger_default_create_logger(
			&fx->logger,
			&logger_default_cfg,
			&fx->logger_default_env),
		LOGGER_STATUS_OK
	);

	*state = fx;

	return 0;
}

static int teardown_logger_default_log(void **state)
{
	test_logger_default_log_fixture_t *fx =
		(test_logger_default_log_fixture_t *) (*state);

	logger_destroy(&fx->logger);
	stream_fake_destroy(&fx->fake_stream_adapter, &fx->fake_stream);

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	free(fx);

	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

static void test_logger_default_log(void **state)
{
	test_logger_default_log_fixture_t *fx =
		(test_logger_default_log_fixture_t *) (*state);

	// ARRANGE
	logger_status_t ret = (logger_status_t) -1; // poison

	if (fx->tc->scenario == LOGGER_LOGGER_DEFAULT_SCENARIO_STREAM_WRITE_FAIL) {
		stream_fake_fail_write_since(
			fx->fake_stream_adapter,
			1,
			fx->tc->write_ret
		);
	}

	if (fx->tc->scenario == LOGGER_LOGGER_DEFAULT_SCENARIO_TIME_OPS_FAIL) {
		fake_time_set_now_status(fx->tc->time_ops_status);
	}

	// ACT
	ret = logger_log(fx->logger, fx->tc->message);

	// ASSERT
	assert_int_equal(ret, fx->tc->expected_ret);
	assert_true(
		fx->tc->write_call
		==
		(stream_fake_counters(fx->fake_stream_adapter)->write_calls > 0)
	);
	if (fx->tc->expected_written_message) {
		assert_int_equal(
			fx->tc->expected_written_len,
			stream_fake_written_len(fx->fake_stream_adapter)
		);
		assert_memory_equal(
			stream_fake_written_data(fx->fake_stream_adapter),
			fx->tc->expected_written_message,
			fx->tc->expected_written_len
		);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_logger_default_log_case_t CASE_LOGGER_DEFAULT_LOG_MESSAGE_NULL = {
	.name = "logger_default_log_message_null",
	.scenario = LOGGER_LOGGER_DEFAULT_SCENARIO_MESSAGE_NULL,
	.message = NULL,

	.expected_ret = LOGGER_STATUS_INVALID,
	.write_call = false,

	.expected_written_len = 0,
	.expected_written_message = NULL
};

static const test_logger_default_log_case_t CASE_LOGGER_DEFAULT_LOG_OK_NO_NEWLINE = {
	.name = "logger_default_log_ok_no_newline",
	.scenario = LOGGER_LOGGER_DEFAULT_SCENARIO_OK_NO_NEWLINE,
	.message = "abc",
	.write_ret = STREAM_STATUS_OK,
	.time_ops_status = OSAL_TIME_STATUS_OK,

	.expected_ret = LOGGER_STATUS_OK,
	.write_call = true,
	.expected_written_len = strlen("[1970-01-01 00:00:00 UTC+0] abc"),
	.expected_written_message = "[1970-01-01 00:00:00 UTC+0] abc"
};

static const test_logger_default_log_case_t CASE_LOGGER_DEFAULT_LOG_OK_APPEND_NEWLINE = {
	.name = "logger_default_log_ok_newline",
	.scenario = LOGGER_LOGGER_DEFAULT_SCENARIO_OK_APPEND_NEWLINE,
	.message = "abc",
	.write_ret = STREAM_STATUS_OK,
	.time_ops_status = OSAL_TIME_STATUS_OK,

	.expected_ret = LOGGER_STATUS_OK,
	.write_call = true,
	.expected_written_len = strlen("[1970-01-01 00:00:00 UTC+0] abc\n"),
	.expected_written_message = "[1970-01-01 00:00:00 UTC+0] abc\n"
};

static const test_logger_default_log_case_t CASE_LOGGER_DEFAULT_LOG_WRITE_FAIL = {
	.name = "logger_default_log_write_fail",
	.scenario = LOGGER_LOGGER_DEFAULT_SCENARIO_STREAM_WRITE_FAIL,
	.message = "abc",
	.write_ret = STREAM_STATUS_IO_ERROR,
	.time_ops_status = OSAL_TIME_STATUS_OK,

	.expected_ret = LOGGER_STATUS_IO_ERROR,
	.write_call = true,
	.expected_written_len = 0,
	.expected_written_message = NULL
};

static const test_logger_default_log_case_t CASE_LOGGER_DEFAULT_LOG_TIME_OPS_FAIL = {
	.name = "logger_default_log_time_ops_fail",
	.scenario = LOGGER_LOGGER_DEFAULT_SCENARIO_TIME_OPS_FAIL,
	.message = "abc",
	.write_ret = STREAM_STATUS_OK,
	.time_ops_status = OSAL_TIME_STATUS_ERROR,

	.expected_ret = LOGGER_STATUS_OK,
	.write_call = true,
	.expected_written_len = strlen("[timestamp error] abc"),
	.expected_written_message = "[timestamp error] abc"
};

/** @endcond */

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define LOGGER_DEFAULT_LOG_CASES(X) \
X(CASE_LOGGER_DEFAULT_LOG_MESSAGE_NULL) \
X(CASE_LOGGER_DEFAULT_LOG_OK_NO_NEWLINE) \
X(CASE_LOGGER_DEFAULT_LOG_OK_APPEND_NEWLINE) \
X(CASE_LOGGER_DEFAULT_LOG_WRITE_FAIL) \
X(CASE_LOGGER_DEFAULT_LOG_TIME_OPS_FAIL)

#define LOGGER_MAKE_LOGGER_DEFAULT_LOG_TEST(case_sym) \
LEXLEO_MAKE_TEST(logger_default_log, case_sym)

static const struct CMUnitTest logger_default_log_tests[] = {
	LOGGER_DEFAULT_LOG_CASES(LOGGER_MAKE_LOGGER_DEFAULT_LOG_TEST)
};

#undef LOGGER_DEFAULT_LOG_CASES
#undef LOGGER_MAKE_LOGGER_DEFAULT_LOG_TEST

/** @endcond */

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

/** @cond INTERNAL */

int main(void) {
	int failed = 0;
	failed += cmocka_run_group_tests(logger_default_log_tests, NULL, NULL);

	return failed;
}

/** @endcond */

