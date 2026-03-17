/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_logger.c
 * @ingroup logger_unit_tests
 * @brief Unit tests implementation for the logger port.
 *
 * @details
 * This file implements the unit-level validation of the logger port contracts.
 *
 * Covered surfaces:
 * - `logger_default_env()`
 * - `logger_create()` / `logger_destroy()`
 * - `logger_log()`
 *
 * See also:
 * - @ref testing_foundation_logger_unit "logger unit tests page"
 * - @ref specifications_logger "logger specifications"
 */

#include "logger/cr/logger_cr_api.h"
#include "logger/adapters/logger_adapters_api.h"
#include "logger/lifecycle/logger_lifecycle.h"
#include "logger/borrowers/logger.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "lexleo_cmocka_xmacro_helpers.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/** @cond INTERNAL */

//-----------------------------------------------------------------------------
// LOCAL TEST DOUBLES
//-----------------------------------------------------------------------------

typedef struct fake_logger_backend_t {
	/** call counters */
	int log_called;
	int destroy_called;

	/** last arguments */
	void *last_backend;
	const char *last_message;

	/** configurable behavior */
	logger_status_t log_ret;
} fake_logger_backend_t;

static void fake_logger_backend_reset(fake_logger_backend_t *b) {
	assert_non_null(b);
	b->log_called = 0;
	b->destroy_called = 0;
	b->last_backend = NULL;
	b->last_message = NULL;
	b->log_ret = LOGGER_STATUS_INVALID;
}

static logger_status_t fake_logger_log(void *backend, const char *message) {
	fake_logger_backend_t *b = (fake_logger_backend_t *)backend;
    assert_non_null(b);

	b->last_backend = backend;
	b->last_message = message;

	b->log_called++;

	return b->log_ret;
}

static void fake_logger_destroy(void *backend) {
    fake_logger_backend_t *b = (fake_logger_backend_t *)backend;
    assert_non_null(b);

	b->last_backend = backend;

	b->destroy_called++;
}

static const logger_vtbl_t fake_logger_vtbl = {
	.log = fake_logger_log,
	.destroy = fake_logger_destroy
};

/** @endcond */

/**
 * @brief Test `logger_default_env()`.
 *
 * logger_env_t logger_default_env(const osal_mem_ops_t *mem_ops);
 *
 * Success:
 * - `ret.mem == mem_ops`.
 *
 * Failure:
 * - None.
 *
 * Doubles:
 * - dummy `osal_mem_ops_t`
 *
 * See also:
 * - @ref testing_foundation_logger_unit_logger_default_env "logger_default_env() unit tests section"
 * - @ref specifications_logger_default_env "logger_default_env() specifications".
 */
static void test_logger_default_env(void **state) {
	(void)state;

	const osal_mem_ops_t dummy = {0};
	const osal_mem_ops_t *dummy_p = &dummy;

	logger_env_t ret = logger_default_env(dummy_p);

	assert_ptr_equal(ret.mem, dummy_p);
}

/**
 * @brief Scenarios for `logger_create()` / `logger_destroy()`.
 *
 * logger_status_t logger_create(
 *	   logger_t **out,
 *	   const logger_vtbl_t *vtbl,
 *	   void *backend,
 *	   const logger_env_t *env );
 *
 * void logger_destroy(logger_t **l);
 *
 * Invalid arguments:
 * - `out`, `vtbl`, `env` must not be NULL.
 * - `vtbl->log`, `vtbl->destroy` must not be NULL.
 * - `env->mem` must not be NULL.
 *
 * Success:
 * - Returns `LOGGER_STATUS_OK`.
 * - Stores a valid logger_t in `*out`.
 * - The produced logger_t must be destroyed via `logger_destroy()`.
 *
 * Failure:
 * - Returns:
 *     - `LOGGER_STATUS_INVALID` for invalid arguments
 *     - `LOGGER_STATUS_OOM` on allocation failure
 * - Leaves `*out` unchanged if `out` is not NULL.
 *
 * Lifecycle:
 * - `logger_destroy()` does nothing if `l` is NULL or `*l` is NULL.
 * - Otherwise, it releases the logger_t object and sets `*l` to NULL.
 *
 * Doubles:
 * - fake_memory
 *
 * See also:
 * - @ref testing_foundation_logger_unit_logger_create_logger_destroy "logger_create() / logger_destroy() unit tests section"
 * - @ref specifications_logger_create "logger_create() specifications"
 * - @ref specifications_logger_destroy "logger_destroy() specifications"
 *
 * The scenarios below define the test oracle for `logger_create()` and `logger_destroy()`.
 */
