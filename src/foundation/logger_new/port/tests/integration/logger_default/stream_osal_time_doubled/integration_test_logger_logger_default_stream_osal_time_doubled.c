/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file integration_test_logger_logger_default_stream_osal_time_doubled.c
 * @ingroup logger_integration_tests
 * @brief Integration tests for `logger` with the `logger_default` adapter and a doubled stream and a doubled osal_time dependency.
 *
 * See also:
 * - @ref testing_foundation_logger_integration_logger_logger_default_stream_osal_time_doubled
 *   "`logger` / `logger_default` integration tests with doubled stream and doubled osal_time dependency page"
 * - @ref specifications_logger "logger specifications"
 * - @ref specifications_logger_default "logger_default specifications"
 */

#include "logger_default/cr/logger_default_cr_api.h"
#include "logger/cr/logger_cr_api.h"
#include "logger/borrowers/logger_borrowers_api.h"

#include "lexleo/test/fake_stream.h"

#include "osal/mem/osal_mem.h"
#include "osal/time/test/osal_time_fake_provider.h"

#include "osal/str/osal_str.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/******************************************************************************************************************************************
 * @brief Smoke test for a `logger_t` backed by the `logger_default` adapter.
 *
 * See contracts:
 * - @ref specifications_logger "logger specifications".
 * - @ref specifications_logger_default "logger_default specifications".
 *
 * See test description:
 * - @ref testing_foundation_logger_integration_logger_logger_default_stream_osal_time_doubled_smoke "`logger` / `logger_default` smoke tests section".
 */
static void test_logger_logger_default_smoke(void **state) {
	(void)state;

	// ARRANGE

	fake_stream_reset();
	fake_stream_adapter_t fake_stream_adapter;
	fake_stream_adapter_reset_instance(&fake_stream_adapter);
	fake_stream_adapter.write_all = true;
	fake_stream_t *fake_stream = fake_stream_create_instance(&fake_stream_adapter);

	fake_time_reset();
	const osal_time_ops_t *fake_time_ops = osal_time_test_fake_ops();
	fake_time_set_now_status(OSAL_TIME_STATUS_OK);
	fake_time_set_now_out((osal_time_t) { .epoch_seconds = 0 }); /* Unix epoch */

	const osal_mem_ops_t *real_mem_ops = osal_mem_default_ops();

	const logger_vtbl_t *logger_adapter_vtbl = logger_default_vtbl();
	logger_t *logger = NULL;
	logger_default_t *logger_default = NULL;
	const char *message_to_be_logged = "It is the Unix epoch now!";
	const char *expected_logger_stream_sink_content = "[1970-01-01 00:00:00 UTC+0] It is the Unix epoch now!\n";
	size_t expected_logger_stream_sink_content_len = osal_strlen(expected_logger_stream_sink_content);

	// ACT and ASSERT

	logger_env_t logger_env = logger_default_env(logger_adapter_vtbl, real_mem_ops);
	assert_int_equal(
		logger_create(&logger, &logger_env),
		LOGGER_STATUS_OK
	);
	logger_default_env_t logger_default_env = logger_default_default_env(fake_stream, fake_time_ops, real_mem_ops);
	assert_int_equal(
		logger_default_create(&logger_default, &logger_default_env),
		LOGGER_DEFAULT_STATUS_OK
	);
	logger_default_cfg_t logger_default_cfg = logger_default_default_cfg();
	assert_int_equal(
		logger_default_complete_default_init(logger_default, &logger_default_cfg),
		LOGGER_DEFAULT_STATUS_OK
	);
	assert_int_equal(
		logger_complete_default_init(logger, logger_default),
		LOGGER_STATUS_OK
	);

	assert_int_equal(
		logger_log(logger, message_to_be_logged),
		LOGGER_STATUS_OK
	);

	assert_int_equal(
		logger_destroy(&logger),
		LOGGER_STATUS_OK
	);

	// ASSERT

	assert_true(fake_stream_adapter.write_call_count > 0);
	assert_true(fake_stream_adapter.flush_call_count > 0);
	assert_int_equal(fake_stream_adapter.close_call_count, 0);
	assert_memory_equal(
		fake_stream_adapter.sink_backing,
		expected_logger_stream_sink_content,
		expected_logger_stream_sink_content_len
	);
	assert_int_equal(fake_stream_adapter.sink_len, expected_logger_stream_sink_content_len);
}

/******************************************************************************************************************************************
 * @brief Test scenarios for `logger_log()` with a `logger_t` backed by the `logger_default` adapter.
 *
 * See contracts:
 * - @ref specifications_logger_log "logger_log() specifications".
 * - @ref specifications_logger_default_log "logger_default_log() specifications".
 *
 * See test description:
 * - @ref testing_foundation_logger_integration_logger_logger_default_stream_osal_time_doubled_log "`logger_log()` / `logger_default` integration tests section".
 */
