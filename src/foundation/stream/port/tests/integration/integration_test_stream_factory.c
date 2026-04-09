/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file integration_test_stream_factory.c
 * @ingroup stream_integration_tests
 * @brief Integration tests implementation for the stream factory.
 *
 * @details
 * This file implements integration-level validation of the stream factory
 * lifecycle and adapter-registration contracts.
 *
 * Covered surfaces:
 * - factory lifecycle: `stream_create_factory()`, `stream_destroy_factory()`
 * - adapter registration: `stream_factory_add_adapter()`
 *
 * Test strategy:
 * - parametric scenario-based testing
 * - explicit validation of argument checking and lifecycle behavior
 * - explicit validation of adapter registration outcomes
 * - indirect observation through `stream_factory_create_stream()`
 *   when registration effects must be verified
 *
 * Local test doubles:
 * - `test_stream_ctor_1()`
 * - `test_stream_ctor_2()`
 * - `make_test_desc()`
 *
 * See also:
 * - @ref testing_foundation_stream_factory_integration "stream factory integration tests page"
 * - @ref specifications_stream "stream specifications"
 */

#include "stream/cr/stream_factory_cr_api.h"

#include "osal/mem/osal_mem.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/** @cond INTERNAL */

//-----------------------------------------------------------------------------
// LOCAL TEST DOUBLES
//-----------------------------------------------------------------------------

#define TEST_STREAM_SENTINEL_1 ((stream_t *)0x1111)
#define TEST_STREAM_SENTINEL_2 ((stream_t *)0x2222)

/**
 * @brief Minimal test adapter constructor #1 used by factory integration tests.
 *
 * @details
 * Stores a fixed non-NULL sentinel pointer into `*out` when `out` is valid.
 * This allows tests to distinguish resolution of the first registered
 * descriptor from resolution of other test descriptors.
 */
static stream_status_t test_stream_ctor_1(
	const void *ud,
	const void *args,
	stream_t **out)
{
	(void)ud;
	(void)args;

	if (!out) return STREAM_STATUS_INVALID;

	*out = TEST_STREAM_SENTINEL_1;
	return STREAM_STATUS_OK;
}

/**
 * @brief Minimal test adapter constructor #2 used by factory integration tests.
 *
 * @details
 * Stores a different fixed non-NULL sentinel pointer into `*out` when `out`
 * is valid. This allows tests to verify which registered descriptor is
 * actually resolved by the factory.
 */
static stream_status_t test_stream_ctor_2(
	const void *ud,
	const void *args,
	stream_t **out)
{
	(void)ud;
	(void)args;

	if (!out) return STREAM_STATUS_INVALID;

	*out = TEST_STREAM_SENTINEL_2;
	return STREAM_STATUS_OK;
}

/**
 * @brief Build a test adapter descriptor for factory integration tests.
 *
 * @details
 * This local helper returns a minimal `stream_adapter_desc_t` suitable for
 * exercising generic factory registration and lookup behavior.
 *
 * The returned descriptor:
 * - uses the provided adapter key,
 * - binds the provided constructor,
 * - carries no constructor user data,
 * - requires no user-data destructor.
 *
 * It is intended only for test scenarios that validate generic factory
 * behavior independently of any real stream adapter implementation.
 */
static stream_adapter_desc_t make_test_desc(
	stream_key_t key,
	stream_ctor_fn_t ctor)
{
	stream_adapter_desc_t d = {0};

	d.key = key;
	d.ctor = ctor;
	d.ud = NULL;
	d.ud_dtor = NULL;

	return d;
}

/** @endcond */