typedef enum {
    /**
     * WHEN `logger_create(out, vtbl, backend, env)` is called with valid arguments
     * EXPECT:
     * - returns `LOGGER_STATUS_OK`
     * - stores a non-NULL logger handle in `*out`
     * - the produced handle is eligible for destruction by `logger_destroy()`
     */
    LOGGER_LIFECYCLE_SCENARIO_OK = 0,

    /**
     * WHEN `out == NULL`
     * EXPECT:
     * - returns `LOGGER_STATUS_INVALID`
     * - no logger handle is produced
     */
    LOGGER_LIFECYCLE_SCENARIO_OUT_NULL,

    /**
     * WHEN `vtbl == NULL` and `out != NULL`
     * EXPECT:
     * - returns `LOGGER_STATUS_INVALID`
     * - leaves `*out` unchanged
     */
    LOGGER_LIFECYCLE_SCENARIO_VTBL_NULL,

    /**
     * WHEN `vtbl != NULL` but `vtbl->log == NULL` and `out != NULL`
     * EXPECT:
     * - returns `LOGGER_STATUS_INVALID`
     * - leaves `*out` unchanged
     *
     * Notes:
     * - The logger port requires a well-formed vtbl at creation time.
     */
    LOGGER_LIFECYCLE_SCENARIO_VTBL_LOG_NULL,

    /**
     * WHEN `vtbl != NULL` but `vtbl->destroy == NULL` and `out != NULL`
     * EXPECT:
     * - returns `LOGGER_STATUS_INVALID`
     * - leaves `*out` unchanged
     *
     * Notes:
     * - The logger port requires a well-formed vtbl at creation time.
     */
    LOGGER_LIFECYCLE_SCENARIO_VTBL_DESTROY_NULL,

    /**
     * WHEN `backend == NULL` and `out != NULL`
     * EXPECT:
     * - returns `LOGGER_STATUS_INVALID`
     * - leaves `*out` unchanged
     */
    LOGGER_LIFECYCLE_SCENARIO_BACKEND_NULL,

    /**
     * WHEN `env == NULL` and `out != NULL`
     * EXPECT:
     * - returns `LOGGER_STATUS_INVALID`
     * - leaves `*out` unchanged
     */
    LOGGER_LIFECYCLE_SCENARIO_ENV_NULL,

    /**
     * WHEN `env != NULL` but `env->mem == NULL` and `out != NULL`
     * EXPECT:
     * - returns `LOGGER_STATUS_INVALID`
     * - leaves `*out` unchanged
     */
    LOGGER_LIFECYCLE_SCENARIO_ENV_MEM_NULL,

    /**
     * WHEN allocation of the logger handle fails (allocator reports OOM)
     * EXPECT:
     * - returns `LOGGER_STATUS_OOM`
     * - leaves `*out` unchanged
     *
     * Notes:
     * - This scenario is exercised by configuring `fake_memory` to fail the
     *   allocation performed by `logger_create()`.
     */
    LOGGER_LIFECYCLE_SCENARIO_OOM,

    /**
     * WHEN `logger_create()` succeeds and `logger_destroy()` is called twice
     * EXPECT:
     * - first `logger_destroy(&l)` releases the handle and sets `l` to NULL
     * - second `logger_destroy(&l)` is a no-op and keeps `l` as NULL
     *
     * Notes:
     * - This scenario checks the idempotence guarantee of `logger_destroy()`.
     */
    LOGGER_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT,
} logger_lifecycle_scenario_t;

/** @cond INTERNAL */

