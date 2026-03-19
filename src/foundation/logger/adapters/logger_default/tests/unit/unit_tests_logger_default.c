/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_tests_logger_default.c
 * @ingroup logger_default_unit_tests
 * @brief Unit tests implementation for the `logger_default` adapter.
 *
 * @details
 * This file implements unit-level validation of the `logger_default` adapter
 * contracts.
 *
 * Covered surfaces:
 * - CR helpers: `logger_default_default_cfg()`, `logger_default_default_env()`
 * - direct creation: `logger_default_create_logger()`
 *
 * See also:
 * - @ref testing_foundation_logger_default_unit "logger_default unit tests page"
 * - @ref specifications_logger_default "logger_default specifications"
 */

#include "logger_default/cr/logger_default_cr_api.h"
#include "logger/cr/logger_cr_api.h"
#include "logger/lifecycle/logger_lifecycle.h"
#include "stream/borrowers/stream_types.h"
#include "osal/mem/test/osal_mem_fake_provider.h"
#include "stream/test/stream_fake_provider.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "lexleo_cmocka_xmacro_helpers.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Test `logger_default_default_cfg()`.
 *
 * logger_default_cfg_t logger_default_default_cfg(void);
 *
 * Success:
 * - Returns a well-formed default `logger_default_cfg_t`.
 * - `ret.append_newline == true`.
 *
 * Failure:
 * - None.
 *
 * See also:
 * - @ref testing_foundation_logger_default_unit_default_cfg "logger_default_default_cfg() unit tests section"
 * - @ref specifications_logger_default_default_cfg "logger_default_default_cfg() specifications"
 */
static void test_logger_default_default_cfg(void **state)
{
	(void)state;

	logger_default_cfg_t ret = logger_default_default_cfg();

	assert_true(ret.append_newline);
}

/**
 * @brief Test `logger_default_default_env()`.
 *
 * logger_default_env_t logger_default_default_env(
 *     stream_t *stream,
 *     const osal_mem_ops_t *adapter_mem,
 *     const logger_env_t *port_env );
 *
 * Success:
 * - `ret.stream == stream`.
 * - `ret.adapter_mem == adapter_mem`.
 * - `ret.port_env == *port_env`.
 *
 * Failure:
 * - None.
 *
 * Doubles:
 * - dummy `stream_t *`
 * - dummy `osal_mem_ops_t *`
 * - dummy `logger_env_t`
 *
 * See also:
 * - @ref testing_foundation_logger_default_unit_default_env "logger_default_default_env() unit tests section"
 * - @ref specifications_logger_default_default_env "logger_default_default_env() specifications".
 */
static void test_logger_default_default_env(void **state) {
	(void)state;

	stream_t *dummy_stream_p = (stream_t *)(uintptr_t)0x1234u;

	const osal_mem_ops_t *dummy_adapter_mem_p =
		(const osal_mem_ops_t *)(uintptr_t)0x5678u;

	const logger_env_t dummy_port_env = {0};
	const logger_env_t *dummy_port_env_p = &dummy_port_env;

	logger_default_env_t ret =
		logger_default_default_env(
			dummy_stream_p,
			dummy_adapter_mem_p,
			dummy_port_env_p);

	assert_ptr_equal(ret.stream, dummy_stream_p);
	assert_ptr_equal(ret.adapter_mem, dummy_adapter_mem_p);
	assert_memory_equal(&ret.port_env, dummy_port_env_p, sizeof(ret.port_env));
}

/**
 * @brief Scenarios for `logger_default_create_logger()`.
 *
 * logger_status_t logger_default_create_logger(
 * 		logger_t **out,
 * 		const logger_default_cfg_t *cfg,
 * 		const logger_default_env_t *env);
 *
 * Doubles:
 * - fake_stream
 * - fake_memory
 *
 * Isolation:
 * - the public `stream` port wrapper is not doubled
 * - stream runtime behavior is controlled through `fake_stream`
 *
 * See also:
 * - @ref testing_foundation_logger_default_unit_create_logger "logger_default_create_logger() unit tests section"
 * - @ref specifications_logger_default_create_logger "logger_default_create_logger() specifications"
 *
 * The scenarios below define the test oracle for `logger_default_create_logger()`.
 */
