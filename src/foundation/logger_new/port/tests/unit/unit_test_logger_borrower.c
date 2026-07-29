/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_logger_borrower.c
 * @ingroup logger_unit_tests
 * @brief Unit tests implementation for logger_borrower.c.
 *
 * See also:
 * - @ref testing_foundation_logger_borrower_unit "logger_borrower.c unit tests page"
 * - @ref specifications_logger "logger specifications"
 */

#include "logger/borrowers/logger_borrowers_api.h"

#include "logger/cr/logger_cr_api.h"

#include "logger/tests/logger_fake_adapter.h"
#include "logger/tests/logger_white_box_tests_access.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/osal_mem_ops.h"

#include "lexleo_cmocka.h"

/******************************************************************************************************************************************
 * @brief Test scenarios for `logger_log()`.
 *
 * See contract:
 * - @ref specifications_logger_log "logger_log() specifications".
 *
 * See test description:
 * - @ref testing_foundation_logger_unit_logger_log "logger_log() unit tests section"
 */
typedef enum {
	LOGGER_LOG_SCENARIO_LOGGER_NULL = 0,
	LOGGER_LOG_SCENARIO_MESSAGE_NULL,
	LOGGER_LOG_SCENARIO_BACKEND_IO_ERROR,
	LOGGER_LOG_SCENARIO_OK
} logger_log_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	logger_log_scenario_t scenario;
} test_logger_log_case_t;

typedef struct {
	logger_t *logger;
	logger_fake_adapter_backend_t fake_adapter_backend;
	const osal_mem_ops_t *mem;
	const test_logger_log_case_t *tc;
} test_logger_log_fixture_t;

static int setup_logger_log(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(logger_log, state, tc, fx);

	logger_fake_adapter_init_backend(&fx->fake_adapter_backend);
	fx->mem = osal_mem_default_ops();
	logger_env_t env = logger_default_env(&logger_fake_adapter_vtbl, fx->mem);
	assert_int_equal(logger_create(&fx->logger, &env), LOGGER_STATUS_OK);
	logger_inject_backend(fx->logger, (void *)&fx->fake_adapter_backend);
	assert_int_equal(logger_complete_default_init(fx->logger, NULL), LOGGER_STATUS_OK);

	*state = fx;
	return 0;
}

static int teardown_logger_log(void **state)
{
	test_logger_log_fixture_t *fx = (test_logger_log_fixture_t *)(*state);
	logger_destroy(&fx->logger);
	osal_free(fx);
	return 0;
}

static void test_logger_log(void **state)
{
	test_logger_log_fixture_t *fx = (test_logger_log_fixture_t *)(*state);
	const test_logger_log_case_t *tc = fx->tc;

	// ARRANGE
	logger_status_t ret = LOGGER_STATUS_OK;
	logger_t *logger_arg = fx->logger;
	const char *message_arg = "test message to be written in logged";
	switch (tc->scenario) {
		case LOGGER_LOG_SCENARIO_LOGGER_NULL: {
			logger_arg = NULL;
			break;
		}
		case LOGGER_LOG_SCENARIO_MESSAGE_NULL: {
			message_arg = NULL;
			break;
		}
		case LOGGER_LOG_SCENARIO_BACKEND_IO_ERROR: {
			fx->fake_adapter_backend.next_log_ret = LOGGER_STATUS_IO_ERROR;
			break;
		}
		case LOGGER_LOG_SCENARIO_OK: {
			break;
		}
		default: {
			fail();
		}
	}

	// ACT
	ret = logger_log(logger_arg, message_arg);

	// ASSERT
	switch (tc->scenario) {
		case LOGGER_LOG_SCENARIO_LOGGER_NULL: {
			assert_int_equal(ret, LOGGER_STATUS_INVALID);
			assert_int_equal(fx->fake_adapter_backend.log_call_count, 0);
			break;
		}
		case LOGGER_LOG_SCENARIO_MESSAGE_NULL: {
			assert_int_equal(ret, LOGGER_STATUS_INVALID);
			assert_int_equal(fx->fake_adapter_backend.log_call_count, 0);
			break;
		}
		case LOGGER_LOG_SCENARIO_BACKEND_IO_ERROR: {
			assert_int_equal(ret, LOGGER_STATUS_IO_ERROR);
			assert_int_equal(fx->fake_adapter_backend.log_call_count, 1);
			assert_ptr_equal(fx->fake_adapter_backend.last_log_backend, logger_get_backend(logger_arg));
			assert_ptr_equal(fx->fake_adapter_backend.last_log_message, message_arg);
			break;
		}
		case LOGGER_LOG_SCENARIO_OK: {
			assert_int_equal(ret, LOGGER_STATUS_OK);
			assert_int_equal(fx->fake_adapter_backend.log_call_count, 1);
			assert_ptr_equal(fx->fake_adapter_backend.last_log_backend, logger_get_backend(logger_arg));
			assert_ptr_equal(fx->fake_adapter_backend.last_log_message, message_arg);
			break;
		}
		default: {
			fail();
		}
	}
}

static const test_logger_log_case_t CASE_LOGGER_LOG_LOGGER_NULL = {
	.name = "logger_log_logger_null",
	.scenario = LOGGER_LOG_SCENARIO_LOGGER_NULL,
};

static const test_logger_log_case_t CASE_LOGGER_LOG_MESSAGE_NULL = {
	.name = "logger_log_message_null",
	.scenario = LOGGER_LOG_SCENARIO_MESSAGE_NULL,
};

static const test_logger_log_case_t CASE_LOGGER_LOG_BACKEND_IO_ERROR = {
	.name = "logger_log_backend_io_error",
	.scenario = LOGGER_LOG_SCENARIO_BACKEND_IO_ERROR,
};

static const test_logger_log_case_t CASE_LOGGER_LOG_OK = {
	.name = "logger_log_ok",
	.scenario = LOGGER_LOG_SCENARIO_OK,
};

#define LOGGER_LOG_CASES(X) \
X(CASE_LOGGER_LOG_LOGGER_NULL) \
X(CASE_LOGGER_LOG_MESSAGE_NULL) \
X(CASE_LOGGER_LOG_BACKEND_IO_ERROR) \
X(CASE_LOGGER_LOG_OK)

#define MAKE_LOGGER_LOG_TEST(case_sym) \
LEXLEO_MAKE_TEST(logger_log, case_sym)

static const struct CMUnitTest logger_log_tests[] = {
	LOGGER_LOG_CASES(MAKE_LOGGER_LOG_TEST)
};

#undef LOGGER_LOG_CASES
#undef MAKE_LOGGER_LOG_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	int failed = 0;
	failed += cmocka_run_group_tests(logger_log_tests, NULL, NULL);
	return failed;
}
/** @endcond */