/**
 * @brief Expected state of the output handle after the call under test.
 *
 * @details
 * This helper enum is used by parametric tests to express the expected
 * postcondition on an output pointer managed by the test fixture.
 *
 * Notes:
 * - `OUT_CHECK_NONE` means no postcondition is asserted on the output handle.
 * - `OUT_EXPECT_NULL` means the output handle is expected to be `NULL`
 *   after the call.
 * - `OUT_EXPECT_NON_NULL` means the output handle is expected to be
 *   non-`NULL` after the call.
 * - `OUT_EXPECT_UNCHANGED` means the output handle is expected to preserve
 *   its pre-call value, typically verified with a sentinel pointer.
 */
typedef enum {
	OUT_CHECK_NONE,
	OUT_EXPECT_NULL,
	OUT_EXPECT_NON_NULL,
	OUT_EXPECT_UNCHANGED
} out_expect_t;

/**
 * @brief One parametric test case for the logger lifecycle contract.
 *
 * Notes:
 * - `fail_call_idx` is used by `fake_memory` to inject an allocation failure
 *   on a specific call number (used by the OOM scenario).
 */
typedef struct {
	const char *name;

	logger_lifecycle_scenario_t scenario;
	size_t fail_call_idx;

	logger_status_t expected_ret;
	out_expect_t out_expect;
} test_logger_lifecycle_case_t;

/**
 * @brief Runtime fixture for `logger_create()` / `logger_destroy()` tests.
 *
 * Holds:
 * - the logger handle under test,
 * - the injected environment,
 * - the fake adapter backend,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	logger_t *out;

	// injection
	logger_env_t env;

	fake_logger_backend_t backend;

	const test_logger_lifecycle_case_t *tc;
} test_logger_lifecycle_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for lifecycle tests.
 */
