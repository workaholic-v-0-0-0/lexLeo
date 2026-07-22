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

#include "logger/borrowers/logger_borrowers_api.h"
#include "logger/owners/logger_owners_api.h"
#include "logger/adapters/logger_adapters_api.h"

#include "logger/tests/logger_white_box_tests_access.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"

#include "lexleo_cmocka.h"

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
	logger_status_t destroy_ret;
} fake_logger_backend_t;

static void fake_logger_backend_reset(fake_logger_backend_t *b) {
	assert_non_null(b);
	b->log_called = 0;
	b->destroy_called = 0;
	b->last_backend = NULL;
	b->last_message = NULL;
	b->log_ret = LOGGER_STATUS_INVALID;
	b->destroy_ret = LOGGER_STATUS_OK;
}

static logger_status_t fake_logger_log(void *backend, const char *message) {
	fake_logger_backend_t *b = (fake_logger_backend_t *)backend;
    assert_non_null(b);

	b->last_backend = backend;
	b->last_message = message;

	b->log_called++;

	return b->log_ret;
}

static logger_status_t fake_logger_destroy(void *backend) {
    fake_logger_backend_t *b = (fake_logger_backend_t *)backend;
    assert_non_null(b);

	b->last_backend = backend;

	b->destroy_called++;

	return b->destroy_ret;
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
 * Doubles:
 * - dummy `logger_vtbl_t`
 * - dummy `osal_mem_ops_t`
 *
 * See also:
 * - @ref testing_foundation_logger_unit_logger_default_env "logger_default_env() unit tests section"
 * - @ref specifications_logger_default_env "logger_default_env() specifications".
 */
static void test_logger_default_env(void **state) {
	(void)state;

	const logger_vtbl_t dummy_vtbl = {0};
	const logger_vtbl_t *dummy_vtbl_p = &dummy_vtbl;

	const osal_mem_ops_t dummy_mem_ops = {0};
	const osal_mem_ops_t *dummy_mem_ops_p = &dummy_mem_ops;

	logger_env_t ret =
		logger_default_env(
			dummy_vtbl_p,
			dummy_mem_ops_p
		);

	assert_ptr_equal(ret.mem, dummy_mem_ops_p);
	assert_ptr_equal(ret.vtbl, dummy_vtbl_p);
}

/**
 * @brief Scenarios for `logger_create()` / `logger_complete_default_init` /
 * `logger_destroy()`.
 *
 * Doubles:
 * - fake_memory
 * - fake_stream
 *
 * See contract:
 * - @ref specifications_logger_create "logger_create() specifications"
 * - @ref specifications_logger_complete_default_init "logger_complete_default_init() specifications"
 * - @ref specifications_logger_destroy "logger_destroy() specifications"
 *
 * See test description:
 * - @ref testing_foundation_logger_unit_logger_create_logger_destroy "logger_create() / logger_destroy() unit tests section"
 *
 * The scenarios below define the test oracle for `logger_create()` and `logger_destroy()`.
 */
typedef enum {
	/**
	 * WHEN the nominal logger lifecycle is executed:
	 * - `logger_create(out, env)`
	 * - `logger_complete_default_init(*out, backend)`
	 * - `logger_destroy(out)`
	 *
	 * EXPECT:
	 * - `logger_create()` returns `LOGGER_STATUS_OK`
	 * - `*out != NULL`
	 * - `(*out)->vtbl == env->vtbl`
	 * - `(*out)->mem == env->mem`
	 * - `(*out)->backend == NULL`
	 * - `logger_complete_default_init()` returns `LOGGER_STATUS_OK`
	 * - `(*out)->backend == backend`
	 * - `logger_destroy()` invokes the backend destroy operation
	 * - `logger_destroy()` releases the logger handle
	 * - `logger_destroy()` sets `*out` to `NULL`
	 * - no memory leak, invalid free or double free
	 */
	LOGGER_LIFECYCLE_SCENARIO_OK = 0,

	/**
	 * WHEN allocation of the logger handle fails during
	 * `logger_create(out, env)`
	 *
	 * EXPECT:
	 * - `logger_create()` returns `LOGGER_STATUS_OOM`
	 * - `*out == NULL`
	 * - no backend destroy operation is invoked
	 * - no memory leak, invalid free or double free
	 */
	LOGGER_LIFECYCLE_SCENARIO_OOM,

	/**
	 * WHEN `logger_destroy(out)` is called twice after a successfully completed
	 * logger lifecycle
	 *
	 * EXPECT:
	 * - the first call invokes the backend destroy operation
	 * - the first call releases the logger handle
	 * - the first call sets `*out` to `NULL`
	 * - the second call leaves `*out == NULL`
	 * - the backend destroy operation is invoked exactly once
	 * - no memory leak, invalid free or double free
	 */
	LOGGER_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT,

} logger_lifecycle_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for the logger lifecycle contract.
 */
typedef struct {
	const char *name;
	logger_lifecycle_scenario_t scenario;
	size_t fail_call_idx;
} test_logger_lifecycle_case_t;

/**
 * @brief Runtime fixture for logger lifecycle contract.
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
	const test_logger_lifecycle_case_t *tc = (const test_logger_lifecycle_case_t *)(*state);
	test_logger_lifecycle_fixture_t *fx = (test_logger_lifecycle_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;
	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();
	if (tc->scenario == LOGGER_LIFECYCLE_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	// borrowed DI
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.vtbl = &fake_logger_vtbl;

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

	osal_free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one parametric test scenario for the logger lifecycle contract.
 */
static void test_logger_lifecycle(void **state)
{
	test_logger_lifecycle_fixture_t *fx = (test_logger_lifecycle_fixture_t *)(*state);
	const test_logger_lifecycle_case_t *tc = fx->tc;

	// ARRANGE logger_create()
	logger_status_t ret = LOGGER_STATUS_INVALID;

	// ACT logger_create()
	ret = logger_create(&fx->out, &fx->env);

	// ASSERT logger_create()
	if (tc->scenario == LOGGER_LIFECYCLE_SCENARIO_OOM) {
		assert_int_equal(ret, LOGGER_STATUS_OOM);
		assert_null(fx->out);
		return;
	}
	assert_int_equal(ret, LOGGER_STATUS_OK);
	assert_non_null(fx->out);
	assert_null(logger_get_backend(fx->out));

	// ACT logger_complete_default_init()
	ret = logger_complete_default_init(fx->out, &fx->backend);

	// ASSERT logger_complete_default_init()
	assert_int_equal(ret, LOGGER_STATUS_OK);
	assert_non_null(fx->out);
	assert_ptr_equal(logger_get_backend(fx->out), &fx->backend);

	// ACT logger_destroy()
	ret = logger_destroy(&fx->out);

	// ASSERT logger_destroy()
	assert_int_equal(ret, LOGGER_STATUS_OK);
	assert_null(fx->out);
	if (tc->scenario == LOGGER_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT) {
		assert_int_equal(fx->backend.destroy_called, 1);

		// ACT second logger_destroy()
		ret = logger_destroy(&fx->out);

		// ASSERT second logger_destroy()
		assert_int_equal(ret, LOGGER_STATUS_OK);
		assert_null(fx->out);
		assert_int_equal(fx->backend.destroy_called, 1);
	}
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_logger_lifecycle_case_t CASE_LOGGER_LIFECYCLE_OOM = {
	.name = "logger_lifecycle_oom",
	.scenario = LOGGER_LIFECYCLE_SCENARIO_OOM,
	.fail_call_idx = 1,
};

static const test_logger_lifecycle_case_t CASE_LOGGER_LIFECYCLE_DESTROY_IDEMPOTENT = {
	.name = "logger_lifecycle_destroy_idempotent",
	.scenario = LOGGER_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT,
	.fail_call_idx = 0,
};

static const test_logger_lifecycle_case_t CASE_LOGGER_LIFECYCLE_OK = {
	.name = "logger_lifecycle_ok",
	.scenario = LOGGER_LIFECYCLE_SCENARIO_OK,
	.fail_call_idx = 0,
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define LOGGER_LIFECYCLE_CASES(X) \
X(CASE_LOGGER_LIFECYCLE_OOM) \
X(CASE_LOGGER_LIFECYCLE_DESTROY_IDEMPOTENT) \
X(CASE_LOGGER_LIFECYCLE_OK)

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
 * See contract:
 * - @ref specifications_logger_log "logger_log() specifications"
 *
 * See test description:
 * - @ref testing_foundation_logger_unit_logger_log "logger_log() unit tests section"
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
	const test_logger_log_case_t *tc = (const test_logger_log_case_t *)(*state);
	test_logger_log_fixture_t *fx = (test_logger_log_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;
	osal_memset(fx, 0, sizeof(*fx));

	fake_memory_reset();
	fake_logger_backend_reset(&fx->backend);

	// borrowed DI
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.vtbl = &fake_logger_vtbl;

	assert_int_equal(
		logger_create(&fx->logger, &fx->env),
		LOGGER_STATUS_OK
	);

	// owned DI
	assert_int_equal(
		logger_complete_default_init(fx->logger, &fx->backend),
		LOGGER_STATUS_OK
	);

	fx->tc = tc;

	*state = fx;
	return 0;
}

/**
 * @brief Release the `logger_log()` test fixture and verify memory invariants.
 */
static int teardown_logger_log(void **state)
{
	test_logger_log_fixture_t *fx = (test_logger_log_fixture_t *)(*state);

	logger_destroy(&fx->logger);

	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
	assert_true(fake_memory_no_leak());

	osal_free(fx);

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
	test_logger_log_fixture_t *fx = (test_logger_log_fixture_t *)(*state);
	const test_logger_log_case_t *tc = fx->tc;

	// ARRANGE
	logger_status_t ret = (logger_status_t)-1; // poison
	logger_t *l_arg = fx->logger;
	const char *message_arg = "test message";

	// invalid args
	if (tc->scenario == LOGGER_LOG_SCENARIO_L_NULL) {
		l_arg = NULL;
	}
	if (tc->scenario == LOGGER_LOG_SCENARIO_MESSAGE_NULL) {
		message_arg = NULL;
	}

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