typedef enum {
	/**
	 * WHEN `logger_default_create_logger(out, cfg, env)` is called with valid arguments
	 * EXPECT:
	 * - returns `LOGGER_STATUS_OK`
	 * - stores a non-NULL logger handle in `*out`
	 */
	LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_OK = 0,

	/**
	 * WHEN `out == NULL`
	 * EXPECT:
	 * - returns `LOGGER_STATUS_INVALID`
	 */
	LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_OUT_NULL,

	/**
	 * WHEN `cfg == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `LOGGER_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_CFG_NULL,

	/**
	 * WHEN `env == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `LOGGER_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_ENV_NULL,

	/**
	 * WHEN allocation required by `logger_default_create_logger()` fails
	 * EXPECT:
	 * - returns `LOGGER_STATUS_OOM`
	 * - leaves `*out` unchanged
	 *
	 * Notes:
	 * - This scenario is exercised by configuring `fake_memory` to fail the
	 *   allocation performed during logger creation.
	 */
	LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_OOM,
} logger_default_create_logger_scenario_t;

/** @cond INTERNAL */

typedef enum {
    OUT_CHECK_NONE,
    OUT_EXPECT_NULL,
    OUT_EXPECT_NON_NULL,
    OUT_EXPECT_UNCHANGED
} out_expect_t;

typedef struct {
	const char *name;

	// arrange
	logger_default_create_logger_scenario_t scenario;
	size_t fail_call_idx; // 0 = no OOM; otherwise 1-based allocation index

	// assert
	logger_status_t expected_ret;
	out_expect_t out_expect;
} test_logger_default_create_logger_case_t;

typedef struct {
	// runtime resources
	logger_t *out;
	stream_fake_t *fake_stream_adapter;
	stream_t *fake_stream;

	// injection
	logger_default_env_t env;

	logger_default_cfg_t cfg;

	const test_logger_default_create_logger_case_t *tc;
} test_logger_default_create_logger_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

static int setup_logger_default_create_logger(void **state)
{
	const test_logger_default_create_logger_case_t *tc =
		(const test_logger_default_create_logger_case_t *)(*state);

	test_logger_default_create_logger_fixture_t *fx = malloc(sizeof(*fx));
	if (!fx) return -1;
	memset(fx, 0, sizeof(*fx));

	fake_memory_reset();

	fx->cfg.append_newline = true;

	assert_int_equal(
		stream_fake_create(&fx->fake_stream_adapter, &fx->fake_stream, osal_mem_test_fake_ops()),
		STREAM_STATUS_OK
	);

	stream_fake_reset(fx->fake_stream_adapter);

	// DI
	fx->env.stream = fx->fake_stream;
	fx->env.adapter_mem = osal_mem_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();

	fx->tc = tc;

	if (tc->scenario == LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_OOM) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	*state = fx;

	return 0;
}