static int setup_logger_lifecycle(void **state)
{
	const test_logger_lifecycle_case_t *tc =
		(const test_logger_lifecycle_case_t *)(*state);

	test_logger_lifecycle_fixture_t *fx =
		(test_logger_lifecycle_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();
	if (tc->scenario == LOGGER_LIFECYCLE_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	// DI
	fx->env.mem = osal_mem_test_fake_ops();

	fake_logger_backend_reset(&fx->backend);

	*state = fx;
	return 0;
}

/**
 * @brief Release the lifecycle test fixture and verify memory invariants.
 */
static int teardown_logger_lifecycle(void **state)
{
	test_logger_lifecycle_fixture_t *fx =
		(test_logger_lifecycle_fixture_t *)(*state);

	if (fx->out) {
		logger_destroy(&fx->out);
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

/**
 * @brief Execute one parametric test scenario for `logger_create()` / `logger_destroy()`.
 */
static void test_logger_lifecycle(void **state)
{
	test_logger_lifecycle_fixture_t *fx =
		(test_logger_lifecycle_fixture_t *)(*state);
	const test_logger_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	logger_status_t ret = LOGGER_STATUS_INVALID;

	logger_t **out_arg = &fx->out;
	const logger_vtbl_t *vtbl_arg = &fake_logger_vtbl;
	logger_vtbl_t vtbl_local;
	void *backend_arg = &fx->backend;
	const logger_env_t *env_arg = &fx->env;

	// invalid args
	if (tc->scenario == LOGGER_LIFECYCLE_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == LOGGER_LIFECYCLE_SCENARIO_VTBL_NULL) vtbl_arg = NULL;
	if (tc->scenario == LOGGER_LIFECYCLE_SCENARIO_BACKEND_NULL) backend_arg = NULL;
	if (tc->scenario == LOGGER_LIFECYCLE_SCENARIO_ENV_NULL) env_arg = NULL;
	if (tc->scenario == LOGGER_LIFECYCLE_SCENARIO_ENV_MEM_NULL) fx->env.mem = NULL;
	if (tc->scenario == LOGGER_LIFECYCLE_SCENARIO_VTBL_LOG_NULL) {
    	vtbl_local = fake_logger_vtbl;
    	vtbl_local.log = NULL;
		vtbl_arg = &vtbl_local;
	}
	if (tc->scenario == LOGGER_LIFECYCLE_SCENARIO_VTBL_DESTROY_NULL) {
    	vtbl_local = fake_logger_vtbl;
    	vtbl_local.destroy = NULL;
		vtbl_arg = &vtbl_local;
	}

    // ensure OUT_EXPECT_UNCHANGED is meaningful
    if (tc->out_expect == OUT_EXPECT_UNCHANGED && out_arg != NULL) {
        fx->out = (logger_t *)(uintptr_t)0xDEADC0DEu; // sentinel
    }

    logger_t *out_arg_snapshot = fx->out;

	// ACT
	ret = logger_create(out_arg, vtbl_arg, backend_arg, env_arg);
	if (tc->scenario == LOGGER_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT) {
		assert_int_equal(ret, LOGGER_STATUS_OK);
		assert_non_null(fx->out);

		logger_destroy(&fx->out);
		assert_null(fx->out);

		logger_destroy(&fx->out);
		assert_null(fx->out);
	}

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
		default: fail();
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_logger_lifecycle_case_t CASE_LOGGER_LIFECYCLE_OUT_NULL = {
	.name = "logger_lifecycle_out_null",

	.scenario = LOGGER_LIFECYCLE_SCENARIO_OUT_NULL,
	.fail_call_idx = 0,

	.expected_ret = LOGGER_STATUS_INVALID,
	.out_expect = OUT_CHECK_NONE
};

static const test_logger_lifecycle_case_t CASE_LOGGER_LIFECYCLE_VTBL_NULL = {
	.name = "logger_lifecycle_vtbl_null",

	.scenario = LOGGER_LIFECYCLE_SCENARIO_VTBL_NULL,
	.fail_call_idx = 0,

	.expected_ret = LOGGER_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_logger_lifecycle_case_t CASE_LOGGER_LIFECYCLE_VTBL_LOG_NULL = {
	.name = "logger_lifecycle_vtbl_log_null",

	.scenario = LOGGER_LIFECYCLE_SCENARIO_VTBL_LOG_NULL,
	.fail_call_idx = 0,

	.expected_ret = LOGGER_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_logger_lifecycle_case_t CASE_LOGGER_LIFECYCLE_VTBL_DESTROY_NULL = {
	.name = "logger_lifecycle_vtbl_destroy_null",

	.scenario = LOGGER_LIFECYCLE_SCENARIO_VTBL_DESTROY_NULL,
	.fail_call_idx = 0,

	.expected_ret = LOGGER_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_logger_lifecycle_case_t CASE_LOGGER_LIFECYCLE_BACKEND_NULL = {
	.name = "logger_lifecycle_backend_null",

	.scenario = LOGGER_LIFECYCLE_SCENARIO_BACKEND_NULL,
	.fail_call_idx = 0,

	.expected_ret = LOGGER_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_logger_lifecycle_case_t CASE_LOGGER_LIFECYCLE_ENV_NULL = {
	.name = "logger_lifecycle_env_null",

	.scenario = LOGGER_LIFECYCLE_SCENARIO_ENV_NULL,
	.fail_call_idx = 0,

	.expected_ret = LOGGER_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_logger_lifecycle_case_t CASE_LOGGER_LIFECYCLE_ENV_MEM_NULL = {
	.name = "logger_lifecycle_mem_null",

	.scenario = LOGGER_LIFECYCLE_SCENARIO_ENV_MEM_NULL,
	.fail_call_idx = 0,

	.expected_ret = LOGGER_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_logger_lifecycle_case_t CASE_LOGGER_LIFECYCLE_OOM_1 = {
	.name = "logger_lifecycle_oom_1",

	.scenario = LOGGER_LIFECYCLE_SCENARIO_OOM,
	.fail_call_idx = 1,

	.expected_ret = LOGGER_STATUS_OOM,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_logger_lifecycle_case_t CASE_LOGGER_LIFECYCLE_OK = {
	.name = "logger_lifecycle_ok",

	.scenario = LOGGER_LIFECYCLE_SCENARIO_OK,
	.fail_call_idx = 0,

	.expected_ret = LOGGER_STATUS_OK,
	.out_expect = OUT_EXPECT_NON_NULL
};

static const test_logger_lifecycle_case_t CASE_LOGGER_LIFECYCLE_DESTROY_IDEMPOTENT = {
	.name = "logger_lifecycle_destroy_idempotent",

	.scenario = LOGGER_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT,
	.fail_call_idx = 0,

	.expected_ret = LOGGER_STATUS_OK,
	.out_expect = OUT_EXPECT_NULL
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define LOGGER_LIFECYCLE_CASES(X) \
X(CASE_LOGGER_LIFECYCLE_OUT_NULL) \
X(CASE_LOGGER_LIFECYCLE_VTBL_NULL) \
X(CASE_LOGGER_LIFECYCLE_VTBL_LOG_NULL) \
X(CASE_LOGGER_LIFECYCLE_VTBL_DESTROY_NULL) \
X(CASE_LOGGER_LIFECYCLE_BACKEND_NULL) \
X(CASE_LOGGER_LIFECYCLE_ENV_NULL) \
X(CASE_LOGGER_LIFECYCLE_ENV_MEM_NULL) \
X(CASE_LOGGER_LIFECYCLE_OOM_1) \
X(CASE_LOGGER_LIFECYCLE_OK) \
X(CASE_LOGGER_LIFECYCLE_DESTROY_IDEMPOTENT)

#define LOGGER_MAKE_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(logger_lifecycle, case_sym)

static const struct CMUnitTest logger_lifecycle_tests[] = {
	LOGGER_LIFECYCLE_CASES(LOGGER_MAKE_LIFECYCLE_TEST)
};

#undef LOGGER_LIFECYCLE_CASES
#undef LOGGER_MAKE_LIFECYCLE_TEST

/** @endcond */

/**
 * @brief Scenarios for `logger_log()`.
 *
 * logger_status_t logger_log(logger_t *l, const char *message);
 *
 * Precondition:
 * - If `l != NULL`, `l` has been created by `logger_create()` with
 *   `fake_logger_vtbl` and `fake_logger_backend_t`.
 *
 * Doubles:
 * - `fake_logger_backend_t`
 * - `fake_logger_vtbl`
 *
 * See also:
 * - @ref testing_foundation_logger_unit_logger_log "logger_log() unit tests section"
 * - @ref specifications_logger_log "logger_log() specifications"
 *
 * The scenarios below define the test oracle for `logger_log()`.
 */
typedef enum {
    /**
     * WHEN `l == NULL` and `message != NULL`
     * EXPECT:
     * - returns `LOGGER_STATUS_INVALID`
     */
    LOGGER_LOG_SCENARIO_L_NULL,

    /**
     * WHEN `l != NULL` and `message == NULL`
     * EXPECT:
     * - returns `LOGGER_STATUS_INVALID`
     */
    LOGGER_LOG_SCENARIO_MESSAGE_NULL,

    /**
     * WHEN `l != NULL` and `message != NULL`
     * AND `fake_logger_backend` is configured as:
     *   - `log_ret = LOGGER_STATUS_IO_ERROR`
     * EXPECT:
     * - calls `fake_logger_vtbl.log(fake_logger_backend, message)` exactly once
     * - does not call `fake_logger_vtbl.destroy`
     * - returns `LOGGER_STATUS_IO_ERROR`
     */
    LOGGER_LOG_SCENARIO_FORWARD_IO_ERROR_OK
} logger_log_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for the `logger_log()` contract.
 */
typedef struct {
	const char *name;

	logger_log_scenario_t scenario;

	logger_status_t expected_ret;
} test_logger_log_case_t;

/**
 * @brief Runtime fixture for `logger_log()` tests.
 *
 * Holds:
 * - the logger under test,
 * - the injected environment,
 * - the fake adapter backend,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	logger_t *logger;
	logger_env_t env;
	fake_logger_backend_t backend;
	const test_logger_log_case_t *tc;
} test_logger_log_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `logger_log()` tests.
 */
static int setup_logger_log(void **state)
{
	const test_logger_log_case_t *tc =
		(const test_logger_log_case_t *)(*state);
	test_logger_log_fixture_t *fx =
		(test_logger_log_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));

	fake_memory_reset();
	fake_logger_backend_reset(&fx->backend);

	// DI
	fx->env.mem = osal_mem_test_fake_ops();

	assert_int_equal(
		logger_create(&fx->logger, &fake_logger_vtbl, &fx->backend, &fx->env),
		LOGGER_STATUS_OK );
	fx->tc = tc;

	*state = fx;

	return 0;
}

/**
 * @brief Release the `logger_log()` test fixture and verify memory invariants.
 */
static int teardown_logger_log(void **state)
{
	test_logger_log_fixture_t *fx =
		(test_logger_log_fixture_t *)(*state);

	logger_destroy(&fx->logger);

	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
	assert_true(fake_memory_no_leak());

	free(fx);

	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one parametric test scenario for `logger_log()`.
 */
static void test_logger_log(void **state)
{
	test_logger_log_fixture_t *fx =
		(test_logger_log_fixture_t *)(*state);
	const test_logger_log_case_t *tc = fx->tc;

	// ARRANGE
	logger_status_t ret = (logger_status_t)-1; // poison
	logger_t *l_arg = fx->logger;
	const char *message_arg = "test message";

	// invalid args
	if (tc->scenario == LOGGER_LOG_SCENARIO_L_NULL) l_arg = NULL;
	if (tc->scenario == LOGGER_LOG_SCENARIO_MESSAGE_NULL) message_arg = NULL;

	// spy cfg
	if (tc->scenario == LOGGER_LOG_SCENARIO_FORWARD_IO_ERROR_OK) {
		fx->backend.log_ret = LOGGER_STATUS_IO_ERROR;
	}

	// ACT
	ret = logger_log(l_arg, message_arg);

	// ASSERT
	assert_int_equal(ret, tc->expected_ret);

	if (tc->scenario == LOGGER_LOG_SCENARIO_FORWARD_IO_ERROR_OK) {
		assert_int_equal(fx->backend.log_called, 1);
		assert_int_equal(fx->backend.destroy_called, 0);
		assert_ptr_equal(fx->backend.last_backend, &fx->backend);
		assert_ptr_equal(fx->backend.last_message, message_arg);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_logger_log_case_t CASE_LOGGER_LOG_L_NULL = {
	.name = "logger_log_l_null",

	.scenario = LOGGER_LOG_SCENARIO_L_NULL,

	.expected_ret = LOGGER_STATUS_INVALID
};

static const test_logger_log_case_t CASE_LOGGER_LOG_MESSAGE_NULL = {
	.name = "logger_log_message_null",

	.scenario = LOGGER_LOG_SCENARIO_MESSAGE_NULL,

	.expected_ret = LOGGER_STATUS_INVALID
};


static const test_logger_log_case_t CASE_LOGGER_LOG_FORWARD_IO_ERROR_OK = {
	.name = "logger_log_forward_io_error_ok",

	.scenario = LOGGER_LOG_SCENARIO_FORWARD_IO_ERROR_OK,

	.expected_ret = LOGGER_STATUS_IO_ERROR
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define LOGGER_LOG_CASES(X) \
X(CASE_LOGGER_LOG_L_NULL) \
X(CASE_LOGGER_LOG_MESSAGE_NULL) \
X(CASE_LOGGER_LOG_FORWARD_IO_ERROR_OK)

#define LOGGER_MAKE_LOG_TEST(case_sym) \
LEXLEO_MAKE_TEST(logger_log, case_sym)

static const struct CMUnitTest logger_log_tests[] = {
	LOGGER_LOG_CASES(LOGGER_MAKE_LOG_TEST)
};

#undef LOGGER_LOG_CASES
#undef LOGGER_MAKE_LOG_TEST

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void)
{
	static const struct CMUnitTest logger_unit_non_parametric_tests[] = {
		cmocka_unit_test(test_logger_default_env)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(logger_unit_non_parametric_tests, NULL, NULL);
	failed += cmocka_run_group_tests(logger_lifecycle_tests, NULL, NULL);
	failed += cmocka_run_group_tests(logger_log_tests, NULL, NULL);
	return failed;
}

/** @endcond */