/**
 * @brief Scenarios for `stream_create_factory()` / `stream_destroy_factory()`.
 *
 * stream_status_t stream_create_factory(
 *     stream_factory_t **out,
 *     const stream_factory_cfg_t *cfg,
 *     const stream_env_t *env );
 *
 * void stream_destroy_factory(stream_factory_t **fact);
 *
 * Invalid arguments:
 * - `out`, `cfg`, and `env` must not be NULL.
 * - `env->mem` must not be NULL.
 * - `env->mem->calloc` and `env->mem->free` must not be NULL.
 *
 * Success:
 * - Returns STREAM_STATUS_OK.
 * - Stores a valid factory handle in `*out`.
 * - The produced factory must be destroyed via `stream_destroy_factory()`.
 *
 * Failure:
 * - Returns:
 *     - STREAM_STATUS_INVALID for invalid arguments
 * - Leaves `*out` unchanged if `out` is not NULL.
 *
 * Lifecycle:
 * - `stream_destroy_factory()` does nothing if `fact` is NULL or `*fact` is NULL.
 * - Otherwise, it releases the factory object and sets `*fact` to NULL.
 *
 * Doubles:
 * - none
 *
 * See also:
 * - @ref testing_foundation_stream_integration_stream_create_factory_stream_destroy_factory "stream_create_factory() / stream_destroy_factory() integration tests section"
 * - @ref specifications_stream_create_factory "stream_create_factory() specifications"
 * - @ref specifications_stream_destroy_factory "stream_destroy_factory() specifications"
 *
 * The scenarios below define the test oracle for
 * `stream_create_factory()` and `stream_destroy_factory()`.
 */
typedef enum {
    /**
     * WHEN `stream_create_factory(out, cfg, env)` is called with valid arguments
     * EXPECT:
     * - returns `STREAM_STATUS_OK`
     * - stores a non-NULL factory handle in `*out`
     * - the produced handle is eligible for destruction by
     *   `stream_destroy_factory()`
     */
    STREAM_FACT_LIFECYCLE_SCENARIO_OK = 0,

    /**
     * WHEN `out == NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     * - no factory handle is produced
     */
    STREAM_FACT_LIFECYCLE_SCENARIO_OUT_NULL,

    /**
     * WHEN `cfg == NULL` and `out != NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     * - leaves `*out` unchanged
     */
    STREAM_FACT_LIFECYCLE_SCENARIO_CFG_NULL,

    /**
     * WHEN `env == NULL` and `out != NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     * - leaves `*out` unchanged
     */
    STREAM_FACT_LIFECYCLE_SCENARIO_ENV_NULL,

    /**
     * WHEN `env != NULL` but `env->mem == NULL` and `out != NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     * - leaves `*out` unchanged
     */
    STREAM_FACT_LIFECYCLE_SCENARIO_ENV_MEM_NULL,

    /**
     * WHEN `env->mem != NULL` but `env->mem->calloc == NULL`
     * AND `out != NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     * - leaves `*out` unchanged
     */
    STREAM_FACT_LIFECYCLE_SCENARIO_ENV_MEM_CALLOC_NULL,

    /**
     * WHEN `env->mem != NULL` but `env->mem->free == NULL`
     * AND `out != NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     * - leaves `*out` unchanged
     */
    STREAM_FACT_LIFECYCLE_SCENARIO_ENV_MEM_FREE_NULL,

    /**
     * WHEN `stream_create_factory()` succeeds and
     * `stream_destroy_factory()` is called twice
     * EXPECT:
     * - first `stream_destroy_factory(&fact)` releases the handle and sets
     *   `fact` to NULL
     * - second `stream_destroy_factory(&fact)` is a no-op and keeps `fact`
     *   as NULL
     *
     * Notes:
     * - This scenario checks the idempotence guarantee of
     *   `stream_destroy_factory()`.
     */
    STREAM_FACT_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT
} stream_fact_lifecycle_scenario_t;

/** @cond INTERNAL */

/**
 * @brief Expected state of the output handle after the call under test.
 *
 * Notes:
 * - `OUT_CHECK_NONE` disables post-call checks on the output handle.
 * - `OUT_EXPECT_UNCHANGED` is typically verified using a sentinel pointer.
 */
typedef enum {
	OUT_CHECK_NONE,
	OUT_EXPECT_NULL,
	OUT_EXPECT_NON_NULL,
	OUT_EXPECT_UNCHANGED
} out_expect_t;

/**
 * @brief One parametric test case for the stream factory lifecycle contract.
 *
 * Holds:
 * - the case name used by the test runner,
 * - the selected lifecycle scenario,
 * - the expected return status,
 * - the expected post-call state of the output handle.
 */
typedef struct {
	const char *name;

	// arrange
	stream_fact_lifecycle_scenario_t scenario;

	// assert
	stream_status_t expected_ret;
	out_expect_t out_expect;
} test_stream_fact_lifecycle_case_t;