typedef enum {
	LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_LOGGER_NULL = 0,
	LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_MESSAGE_NULL,
	LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_STREAM_WRITE_ERROR,
	LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_STREAM_FLUSH_ERROR,
	LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_OK
} logger_log_logger_default_scenario_t;

typedef struct {

	const char *name;
	logger_log_logger_default_scenario_t scenario;

	const char *logger_stream_sink_initial_content;
	size_t logger_stream_sink_initial_content_len;
	stream_status_t next_write_st_value;
	stream_status_t next_flush_ret;

	bool arg_logger_is_null;
	bool arg_message_is_null;
	const char *first_message_to_be_logged;
	osal_time_t first_log_call_date;
	bool expected_logger_stream_sink_content_is_unchanged;
	const char *expected_first_logger_stream_sink_content;
	size_t expected_first_logger_stream_sink_content_len;

	bool second_log;
	const char *second_message_to_be_logged;
	osal_time_t second_log_call_date;
	const char *expected_second_logger_stream_sink_content;
	size_t expected_second_logger_stream_sink_content_len;

} test_logger_log_logger_default_case_t;

typedef struct {
	fake_stream_adapter_t fake_stream_adapter;
	stream_t *fake_stream;
	logger_t *logger;

	const test_logger_log_logger_default_case_t *tc;
} test_logger_log_logger_default_fixture_t;

static int setup_logger_log_logger_default(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(logger_log_logger_default, state, tc, fx);

	fake_stream_reset();
	fake_stream_adapter_reset_instance(&fx->fake_stream_adapter);
	fx->fake_stream_adapter.write_all = true;
	fake_stream_adapter_set_sink_backing(
		&fx->fake_stream_adapter,
		(const uint8_t *)tc->logger_stream_sink_initial_content,
		tc->logger_stream_sink_initial_content_len
	);
	fx->fake_stream = fake_stream_create_instance(&fx->fake_stream_adapter);
	fake_time_reset();

	logger_env_t logger_env = logger_default_env(logger_default_vtbl(), osal_mem_default_ops());
	assert_int_equal(
		logger_create(&fx->logger, &logger_env),
		LOGGER_STATUS_OK
	);
	logger_default_t *logger_default = NULL;
	logger_default_env_t logger_default_env = logger_default_default_env(fx->fake_stream, osal_time_test_fake_ops(), osal_mem_default_ops());
	assert_int_equal(
		logger_default_create(&logger_default, &logger_default_env),
		LOGGER_DEFAULT_STATUS_OK
	);
	logger_default_cfg_t logger_default_cfg = logger_default_default_cfg();
	assert_int_equal(
		logger_default_complete_default_init(logger_default, &logger_default_cfg),
		LOGGER_DEFAULT_STATUS_OK
	);
	assert_int_equal(
		logger_complete_default_init(fx->logger, logger_default),
		LOGGER_STATUS_OK
	);

	*state = fx;
	return 0;
}

static int teardown_logger_log_logger_default(void **state)
{
	test_logger_log_logger_default_fixture_t *fx = (test_logger_log_logger_default_fixture_t *)(*state);
	logger_destroy(&fx->logger);
	osal_free(fx);
	return 0;
}