static int teardown_logger_default_create_logger(void **state)
{
	test_logger_default_create_logger_fixture_t *fx =
		(test_logger_default_create_logger_fixture_t *)(*state);

	if (fx->out) {
		logger_destroy(&fx->out);
		fx->out = NULL;
	}

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

static void test_logger_default_create_logger(void **state)
{
	test_logger_default_create_logger_fixture_t *fx =
		(test_logger_default_create_logger_fixture_t *)(*state);
	const test_logger_default_create_logger_case_t *tc = fx->tc;

	// ARRANGE
	logger_status_t ret = (logger_status_t)-1; // poison

	logger_t **out_arg = &fx->out;
	const logger_default_cfg_t *cfg_arg = &fx->cfg;
	const logger_default_env_t *env_arg = &fx->env;

	// invalid args
	if (tc->scenario == LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_CFG_NULL) cfg_arg = NULL;
	if (tc->scenario == LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_ENV_NULL) env_arg = NULL;

    // ensure OUT_EXPECT_UNCHANGED is meaningful
    if (tc->out_expect == OUT_EXPECT_UNCHANGED && out_arg != NULL) {
        fx->out = (logger_t *)(uintptr_t)0xDEADC0DEu; // sentinel
    }

    logger_t *out_arg_snapshot = fx->out;

	// ACT
	ret = logger_default_create_logger(out_arg, cfg_arg, env_arg);

	// ASSERT
	assert_int_equal(ret, tc->expected_ret);

	switch (tc->out_expect) {
		case OUT_CHECK_NONE: break;
		case OUT_EXPECT_NULL: assert_null(fx->out); break;
		case OUT_EXPECT_NON_NULL: assert_non_null(fx->out); break;
		case OUT_EXPECT_UNCHANGED:
			assert_ptr_equal(out_arg_snapshot, fx->out);
			fx->out = NULL; // prevent teardown from destroying sentinel
			break;
		default: assert_true(false);
	}

	if (tc->scenario == LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_OK) {
		assert_non_null(fx->out);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_logger_default_create_logger_case_t CASE_LOGGER_DEFAULT_CREATE_LOGGER_OUT_NULL = {
	.name = "logger_default_create_logger_out_null",
	.scenario = LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_OUT_NULL,
	.fail_call_idx = 0,

	.expected_ret = LOGGER_STATUS_INVALID,
	.out_expect = OUT_CHECK_NONE
};

static const test_logger_default_create_logger_case_t CASE_LOGGER_DEFAULT_CREATE_LOGGER_CFG_NULL = {
	.name = "logger_default_create_logger_cfg_null",
	.scenario = LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_CFG_NULL,
	.fail_call_idx = 0,

	.expected_ret = LOGGER_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_logger_default_create_logger_case_t CASE_LOGGER_DEFAULT_CREATE_LOGGER_ENV_NULL = {
	.name = "logger_default_create_logger_env_null",
	.scenario = LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_ENV_NULL,
	.fail_call_idx = 0,

	.expected_ret = LOGGER_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_logger_default_create_logger_case_t CASE_LOGGER_DEFAULT_CREATE_LOGGER_OOM_1 = {
	.name = "logger_default_create_logger_oom_1",
	.scenario = LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_OOM,
	.fail_call_idx = 1,

	.expected_ret = LOGGER_STATUS_OOM,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_logger_default_create_logger_case_t CASE_LOGGER_DEFAULT_CREATE_LOGGER_OOM_2 = {
	.name = "logger_default_create_logger_oom_2",
	.scenario = LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_OOM,
	.fail_call_idx = 2,

	.expected_ret = LOGGER_STATUS_OOM,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_logger_default_create_logger_case_t CASE_LOGGER_DEFAULT_CREATE_LOGGER_OK = {
	.name = "logger_default_create_logger_ok",
	.scenario = LOGGER_DEFAULT_CREATE_LOGGER_SCENARIO_OK,
	.fail_call_idx = 0,

	.expected_ret = LOGGER_STATUS_OK,
	.out_expect = OUT_EXPECT_NON_NULL
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define LOGGER_DEFAULT_CREATE_LOGGER_CASES(X) \
X(CASE_LOGGER_DEFAULT_CREATE_LOGGER_OUT_NULL) \
X(CASE_LOGGER_DEFAULT_CREATE_LOGGER_CFG_NULL) \
X(CASE_LOGGER_DEFAULT_CREATE_LOGGER_ENV_NULL) \
X(CASE_LOGGER_DEFAULT_CREATE_LOGGER_OOM_1) \
X(CASE_LOGGER_DEFAULT_CREATE_LOGGER_OOM_2) \
X(CASE_LOGGER_DEFAULT_CREATE_LOGGER_OK)

#define LOGGER_DEFAULT_MAKE_CREATE_LOGGER_TEST(case_sym) \
LEXLEO_MAKE_TEST(logger_default_create_logger, case_sym)

static const struct CMUnitTest logger_default_create_logger_tests[] = {
	LOGGER_DEFAULT_CREATE_LOGGER_CASES(LOGGER_DEFAULT_MAKE_CREATE_LOGGER_TEST)
};

#undef LOGGER_DEFAULT_CREATE_LOGGER_CASES
#undef LOGGER_DEFAULT_MAKE_CREATE_LOGGER_TEST

/** @endcond */

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

/** @cond INTERNAL */

int main(void) {
	static const struct CMUnitTest logger_default_non_parametric_tests[] = {
		cmocka_unit_test(test_logger_default_default_cfg),
		cmocka_unit_test(test_logger_default_default_env)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(logger_default_non_parametric_tests, NULL, NULL);
	failed += cmocka_run_group_tests(logger_default_create_logger_tests, NULL, NULL);

	return failed;
}

/** @endcond */