/**
 * @brief Runtime fixture for `stream_create_factory()` / `stream_destroy_factory()` tests.
 *
 * Holds:
 * - the factory handle under test,
 * - the injected factory configuration,
 * - the injected factory environment,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resource
	stream_factory_t *out;

	// injection
	stream_factory_cfg_t stream_factory_cfg;
	stream_env_t stream_env;

	// reference to test case
	const test_stream_fact_lifecycle_case_t *tc;
} test_stream_fact_lifecycle_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for
 * `stream_create_factory()` / `stream_destroy_factory()` tests.
 */
static int setup_stream_fact_lifecycle(void **state)
{
	const test_stream_fact_lifecycle_case_t *tc =
		(const test_stream_fact_lifecycle_case_t *)(*state);

	test_stream_fact_lifecycle_fixture_t *fx =
		(test_stream_fact_lifecycle_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fx->stream_factory_cfg.fact_cap = 8;

	// DI
	fx->stream_env.mem = osal_mem_default_ops();

	*state = fx;
	return 0;
}

/**
 * @brief Release the factory lifecycle test fixture.
 */
static int teardown_stream_fact_lifecycle(void **state)
{
	test_stream_fact_lifecycle_fixture_t *fx =
		(test_stream_fact_lifecycle_fixture_t *)(*state);

	if (fx->out) {
		stream_destroy_factory(&fx->out);
		fx->out = NULL;
	}

	free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute the active parametric test scenario for
 * `stream_create_factory()` / `stream_destroy_factory()`.
 */
static void test_stream_fact_lifecycle(void **state)
{
	test_stream_fact_lifecycle_fixture_t *fx =
		(test_stream_fact_lifecycle_fixture_t *)(*state);
	const test_stream_fact_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t ret = STREAM_STATUS_INVALID;

	stream_factory_t **out_arg = &fx->out;
	const stream_factory_cfg_t *cfg_arg = &fx->stream_factory_cfg;
	stream_env_t env = fx->stream_env;
	const stream_env_t *env_arg = &env;
	osal_mem_ops_t mem_ops;
	bool use_mem_ops_copy = false;

	// invalid args
	if (tc->scenario == STREAM_FACT_LIFECYCLE_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == STREAM_FACT_LIFECYCLE_SCENARIO_CFG_NULL) cfg_arg = NULL;
	if (tc->scenario == STREAM_FACT_LIFECYCLE_SCENARIO_ENV_NULL) env_arg = NULL;
	if (tc->scenario == STREAM_FACT_LIFECYCLE_SCENARIO_ENV_MEM_NULL) env.mem = NULL;
	if (tc->scenario == STREAM_FACT_LIFECYCLE_SCENARIO_ENV_MEM_CALLOC_NULL) {
		mem_ops = *fx->stream_env.mem;
	    use_mem_ops_copy = true;
		mem_ops.calloc = NULL;
	}
	if (tc->scenario == STREAM_FACT_LIFECYCLE_SCENARIO_ENV_MEM_FREE_NULL)  {
		mem_ops = *fx->stream_env.mem;
	    use_mem_ops_copy = true;
		mem_ops.free = NULL;
	}

	if (use_mem_ops_copy) {
		env.mem = &mem_ops;
	}

    // ensure OUT_EXPECT_UNCHANGED is meaningful
    if (tc->out_expect == OUT_EXPECT_UNCHANGED && out_arg != NULL) {
        fx->out = (stream_factory_t *)(uintptr_t)0xDEADC0DEu; // sentinel
    }

    stream_factory_t *out_arg_snapshot = fx->out;

	// ACT
	ret = stream_create_factory(out_arg, cfg_arg, env_arg);
	if (tc->scenario == STREAM_FACT_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT) {
	    assert_int_equal(ret, STREAM_STATUS_OK);
    	assert_non_null(fx->out);

    	stream_destroy_factory(&fx->out);
    	assert_null(fx->out);

    	stream_destroy_factory(&fx->out);
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

static const test_stream_fact_lifecycle_case_t CASE_STREAM_FACT_LIFECYCLE_OK = {
	.name = "stream_fact_lifecycle_ok",
	.scenario = STREAM_FACT_LIFECYCLE_SCENARIO_OK,

	.expected_ret = STREAM_STATUS_OK,
	.out_expect = OUT_EXPECT_NON_NULL
};

static const test_stream_fact_lifecycle_case_t CASE_STREAM_FACT_LIFECYCLE_OUT_NULL = {
	.name = "stream_fact_lifecycle_out_null",
	.scenario = STREAM_FACT_LIFECYCLE_SCENARIO_OUT_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_CHECK_NONE
};

static const test_stream_fact_lifecycle_case_t CASE_STREAM_FACT_LIFECYCLE_CFG_NULL = {
	.name = "stream_fact_lifecycle_cfg_null",
	.scenario = STREAM_FACT_LIFECYCLE_SCENARIO_CFG_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_lifecycle_case_t CASE_STREAM_FACT_LIFECYCLE_ENV_NULL = {
	.name = "stream_fact_lifecycle_env_null",
	.scenario = STREAM_FACT_LIFECYCLE_SCENARIO_ENV_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_lifecycle_case_t CASE_STREAM_FACT_LIFECYCLE_ENV_MEM_NULL = {
	.name = "stream_fact_lifecycle_env_mem_null",
	.scenario = STREAM_FACT_LIFECYCLE_SCENARIO_ENV_MEM_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_lifecycle_case_t CASE_STREAM_FACT_LIFECYCLE_ENV_MEM_CALLOC_NULL = {
	.name = "stream_fact_lifecycle_env_mem_calloc_null",
	.scenario = STREAM_FACT_LIFECYCLE_SCENARIO_ENV_MEM_CALLOC_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_lifecycle_case_t CASE_STREAM_FACT_LIFECYCLE_ENV_MEM_FREE_NULL = {
	.name = "stream_fact_lifecycle_env_mem_free_null",
	.scenario = STREAM_FACT_LIFECYCLE_SCENARIO_ENV_MEM_FREE_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_fact_lifecycle_case_t CASE_STREAM_FACT_LIFECYCLE_DESTROY_IDEMPOTENT = {
	.name = "stream_fact_lifecycle_destroy_idempotent",
	.scenario = STREAM_FACT_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT,

	.expected_ret = STREAM_STATUS_OK,
	.out_expect = OUT_EXPECT_NULL
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STREAM_FACT_LIFECYCLE_CASES(X) \
X(CASE_STREAM_FACT_LIFECYCLE_OK) \
X(CASE_STREAM_FACT_LIFECYCLE_OUT_NULL) \
X(CASE_STREAM_FACT_LIFECYCLE_CFG_NULL) \
X(CASE_STREAM_FACT_LIFECYCLE_ENV_NULL) \
X(CASE_STREAM_FACT_LIFECYCLE_ENV_MEM_NULL) \
X(CASE_STREAM_FACT_LIFECYCLE_ENV_MEM_CALLOC_NULL) \
X(CASE_STREAM_FACT_LIFECYCLE_ENV_MEM_FREE_NULL) \
X(CASE_STREAM_FACT_LIFECYCLE_DESTROY_IDEMPOTENT)

#define STREAM_MAKE_FACT_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_fact_lifecycle, case_sym)

static const struct CMUnitTest fact_lifecycle_tests[] = {
	STREAM_FACT_LIFECYCLE_CASES(STREAM_MAKE_FACT_LIFECYCLE_TEST)
};

#undef STREAM_FACT_LIFECYCLE_CASES
#undef STREAM_MAKE_FACT_LIFECYCLE_TEST

/** @endcond */

/**
 * @brief Scenarios for `stream_factory_add_adapter()`.
 *
 * stream_status_t stream_factory_add_adapter(
 *     stream_factory_t *fact,
 *     const stream_adapter_desc_t *desc );
 *
 * Precondition:
 * - `fact` designates a valid factory instance previously created by
 *   `stream_create_factory()`.
 *
 * Invalid arguments:
 * - `fact` and `desc` must not be NULL.
 * - `desc->key` and `desc->ctor` must not be NULL.
 * - `desc->key` must not be empty.
 *
 * Success:
 * - Returns STREAM_STATUS_OK.
 * - A later call to `stream_factory_create_stream()` with the registered key
 *   can resolve the descriptor.
 *
 * Failure:
 * - Returns:
 *     - STREAM_STATUS_INVALID for invalid arguments
 *     - STREAM_STATUS_ALREADY_EXISTS if the key is already registered
 *     - STREAM_STATUS_FULL if the factory capacity is exhausted
 *
 * Doubles:
 * - none
 *
 * See also:
 * - @ref testing_foundation_stream_integration_stream_factory_add_adapter "stream_factory_add_adapter() integration tests section"
 * - @ref specifications_stream_factory_add_adapter "stream_factory_add_adapter() specifications"
 *
 * The scenarios below define the test oracle for
 * `stream_factory_add_adapter()`.
 */
typedef enum {
    /**
     * WHEN `stream_factory_add_adapter(fact, desc)` is called with a valid
     * factory, a valid descriptor whose key is not yet registered, and a
     * factory that has not reached its registration capacity
     * EXPECT:
     * - returns `STREAM_STATUS_OK`
     * - a later call to `stream_factory_create_stream()` with that key returns
     *   `STREAM_STATUS_OK`
     * - the produced stream corresponds to the registered descriptor
     */
    STREAM_FACT_ADD_ADAPTER_SCENARIO_OK = 0,

    /**
     * WHEN `fact == NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     */
    STREAM_FACT_ADD_ADAPTER_SCENARIO_FACT_NULL,

    /**
     * WHEN `desc == NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     */
    STREAM_FACT_ADD_ADAPTER_SCENARIO_DESC_NULL,

    /**
     * WHEN `desc != NULL` but `desc->key == NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     */
    STREAM_FACT_ADD_ADAPTER_SCENARIO_KEY_NULL,

    /**
     * WHEN `desc != NULL` but `desc->key` is an empty string
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     */
    STREAM_FACT_ADD_ADAPTER_SCENARIO_KEY_EMPTY,

    /**
     * WHEN `desc != NULL` but `desc->ctor == NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     */
    STREAM_FACT_ADD_ADAPTER_SCENARIO_CTOR_NULL,

    /**
     * WHEN a descriptor is added with a key that is already registered
     * EXPECT:
     * - returns `STREAM_STATUS_ALREADY_EXISTS`
     * - a later call to `stream_factory_create_stream()` with that key returns
     *   `STREAM_STATUS_OK`
     * - the produced stream corresponds to the previously registered descriptor
     */
    STREAM_FACT_ADD_ADAPTER_SCENARIO_DUPLICATE_KEY,

    /**
     * WHEN the factory has reached its registration capacity and a new
     * descriptor is added
     * EXPECT:
     * - returns `STREAM_STATUS_FULL`
	 * - a later call to `stream_factory_create_stream()` with the new key
 	 *   returns `STREAM_STATUS_NOT_FOUND`
     * - no stream handle is produced
     */
    STREAM_FACT_ADD_ADAPTER_SCENARIO_CAP_REACHED
} stream_fact_add_adapter_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `stream_factory_add_adapter()`.
 *
 * Holds:
 * - the case name used by the test runner,
 * - the selected adapter-registration scenario,
 * - the expected return status,
 * - whether post-registration resolution must be checked,
 * - the expected constructor result when resolution is checked.
 */
typedef struct {
	const char *name;

	// arrange
	stream_fact_add_adapter_scenario_t scenario;

	// assert
	stream_status_t expected_ret;
	bool check_resolution;
	stream_status_t expected_create_ret;
	stream_t *expected_created_stream;
} test_stream_fact_add_adapter_case_t;

/**
 * @brief Runtime fixture for `stream_factory_add_adapter()` tests.
 *
 * Holds:
 * - the factory handle under test,
 * - the injected factory configuration,
 * - the injected factory environment,
 * - the adapter descriptors used by test scenarios,
 * - the stream handle produced by follow-up resolution checks,
 * - a dummy argument blob used for `stream_factory_create_stream()`,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	stream_factory_t *factory;
	stream_t *created_stream;

	// configuration
	stream_factory_cfg_t stream_factory_cfg;

	// injection
	stream_env_t stream_env;

	// descriptors under test
	stream_adapter_desc_t desc;
	stream_adapter_desc_t other_desc;

	// dummy args used by test ctors
	int dummy_args;

	// reference to test case
	const test_stream_fact_add_adapter_case_t *tc;
} test_stream_fact_add_adapter_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for
 * `stream_factory_add_adapter()` tests.
 */
static int setup_stream_fact_add_adapter(void **state)
{
	const test_stream_fact_add_adapter_case_t *tc =
		(const test_stream_fact_add_adapter_case_t *)(*state);

	test_stream_fact_add_adapter_fixture_t *fx =
		(test_stream_fact_add_adapter_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	// DI
	fx->stream_env.mem = osal_mem_default_ops();

	fx->stream_factory_cfg.fact_cap = (tc->scenario == STREAM_FACT_ADD_ADAPTER_SCENARIO_CAP_REACHED) ? 1 : 8;

	assert_int_equal(
		stream_create_factory(&fx->factory, &fx->stream_factory_cfg, &fx->stream_env),
		STREAM_STATUS_OK );

	// valid test descriptors used by scenarios
	fx->desc = make_test_desc("key_1", test_stream_ctor_1);
	fx->other_desc = make_test_desc("key_2", test_stream_ctor_2);

	*state = fx;
	return 0;
}

/**
 * @brief Release the `stream_factory_add_adapter()` test fixture.
 */
static int teardown_stream_fact_add_adapter(void **state)
{
	test_stream_fact_add_adapter_fixture_t *fx =
		(test_stream_fact_add_adapter_fixture_t *)(*state);

	if (fx->factory) {
		stream_destroy_factory(&fx->factory);
		fx->factory = NULL;
	}

	free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one test scenario for `stream_factory_add_adapter()`.
 */
static void test_stream_fact_add_adapter(void **state)
{
	test_stream_fact_add_adapter_fixture_t *fx =
		(test_stream_fact_add_adapter_fixture_t *)(*state);
	const test_stream_fact_add_adapter_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t ret = STREAM_STATUS_INVALID;

	stream_factory_t *fact_arg = fx->factory;
	const stream_adapter_desc_t *desc_arg = &fx->desc;
	stream_key_t resolution_key = fx->desc.key;

	if (tc->scenario == STREAM_FACT_ADD_ADAPTER_SCENARIO_DUPLICATE_KEY) {
		assert_int_equal(
			stream_factory_add_adapter(fact_arg, &fx->desc),
			STREAM_STATUS_OK );
		fx->other_desc.key = fx->desc.key;
		desc_arg = &fx->other_desc;
		resolution_key = fx->desc.key;
	}

	// invalid args
	if (tc->scenario == STREAM_FACT_ADD_ADAPTER_SCENARIO_FACT_NULL) fact_arg = NULL;
	if (tc->scenario == STREAM_FACT_ADD_ADAPTER_SCENARIO_DESC_NULL) desc_arg = NULL;
	if (tc->scenario == STREAM_FACT_ADD_ADAPTER_SCENARIO_KEY_NULL) fx->desc.key = NULL;
	if (tc->scenario == STREAM_FACT_ADD_ADAPTER_SCENARIO_KEY_EMPTY) fx->desc.key = "";
	if (tc->scenario == STREAM_FACT_ADD_ADAPTER_SCENARIO_CTOR_NULL) fx->desc.ctor = NULL;

	// make factory full
	if (tc->scenario == STREAM_FACT_ADD_ADAPTER_SCENARIO_CAP_REACHED) {
		assert_int_equal(
			stream_factory_add_adapter(fact_arg, &fx->desc),
			STREAM_STATUS_OK );
		desc_arg = &fx->other_desc;
		resolution_key = fx->other_desc.key;
	}

	// ACT
	ret = stream_factory_add_adapter(fact_arg, desc_arg);

	// ASSERT
	assert_int_equal(ret, tc->expected_ret);
	if (tc->check_resolution) {
		assert_null(fx->created_stream);
		assert_int_equal(
			stream_factory_create_stream(
				fact_arg,
				resolution_key,
				(void*)&fx->dummy_args,
				&fx->created_stream ),
			tc->expected_create_ret );
		assert_ptr_equal(
			fx->created_stream,
			tc->expected_created_stream );
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_stream_fact_add_adapter_case_t CASE_STREAM_FACT_ADD_ADAPTER_FACT_NULL = {
	.name = "stream_fact_add_adapter_fact_null",
	.scenario = STREAM_FACT_ADD_ADAPTER_SCENARIO_FACT_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.check_resolution = false,
};

static const test_stream_fact_add_adapter_case_t CASE_STREAM_FACT_ADD_ADAPTER_DESC_NULL = {
	.name = "stream_fact_add_adapter_desc_null",
	.scenario = STREAM_FACT_ADD_ADAPTER_SCENARIO_DESC_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.check_resolution = false,
};

static const test_stream_fact_add_adapter_case_t CASE_STREAM_FACT_ADD_ADAPTER_KEY_NULL = {
	.name = "stream_fact_add_adapter_key_null",
	.scenario = STREAM_FACT_ADD_ADAPTER_SCENARIO_KEY_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.check_resolution = false,
};

static const test_stream_fact_add_adapter_case_t CASE_STREAM_FACT_ADD_ADAPTER_KEY_EMPTY = {
	.name = "stream_fact_add_adapter_key_empty",
	.scenario = STREAM_FACT_ADD_ADAPTER_SCENARIO_KEY_EMPTY,

	.expected_ret = STREAM_STATUS_INVALID,
	.check_resolution = false,
};

static const test_stream_fact_add_adapter_case_t CASE_STREAM_FACT_ADD_ADAPTER_CTOR_NULL = {
	.name = "stream_fact_add_adapter_ctor_null",
	.scenario = STREAM_FACT_ADD_ADAPTER_SCENARIO_CTOR_NULL,

	.expected_ret = STREAM_STATUS_INVALID,
	.check_resolution = false,
};

static const test_stream_fact_add_adapter_case_t CASE_STREAM_FACT_ADD_ADAPTER_DUPLICATE_KEY = {
	.name = "stream_fact_add_adapter_duplicate_key",
	.scenario = STREAM_FACT_ADD_ADAPTER_SCENARIO_DUPLICATE_KEY,

	.expected_ret = STREAM_STATUS_ALREADY_EXISTS,
	.check_resolution = true,
	.expected_create_ret = STREAM_STATUS_OK,
	.expected_created_stream = TEST_STREAM_SENTINEL_1,
};

static const test_stream_fact_add_adapter_case_t CASE_STREAM_FACT_ADD_ADAPTER_CAP_REACHED = {
	.name = "stream_fact_add_adapter_cap_reached",
	.scenario = STREAM_FACT_ADD_ADAPTER_SCENARIO_CAP_REACHED,

	.expected_ret = STREAM_STATUS_FULL,
	.check_resolution = true,
	.expected_create_ret = STREAM_STATUS_NOT_FOUND,
	.expected_created_stream = NULL,
};

static const test_stream_fact_add_adapter_case_t CASE_STREAM_FACT_ADD_ADAPTER_OK = {
	.name = "stream_fact_add_adapter_ok",
	.scenario = STREAM_FACT_ADD_ADAPTER_SCENARIO_OK,

	.expected_ret = STREAM_STATUS_OK,
	.check_resolution = true,
	.expected_create_ret = STREAM_STATUS_OK,
	.expected_created_stream = TEST_STREAM_SENTINEL_1,
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STREAM_FACT_ADD_ADAPTER_CASES(X) \
X(CASE_STREAM_FACT_ADD_ADAPTER_FACT_NULL) \
X(CASE_STREAM_FACT_ADD_ADAPTER_DESC_NULL) \
X(CASE_STREAM_FACT_ADD_ADAPTER_KEY_NULL) \
X(CASE_STREAM_FACT_ADD_ADAPTER_KEY_EMPTY) \
X(CASE_STREAM_FACT_ADD_ADAPTER_CTOR_NULL) \
X(CASE_STREAM_FACT_ADD_ADAPTER_DUPLICATE_KEY) \
X(CASE_STREAM_FACT_ADD_ADAPTER_CAP_REACHED) \
X(CASE_STREAM_FACT_ADD_ADAPTER_OK)

#define STREAM_MAKE_STREAM_FACT_ADD_ADAPTER_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_fact_add_adapter, case_sym)

static const struct CMUnitTest stream_fact_add_adapter_tests[] = {
	STREAM_FACT_ADD_ADAPTER_CASES(STREAM_MAKE_STREAM_FACT_ADD_ADAPTER_TEST)
};

#undef STREAM_FACT_ADD_ADAPTER_CASES
#undef STREAM_MAKE_STREAM_FACT_ADD_ADAPTER_TEST

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	int failed = 0;
	failed += cmocka_run_group_tests(fact_lifecycle_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stream_fact_add_adapter_tests, NULL, NULL);
	return failed;
}

/** @endcond */