static void test_logger_log_logger_default(void **state) {
	test_logger_log_logger_default_fixture_t *fx = (test_logger_log_logger_default_fixture_t *)(*state);
	const test_logger_log_logger_default_case_t *tc = fx->tc;

	// ARRANGE
	logger_t *arg_logger = (tc->arg_logger_is_null) ? NULL : fx->logger;
	const char *arg_message = (tc->arg_message_is_null) ? NULL : tc->first_message_to_be_logged;
	fx->fake_stream_adapter.next_write_st_value = tc->next_write_st_value;
	fx->fake_stream_adapter.next_flush_ret = tc->next_flush_ret;
	fake_time_set_now_out(tc->first_log_call_date);
	uint8_t logger_stream_sink_backing_content_snapshot[FAKE_STREAM_ADAPTER_BUF_SIZE] = {0};
	osal_memcpy(logger_stream_sink_backing_content_snapshot, fx->fake_stream_adapter.sink_backing, FAKE_STREAM_ADAPTER_BUF_SIZE);
	size_t logger_stream_sink_backing_content_len_snapshot = fx->fake_stream_adapter.sink_len;

	// ACT
	logger_status_t ret = logger_log(arg_logger, arg_message);

	// ASSERT
	switch (tc->scenario) {
		case LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_LOGGER_NULL: {
			assert_int_equal(ret, LOGGER_STATUS_INVALID);
			break;
		}
		case LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_MESSAGE_NULL: {
			assert_int_equal(ret, LOGGER_STATUS_INVALID);
			break;
		}
		case LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_STREAM_WRITE_ERROR: {
			assert_int_equal(ret, LOGGER_STATUS_IO_ERROR);
			break;
		}
		case LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_STREAM_FLUSH_ERROR: {
			assert_int_equal(ret, LOGGER_STATUS_IO_ERROR);
			break;
		}
		case LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_OK: {
			assert_int_equal(ret, LOGGER_STATUS_OK);
			break;
		}
		default: fail();
	}
	if (tc->expected_logger_stream_sink_content_is_unchanged) {
		assert_int_equal(fx->fake_stream_adapter.sink_len, logger_stream_sink_backing_content_len_snapshot);
		assert_memory_equal(
			fx->fake_stream_adapter.sink_backing,
			logger_stream_sink_backing_content_snapshot,
			logger_stream_sink_backing_content_len_snapshot
		);
	} else {
		assert_int_equal(fx->fake_stream_adapter.sink_len, tc->expected_first_logger_stream_sink_content_len);
		assert_memory_equal(
			fx->fake_stream_adapter.sink_backing,
			tc->expected_first_logger_stream_sink_content,
			tc->expected_first_logger_stream_sink_content_len
		);
	}

	if (!tc->second_log) {
		return;
	}

	// ARRANGE
	arg_logger = fx->logger;
	arg_message = tc->second_message_to_be_logged;
	fake_time_set_now_out(tc->second_log_call_date);

	// ACT
	ret = logger_log(arg_logger, arg_message);

	// ASSERT
	assert_int_equal(ret, LOGGER_STATUS_OK);
	assert_int_equal(fx->fake_stream_adapter.sink_len, tc->expected_second_logger_stream_sink_content_len);
	assert_memory_equal(
		fx->fake_stream_adapter.sink_backing,
		tc->expected_second_logger_stream_sink_content,
		tc->expected_second_logger_stream_sink_content_len
	);
}

static const test_logger_log_logger_default_case_t CASE_LOGGER_LOG_LOGGER_NULL = {
	.name = "logger_log_logger_default_logger_null",
	.scenario = LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_LOGGER_NULL,

	.logger_stream_sink_initial_content = "[1969-12-31 23:59:59 UTC+0] One second before the Unix epoch!\n",
	.logger_stream_sink_initial_content_len =  sizeof("[1969-12-31 23:59:59 UTC+0] One second before the Unix epoch!\n") - 1,
	.next_write_st_value = STREAM_STATUS_OK,
	.next_flush_ret = STREAM_STATUS_OK,

	.arg_logger_is_null = true,
	.arg_message_is_null = false,
	.first_message_to_be_logged = "It is the Unix epoch now!",
	.first_log_call_date = (osal_time_t) { .epoch_seconds = 0 },
	.expected_logger_stream_sink_content_is_unchanged = true,

	.second_log = false
};

static const test_logger_log_logger_default_case_t CASE_LOGGER_LOG_MESSAGE_NULL = {
	.name = "logger_log_logger_default_message_null",
	.scenario = LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_MESSAGE_NULL,

	.logger_stream_sink_initial_content = "[1969-12-31 23:59:59 UTC+0] One second before the Unix epoch!\n",
	.logger_stream_sink_initial_content_len =  sizeof("[1969-12-31 23:59:59 UTC+0] One second before the Unix epoch!\n") - 1,
	.next_write_st_value = STREAM_STATUS_OK,
	.next_flush_ret = STREAM_STATUS_OK,

	.arg_logger_is_null = false,
	.arg_message_is_null = true,
	.first_message_to_be_logged = "It is the Unix epoch now!",
	.first_log_call_date = (osal_time_t) { .epoch_seconds = 0 },
	.expected_logger_stream_sink_content_is_unchanged = true,

	.second_log = false
};

static const test_logger_log_logger_default_case_t CASE_LOGGER_LOG_STREAM_WRITE_ERROR = {
	.name = "logger_log_logger_default_stream_write_error",
	.scenario = LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_STREAM_WRITE_ERROR,

	.logger_stream_sink_initial_content = "[1969-12-31 23:59:59 UTC+0] One second before the Unix epoch!\n",
	.logger_stream_sink_initial_content_len =  sizeof("[1969-12-31 23:59:59 UTC+0] One second before the Unix epoch!\n") - 1,
	.next_write_st_value = STREAM_STATUS_IO_ERROR,
	.next_flush_ret = STREAM_STATUS_OK,

	.arg_logger_is_null = false,
	.arg_message_is_null = false,
	.first_message_to_be_logged = "It is the Unix epoch now!",
	.first_log_call_date = (osal_time_t) { .epoch_seconds = 0 },
	.expected_logger_stream_sink_content_is_unchanged = true,

	.second_log = false
};

static const test_logger_log_logger_default_case_t CASE_LOGGER_LOG_STREAM_FLUSH_ERROR = {
	.name = "logger_log_logger_default_stream_flush_error",
	.scenario = LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_STREAM_FLUSH_ERROR,

	.logger_stream_sink_initial_content = "[1969-12-31 23:59:59 UTC+0] One second before the Unix epoch!\n",
	.logger_stream_sink_initial_content_len =  sizeof("[1969-12-31 23:59:59 UTC+0] One second before the Unix epoch!\n") - 1,
	.next_write_st_value = STREAM_STATUS_OK,
	.next_flush_ret = STREAM_STATUS_IO_ERROR,

	.arg_logger_is_null = false,
	.arg_message_is_null = false,
	.first_message_to_be_logged = "It is the Unix epoch now!",
	.first_log_call_date = (osal_time_t) { .epoch_seconds = 0 },
	.expected_logger_stream_sink_content_is_unchanged = true,

	.second_log = false
};

static const test_logger_log_logger_default_case_t CASE_LOGGER_LOG_OK = {
	.name = "logger_log_logger_default_ok",
	.scenario = LOGGER_LOG_LOGGER_DEFAULT_SCENARIO_OK,

	.logger_stream_sink_initial_content = "[1969-12-31 23:59:59 UTC+0] One second before the Unix epoch!\n",
	.logger_stream_sink_initial_content_len =  sizeof("[1969-12-31 23:59:59 UTC+0] One second before the Unix epoch!\n") - 1,
	.next_write_st_value = STREAM_STATUS_OK,
	.next_flush_ret = STREAM_STATUS_OK,

	.arg_logger_is_null = false,
	.arg_message_is_null = false,
	.first_message_to_be_logged = "It is the Unix epoch now!",
	.first_log_call_date = (osal_time_t) { .epoch_seconds = 0 },
	.expected_logger_stream_sink_content_is_unchanged = false,
	.expected_first_logger_stream_sink_content = "\
[1969-12-31 23:59:59 UTC+0] One second before the Unix epoch!\n\
[1970-01-01 00:00:00 UTC+0] It is the Unix epoch now!\n",
	.expected_first_logger_stream_sink_content_len = sizeof("\
[1969-12-31 23:59:59 UTC+0] One second before the Unix epoch!\n\
[1970-01-01 00:00:00 UTC+0] It is the Unix epoch now!\n") - 1,
	.second_log = true,
	.second_message_to_be_logged = "One minute after the Unix epoch!",
	.second_log_call_date = (osal_time_t) { .epoch_seconds = 60 },
	.expected_second_logger_stream_sink_content = "\
[1969-12-31 23:59:59 UTC+0] One second before the Unix epoch!\n\
[1970-01-01 00:00:00 UTC+0] It is the Unix epoch now!\n\
[1970-01-01 00:01:00 UTC+0] One minute after the Unix epoch!\n",
	.expected_second_logger_stream_sink_content_len = sizeof("\
[1969-12-31 23:59:59 UTC+0] One second before the Unix epoch!\n\
[1970-01-01 00:00:00 UTC+0] It is the Unix epoch now!\n\
[1970-01-01 00:01:00 UTC+0] One minute after the Unix epoch!\n") - 1
};

#define LOGGER_LOG_LOGGER_DEFAULT_CASES(X) \
X(CASE_LOGGER_LOG_LOGGER_NULL) \
X(CASE_LOGGER_LOG_MESSAGE_NULL) \
X(CASE_LOGGER_LOG_STREAM_WRITE_ERROR) \
X(CASE_LOGGER_LOG_STREAM_FLUSH_ERROR) \
X(CASE_LOGGER_LOG_OK)

#define MAKE_LOGGER_LOG_LOGGER_DEFAULT_TEST(case_sym) \
LEXLEO_MAKE_TEST(logger_log_logger_default, case_sym)

static const struct CMUnitTest logger_log_logger_default_tests[] = {
	LOGGER_LOG_LOGGER_DEFAULT_CASES(MAKE_LOGGER_LOG_LOGGER_DEFAULT_TEST)
};

#undef LOGGER_LOG_LOGGER_DEFAULT_CASES
#undef MAKE_LOGGER_LOG_LOGGER_DEFAULT_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	static const struct CMUnitTest logger_logger_default_integration_tests_non_parametric[] = {
		cmocka_unit_test(test_logger_logger_default_smoke)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(logger_logger_default_integration_tests_non_parametric, NULL, NULL);
	failed += cmocka_run_group_tests(logger_log_logger_default_tests, NULL, NULL);

	return failed;
}
/** @endcond */
