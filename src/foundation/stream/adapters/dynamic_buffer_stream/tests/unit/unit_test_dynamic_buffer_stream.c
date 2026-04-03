/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_dynamic_buffer_stream.c
 * @ingroup dynamic_buffer_stream_unit_tests
 * @brief Unit tests implementation for the `dynamic_buffer_stream` adapter.
 *
 * @details
 * This file implements unit-level validation of the
 * `dynamic_buffer_stream` adapter contracts.
 *
 * Covered surfaces:
 * - CR helpers:
 *   - `dynamic_buffer_stream_default_cfg()`
 *   - `dynamic_buffer_stream_default_env()`
 * - direct creation:
 *   - `dynamic_buffer_stream_create_stream()`
 * - CR descriptor helper:
 *   - `dynamic_buffer_stream_create_desc()`
 * - descriptor constructor usage through `stream_adapter_desc_t::ctor`
 * - behavior implemented by the following private adapter callbacks,
 *   exercised through the public `stream` API and lifecycle entry points:
 *   - `dynamic_buffer_stream_write()`
 *   - `dynamic_buffer_stream_read()`
 *   - `dynamic_buffer_stream_flush()`
 *   - `dynamic_buffer_stream_close()`
 *
 * Test strategy:
 * - direct validation of default CR helper return values
 * - parametric scenario-based testing where meaningful
 * - explicit validation of argument checking and output-handle preservation
 * - allocator fault injection through `fake_memory`
 * - borrower-facing `stream` API checks on successfully created streams
 * - adapter-specific runtime checks for buffer growth, read/write behavior,
 *   flush no-op behavior, and destruction/close behavior
 *
 * See also:
 * - @ref testing_foundation_dynamic_buffer_stream_unit
 *   "dynamic_buffer_stream unit tests page"
 * - @ref specifications_dynamic_buffer_stream
 *   "dynamic_buffer_stream specifications"
 */

#include "dynamic_buffer_stream/cr/dynamic_buffer_stream_cr_api.h"

#include "stream/lifecycle/stream_lifecycle.h"
#include "stream/borrowers/stream.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

// for white-box tests
#include "internal/dynamic_buffer_stream_handle.h"
#include "internal/stream_handle.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "lexleo_cmocka_xmacro_helpers.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Test `dynamic_buffer_stream_default_cfg()`.
 *
 * dynamic_buffer_stream_cfg_t dynamic_buffer_stream_default_cfg(void);
 *
 * Success:
 * - Returns a value-initialized `dynamic_buffer_stream_cfg_t`.
 * - `ret.default_cap > 0`.
 *
 * Failure:
 * - None.
 *
 * See also:
 * - @ref testing_foundation_dynamic_buffer_stream_unit_default_cfg "dynamic_buffer_stream_default_cfg() unit tests section"
 * - @ref specifications_dynamic_buffer_stream_default_cfg "dynamic_buffer_stream_default_cfg() specifications"
 */
static void test_dynamic_buffer_stream_default_cfg(void **state)
{
	(void)state;

	dynamic_buffer_stream_cfg_t ret = dynamic_buffer_stream_default_cfg();

	assert_true(ret.default_cap > 0);
}

/**
 * @brief Test `dynamic_buffer_stream_default_env()`.
 *
 * dynamic_buffer_stream_env_t dynamic_buffer_stream_default_env(
 *     const osal_mem_ops_t *mem,
 *     const stream_env_t *port_env );
 *
 * Success:
 * - `ret.mem == mem`.
 * - `ret.port_env == *port_env`.
 *
 * Failure:
 * - None.
 *
 * Doubles:
 * - dummy `osal_mem_ops_t *`
 * - dummy `stream_env_t`
 *
 * See also:
 * - @ref testing_foundation_dynamic_buffer_stream_unit_default_env "dynamic_buffer_stream_default_env() unit tests section"
 * - @ref specifications_dynamic_buffer_stream_default_env "dynamic_buffer_stream_default_env() specifications".
 */
static void test_dynamic_buffer_stream_default_env(void **state)
{
	(void)state;

	const osal_mem_ops_t *dummy_mem_p =
		(const osal_mem_ops_t *)(uintptr_t)0x1234u;

	const stream_env_t dummy_port_env = {0};
	const stream_env_t *dummy_port_env_p = &dummy_port_env;

	dynamic_buffer_stream_env_t ret =
		dynamic_buffer_stream_default_env(dummy_mem_p, dummy_port_env_p);

	assert_ptr_equal(ret.mem, dummy_mem_p);
	assert_memory_equal(&ret.port_env, dummy_port_env_p, sizeof(ret.port_env));
}

/**
 * @brief Scenarios for `dynamic_buffer_stream_create_stream()`.
 *
 * stream_status_t dynamic_buffer_stream_create_stream(
 *     stream_t **out,
 *     const dynamic_buffer_stream_cfg_t *cfg,
 *     const dynamic_buffer_stream_env_t *env);
 *
 * Doubles:
 * - fake_memory
 *
 * Isolation:
 * - the public `stream` port wrapper is not doubled
 *
 * See also:
 * - @ref testing_foundation_dynamic_buffer_stream_unit_create_stream "dynamic_buffer_stream_create_stream() unit tests section"
 * - @ref specifications_dynamic_buffer_stream_create_stream "dynamic_buffer_stream_create_stream() specifications"
 *
 * The scenarios below define the test oracle for
 * `dynamic_buffer_stream_create_stream()`.
 */
typedef enum {
	/**
	 * WHEN `dynamic_buffer_stream_create_stream(out, cfg, env)` is called with
	 * valid arguments
	 * EXPECT:
	 * - returns `STREAM_STATUS_OK`
	 * - stores a non-NULL stream handle in `*out`
	 */
	DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_OK = 0,

	/**
	 * WHEN `out == NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 */
	DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_OUT_NULL,

	/**
	 * WHEN `cfg == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_CFG_NULL,

	/**
	 * WHEN `env == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_ENV_NULL,

	/**
	 * WHEN allocation required by `dynamic_buffer_stream_create_stream()`
	 * fails
	 * EXPECT:
	 * - returns `STREAM_STATUS_OOM`
	 * - leaves `*out` unchanged
	 *
	 * Notes:
	 * - This scenario is exercised by configuring `fake_memory` to fail the
	 *   allocation performed during stream creation.
	 */
	DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_OOM,
} dynamic_buffer_stream_create_stream_scenario_t;

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
 * @brief One parametric test case for `dynamic_buffer_stream_create_stream()`.
 *
 * Notes:
 * - `fail_call_idx` is used by `fake_memory` to inject an allocation failure
 *   on a specific call number (used by the OOM scenario).
 */
typedef struct {
	const char *name;

	// arrange
	dynamic_buffer_stream_create_stream_scenario_t scenario;
	size_t fail_call_idx; // 0 = no OOM, otherwise 1-based

	// assert
	stream_status_t expected_ret;
	out_expect_t out_expect;
} test_dynamic_buffer_stream_create_stream_case_t;

/**
 * @brief Runtime fixture for `dynamic_buffer_stream_create_stream()` tests.
 *
 * Holds:
 * - the stream handle under test,
 * - the injected adapter environment,
 * - the adapter configuration,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	stream_t *out;

	// injection
	dynamic_buffer_stream_env_t env;

	dynamic_buffer_stream_cfg_t cfg;

	const test_dynamic_buffer_stream_create_stream_case_t *tc;
} test_dynamic_buffer_stream_create_stream_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for
 * `dynamic_buffer_stream_create_stream()` tests.
 */
static int setup_dynamic_buffer_stream_create_stream(void **state)
{
	const test_dynamic_buffer_stream_create_stream_case_t *tc =
		(const test_dynamic_buffer_stream_create_stream_case_t *)(*state);

	test_dynamic_buffer_stream_create_stream_fixture_t *fx =
		(test_dynamic_buffer_stream_create_stream_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_OOM
		&& tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	// DI
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();

	fx->cfg.default_cap = 16;

	*state = fx;
	return 0;
}

/**
 * @brief Release the `dynamic_buffer_stream_create_stream()` test fixture and
 * verify memory invariants.
 */
static int teardown_dynamic_buffer_stream_create_stream(void **state)
{
	test_dynamic_buffer_stream_create_stream_fixture_t *fx =
		(test_dynamic_buffer_stream_create_stream_fixture_t *)(*state);

	if (fx->out) {
		stream_destroy(&fx->out);
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

/**
 * @brief Execute one parametric test scenario for
 * `dynamic_buffer_stream_create_stream()`.
 */
static void test_dynamic_buffer_stream_create_stream(void **state)
{
	test_dynamic_buffer_stream_create_stream_fixture_t *fx =
		(test_dynamic_buffer_stream_create_stream_fixture_t *)(*state);
	const test_dynamic_buffer_stream_create_stream_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t st = STREAM_STATUS_INVALID;
	stream_status_t ret = STREAM_STATUS_INVALID;

	stream_t **out_arg = &fx->out;
	const dynamic_buffer_stream_cfg_t *cfg_arg = &fx->cfg;
	const dynamic_buffer_stream_env_t *env_arg = &fx->env;

	// invalid args
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_CFG_NULL) cfg_arg = NULL;
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_ENV_NULL) env_arg = NULL;

	// ensure OUT_EXPECT_UNCHANGED is meaningful
	if (tc->out_expect == OUT_EXPECT_UNCHANGED && out_arg != NULL) {
		fx->out = (stream_t *)(uintptr_t)0xDEADC0DEu; // sentinel
	}

	stream_t *out_arg_snapshot = fx->out;

	// ACT
	ret = dynamic_buffer_stream_create_stream(out_arg, cfg_arg, env_arg);

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

	if (tc->scenario == DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_OK) {
		assert_non_null(fx->out);

		const char msg[] = "hello";
		size_t w = stream_write(fx->out, msg, sizeof(msg) - 1, &st);
		assert_int_equal((int)w, (int)(sizeof(msg) - 1));
		assert_true(st == STREAM_STATUS_OK);

		assert_int_equal(stream_flush(fx->out), STREAM_STATUS_OK);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_dynamic_buffer_stream_create_stream_case_t
CASE_DYNAMIC_BUFFER_STREAM_CREATE_STREAM_OK = {
	.name = "dynamic_buffer_stream_create_stream_ok",
	.scenario = DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_OK,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_OK,
	.out_expect = OUT_EXPECT_NON_NULL
};

static const test_dynamic_buffer_stream_create_stream_case_t
CASE_DYNAMIC_BUFFER_STREAM_CREATE_STREAM_OUT_NULL = {
	.name = "dynamic_buffer_stream_create_stream_out_null",
	.scenario = DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_OUT_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_CHECK_NONE
};

static const test_dynamic_buffer_stream_create_stream_case_t
CASE_DYNAMIC_BUFFER_STREAM_CREATE_STREAM_CFG_NULL = {
	.name = "dynamic_buffer_stream_create_stream_cfg_null",
	.scenario = DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_CFG_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_dynamic_buffer_stream_create_stream_case_t
CASE_DYNAMIC_BUFFER_STREAM_CREATE_STREAM_ENV_NULL = {
	.name = "dynamic_buffer_stream_create_stream_env_null",
	.scenario = DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_ENV_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_dynamic_buffer_stream_create_stream_case_t
CASE_DYNAMIC_BUFFER_STREAM_CREATE_STREAM_OOM = {
	.name = "dynamic_buffer_stream_create_stream_oom",
	.scenario = DYNAMIC_BUFFER_STREAM_CREATE_STREAM_SCENARIO_OOM,
	.fail_call_idx = 1,

	.expected_ret = STREAM_STATUS_OOM,
	.out_expect = OUT_EXPECT_UNCHANGED
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define DYNAMIC_BUFFER_STREAM_CREATE_STREAM_CASES(X) \
X(CASE_DYNAMIC_BUFFER_STREAM_CREATE_STREAM_OK) \
X(CASE_DYNAMIC_BUFFER_STREAM_CREATE_STREAM_OUT_NULL) \
X(CASE_DYNAMIC_BUFFER_STREAM_CREATE_STREAM_CFG_NULL) \
X(CASE_DYNAMIC_BUFFER_STREAM_CREATE_STREAM_ENV_NULL) \
X(CASE_DYNAMIC_BUFFER_STREAM_CREATE_STREAM_OOM)

#define DYNAMIC_BUFFER_STREAM_MAKE_CREATE_STREAM_TEST(case_sym) \
LEXLEO_MAKE_TEST(dynamic_buffer_stream_create_stream, case_sym)

static const struct CMUnitTest dynamic_buffer_stream_create_stream_tests[] = {
	DYNAMIC_BUFFER_STREAM_CREATE_STREAM_CASES(
		DYNAMIC_BUFFER_STREAM_MAKE_CREATE_STREAM_TEST)
};

#undef DYNAMIC_BUFFER_STREAM_CREATE_STREAM_CASES
#undef DYNAMIC_BUFFER_STREAM_MAKE_CREATE_STREAM_TEST

/** @endcond */

/**
 * @brief Scenarios for `dynamic_buffer_stream_create_desc()`.
 *
 * stream_status_t dynamic_buffer_stream_create_desc(
 *     stream_adapter_desc_t *out,
 *     stream_key_t key,
 *     const dynamic_buffer_stream_cfg_t *cfg,
 *     const dynamic_buffer_stream_env_t *env,
 *     const osal_mem_ops_t *mem );
 *
 * Invalid arguments:
 * - `out`, `key`, `cfg`, `env`, `mem` must not be NULL.
 * - `key` must not be an empty string.
 *
 * Success:
 * - Returns `STREAM_STATUS_OK`.
 * - Stores a valid adapter descriptor in `*out`.
 * - The produced descriptor must later be released via `out->ud_dtor()`.
 *
 * Failure:
 * - Returns:
 *     - `STREAM_STATUS_INVALID` for invalid arguments
 *     - `STREAM_STATUS_OOM` on allocation failure
 * - If `out` is not `NULL`, resets `*out` to an empty descriptor.
 *
 * Doubles:
 * - fake_memory
 *
 * See also:
 * - @ref testing_foundation_dynamic_buffer_stream_unit_create_desc "dynamic_buffer_stream_create_desc() unit tests section"
 * - @ref specifications_dynamic_buffer_stream_create_desc "dynamic_buffer_stream_create_desc() specifications"
 *
 * The scenarios below define the test oracle for
 * `dynamic_buffer_stream_create_desc()`.
 */
typedef enum {
	/**
	 * WHEN `dynamic_buffer_stream_create_desc(out, key, cfg, env, mem)` is
	 * called with valid arguments
	 * EXPECT:
	 * - returns `STREAM_STATUS_OK`
	 * - stores a valid adapter descriptor in `*out`
	 * - the produced descriptor is eligible for later destruction via
	 *   `out->ud_dtor()`
	 */
	DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_OK = 0,

	/**
	 * WHEN `out == NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - no descriptor is produced
	 */
	DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_OUT_NULL,

	/**
	 * WHEN `key == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_KEY_NULL,

	/**
	 * WHEN `key` is an empty string and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_KEY_EMPTY,

	/**
	 * WHEN `cfg == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_CFG_NULL,

	/**
	 * WHEN `env == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_ENV_NULL,

	/**
	 * WHEN `mem == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_MEM_NULL,

	/**
	 * WHEN allocation required by `dynamic_buffer_stream_create_desc()` fails
	 * EXPECT:
	 * - returns `STREAM_STATUS_OOM`
	 * - resets `*out` to an empty descriptor
	 *
	 * Notes:
	 * - This scenario is exercised by configuring `fake_memory` to fail the
	 *   allocation performed during descriptor construction.
	 */
	DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_OOM
} dynamic_buffer_stream_create_desc_scenario_t;

/** @cond INTERNAL */

/**
 * @brief Expected state of the descriptor output after the call under test.
 */
typedef enum {
	DESC_CHECK_NONE,
	DESC_EXPECT_EMPTY,
	DESC_EXPECT_VALID
} desc_expect_t;

/**
 * @brief One parametric test case for `dynamic_buffer_stream_create_desc()`.
 *
 * Notes:
 * - `fail_call_idx` is used by `fake_memory` to inject an allocation failure
 *   on a specific call number (used by the OOM scenario).
 */
typedef struct {
	const char *name;

	// arrange
	dynamic_buffer_stream_create_desc_scenario_t scenario;
	size_t fail_call_idx; // 0 = no OOM, otherwise 1-based

	// assert
	stream_status_t expected_ret;
	desc_expect_t desc_expect;
} test_dynamic_buffer_stream_create_desc_case_t;

/**
 * @brief Runtime fixture for `dynamic_buffer_stream_create_desc()` tests.
 *
 * Holds:
 * - the descriptor output under test,
 * - the injected adapter environment,
 * - the injected descriptor allocator,
 * - the adapter key and configuration,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	stream_adapter_desc_t out;

	// injection
	dynamic_buffer_stream_env_t env;
	const osal_mem_ops_t *mem;

	stream_key_t key;
	dynamic_buffer_stream_cfg_t cfg;

	const test_dynamic_buffer_stream_create_desc_case_t *tc;
} test_dynamic_buffer_stream_create_desc_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for
 * `dynamic_buffer_stream_create_desc()` tests.
 */
static int setup_dynamic_buffer_stream_create_desc(void **state)
{
	const test_dynamic_buffer_stream_create_desc_case_t *tc =
		(const test_dynamic_buffer_stream_create_desc_case_t *)(*state);

	test_dynamic_buffer_stream_create_desc_fixture_t *fx =
		(test_dynamic_buffer_stream_create_desc_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_OOM
		&& tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	// DI
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();
	fx->mem = osal_mem_test_fake_ops();

	fx->key = "dynamic_buffer";

	fx->cfg.default_cap = 16u;

	*state = fx;
	return 0;
}

/**
 * @brief Release the `dynamic_buffer_stream_create_desc()` test fixture and
 * verify memory invariants.
 */
static int teardown_dynamic_buffer_stream_create_desc(void **state)
{
	test_dynamic_buffer_stream_create_desc_fixture_t *fx =
		(test_dynamic_buffer_stream_create_desc_fixture_t *)(*state);

	if (fx->out.ud_dtor) fx->out.ud_dtor(fx->out.ud, fx->mem);

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
 * @brief Execute one parametric test scenario for
 * `dynamic_buffer_stream_create_desc()`.
 */
static void test_dynamic_buffer_stream_create_desc(void **state)
{
	test_dynamic_buffer_stream_create_desc_fixture_t *fx =
		(test_dynamic_buffer_stream_create_desc_fixture_t *)(*state);
	const test_dynamic_buffer_stream_create_desc_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t ret = STREAM_STATUS_INVALID;

	stream_adapter_desc_t *out_arg = &fx->out;
	stream_key_t key_arg = fx->key;
	const dynamic_buffer_stream_cfg_t *cfg_arg = &fx->cfg;
	const dynamic_buffer_stream_env_t *env_arg = &fx->env;
	const osal_mem_ops_t *mem_arg = fx->mem;

	// invalid args
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_KEY_NULL) key_arg = NULL;
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_KEY_EMPTY) key_arg = "";
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_CFG_NULL) cfg_arg = NULL;
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_ENV_NULL) env_arg = NULL;
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_MEM_NULL) mem_arg = NULL;

	if (tc->desc_expect == DESC_EXPECT_EMPTY && out_arg != NULL) {
		fx->out.key = (stream_key_t)(uintptr_t)0xDEADC0DEu;
		fx->out.ctor = (void *)(uintptr_t)0xDEADC0DEu;
		fx->out.ud = (void *)(uintptr_t)0xDEADC0DEu;
		fx->out.ud_dtor = (void *)(uintptr_t)0xDEADC0DEu;
	}

	// ACT
	ret = dynamic_buffer_stream_create_desc(out_arg, key_arg, cfg_arg, env_arg, mem_arg);

	// ASSERT
	assert_int_equal(ret, tc->expected_ret);

	if (tc->desc_expect == DESC_EXPECT_EMPTY) {
		assert_null(fx->out.key);
		assert_null(fx->out.ctor);
		assert_null(fx->out.ud);
		assert_null(fx->out.ud_dtor);
	}
	else if (tc->desc_expect == DESC_EXPECT_VALID) {
		assert_true(fx->out.key != (stream_key_t)(uintptr_t)0xDEADC0DEu);
		assert_non_null(fx->out.key);
		assert_true(*fx->out.key != '\0');
		assert_true(fx->out.ctor != (void *)(uintptr_t)0xDEADC0DEu);
		assert_non_null(fx->out.ctor);
		assert_true(fx->out.ud != (void *)(uintptr_t)0xDEADC0DEu);
		assert_non_null(fx->out.ud);
		assert_true(fx->out.ud_dtor != (void *)(uintptr_t)0xDEADC0DEu);
		assert_non_null(fx->out.ud_dtor);
	}
	else {
		assert_true(tc->desc_expect == DESC_CHECK_NONE);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_dynamic_buffer_stream_create_desc_case_t
CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_OK = {
	.name = "dynamic_buffer_stream_create_desc_ok",
	.scenario = DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_OK,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_OK,
	.desc_expect = DESC_EXPECT_VALID
};

static const test_dynamic_buffer_stream_create_desc_case_t
CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_OUT_NULL = {
	.name = "dynamic_buffer_stream_create_desc_out_null",
	.scenario = DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_OUT_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_CHECK_NONE
};

static const test_dynamic_buffer_stream_create_desc_case_t
CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_KEY_NULL = {
	.name = "dynamic_buffer_stream_create_desc_key_null",
	.scenario = DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_KEY_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_dynamic_buffer_stream_create_desc_case_t
CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_KEY_EMPTY = {
	.name = "dynamic_buffer_stream_create_desc_key_empty",
	.scenario = DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_KEY_EMPTY,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_dynamic_buffer_stream_create_desc_case_t
CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_CFG_NULL = {
	.name = "dynamic_buffer_stream_create_desc_cfg_null",
	.scenario = DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_CFG_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_dynamic_buffer_stream_create_desc_case_t
CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_ENV_NULL = {
	.name = "dynamic_buffer_stream_create_desc_env_null",
	.scenario = DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_ENV_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_dynamic_buffer_stream_create_desc_case_t
CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_MEM_NULL = {
	.name = "dynamic_buffer_stream_create_desc_mem_null",
	.scenario = DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_MEM_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_dynamic_buffer_stream_create_desc_case_t
CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_OOM_1 = {
	.name = "dynamic_buffer_stream_create_desc_oom_1",
	.scenario = DYNAMIC_BUFFER_STREAM_CREATE_DESC_SCENARIO_OOM,
	.fail_call_idx = 1,

	.expected_ret = STREAM_STATUS_OOM,
	.desc_expect = DESC_EXPECT_EMPTY
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define DYNAMIC_BUFFER_STREAM_CREATE_DESC_CASES(X) \
X(CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_OK) \
X(CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_OUT_NULL) \
X(CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_KEY_NULL) \
X(CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_KEY_EMPTY) \
X(CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_CFG_NULL) \
X(CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_ENV_NULL) \
X(CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_MEM_NULL) \
X(CASE_DYNAMIC_BUFFER_STREAM_CREATE_DESC_OOM_1)

#define DYNAMIC_BUFFER_STREAM_MAKE_CREATE_DESC_TEST(case_sym) \
LEXLEO_MAKE_TEST(dynamic_buffer_stream_create_desc, case_sym)

static const struct CMUnitTest create_desc_dynamic_buffer_stream_tests[] = {
	DYNAMIC_BUFFER_STREAM_CREATE_DESC_CASES(
		DYNAMIC_BUFFER_STREAM_MAKE_CREATE_DESC_TEST)
};

#undef DYNAMIC_BUFFER_STREAM_CREATE_DESC_CASES
#undef DYNAMIC_BUFFER_STREAM_MAKE_CREATE_DESC_TEST

/** @endcond */

/**
 * @brief Scenarios for `dynamic_buffer_stream_write()`.
 *
 * static size_t dynamic_buffer_stream_write(
 *     void *backend,
 *     const void *buf,
 *     size_t n,
 *     stream_status_t *st );
 *
 * This private callback is exercised through the public `stream_write()` API
 * on streams created by `dynamic_buffer_stream_create_stream()`.
 *
 * Invalid arguments:
 * - `backend == NULL`
 * - `buf == NULL && n > 0`
 *
 * Success:
 * - Returns the number of bytes written.
 * - Appends written bytes to the end of the dynamic buffer.
 * - Leaves `read_pos` unchanged.
 * - Sets `*st` to `STREAM_STATUS_OK` when `st != NULL`.
 *
 * Failure:
 * - Returns:
 *     - `STREAM_STATUS_INVALID` for invalid arguments
 *     - `STREAM_STATUS_OOM` on growth / reserve failure
 * - Leaves the backend unchanged on failure paths covered here.
 *
 * Doubles:
 * - fake_memory
 *
 * Isolation:
 * - the callback is not called directly
 * - behavior is exercised through the public `stream` port wrapper
 *
 * See also:
 * - @ref testing_foundation_dynamic_buffer_stream_unit_write "dynamic_buffer_stream_write() unit tests section"
 * - @ref specifications_dynamic_buffer_stream_write "dynamic_buffer_stream_write() specifications"
 * - @ref specifications_stream_write "stream_write() specifications"
 *
 * The scenarios below define the test oracle for `dynamic_buffer_stream_write()`.
 */
typedef enum {
	/**
	 * WHEN `stream_write(s, buf, n, &st)` is called with valid arguments
	 * AND `st != NULL`
	 * AND enough existing capacity
	 * EXPECT:
	 * - returns `n`
	 * - `st == STREAM_STATUS_OK`
	 * - bytes are appended to the backend buffer
	 * - `read_pos` is unchanged
	 */
	DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_OK_NO_GROW = 0,

	/**
	 * WHEN `stream_write(s, buf, 0, &st)` is called
	 * AND `buf != NULL`
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `*st == STREAM_STATUS_OK`
	 * - backend is left unchanged
	 */
	DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_ZERO_LEN,

	/**
	 * WHEN `stream_write(s, NULL, 0, &st)` is called
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `*st == STREAM_STATUS_OK`
	 * - backend is left unchanged
	 */
	DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_BUF_NULL_ZERO_LEN,

	/**
	 * WHEN `stream_write(s, buf, n, &st)` is called with valid arguments
	 * AND `st != NULL`
	 * AND buffer growth is required
	 * EXPECT:
	 * - returns `n`
	 * - `st == STREAM_STATUS_OK`
	 * - bytes are appended to the backend buffer
	 * - `read_pos` is unchanged
	 * - `cap` grows to accommodate the requested write
	 */
	DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_OK_GROW,

	/**
	 * WHEN `stream_write(s, NULL, n, &st)` is called with `n > 0`
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `st == STREAM_STATUS_INVALID`
	 * - backend is left unchanged
	 */
	DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_BUF_NULL_NONZERO,

	/**
	 * WHEN `stream_write(s, buf, n, &st)` is called with valid arguments
	 * AND `st != NULL`
	 * AND allocation required by reserve during write fails
	 * EXPECT:
	 * - returns `0`
	 * - `st == STREAM_STATUS_OOM`
	 * - backend is left unchanged
	 *
	 * Notes:
	 * - This scenario is exercised by configuring `fake_memory` to fail the
	 *   allocation performed during growth / reserve.
	 */
	DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_RESERVE_OOM,

	/**
	 * WHEN `stream_write(s, buf, n, &st)` is called with valid arguments
	 * AND `st != NULL`
	 * AND the requested growth is invalid because `n > SIZE_MAX - dbuf->len`
	 * EXPECT:
	 * - returns `0`
	 * - `st == STREAM_STATUS_INVALID`
	 * - backend is left unchanged
	 *
	 * Notes:
	 * - This scenario requires direct fixture seeding of backend internals.
	 * - It is still executed through the public `stream_write()` wrapper.
	 */
	DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_SIZE_OVERFLOW
} dynamic_buffer_stream_write_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `dynamic_buffer_stream_write()`.
 *
 * Notes:
 * - `initial_cap` controls the configuration used when creating the stream.
 * - `prefill_len` and `prefill_read_pos` are used to seed backend state before
 *   the write under test.
 * - `write_len` is the size passed to `stream_write()`.
 * - `fail_call_idx` is used by `fake_memory` to inject an allocation failure
 *   after the stream has already been created.
 */
typedef struct {
	const char *name;

	// arrange
	dynamic_buffer_stream_write_scenario_t scenario;
	size_t initial_cap;
	size_t prefill_len;
	size_t prefill_read_pos;
	size_t write_len;
	size_t fail_call_idx; // 0 = no injected OOM, otherwise 1-based

	// assert
	size_t expected_written;
	stream_status_t expected_st;
	bool expect_cap_growth;
	bool expect_backend_unchanged;
} test_dynamic_buffer_stream_write_case_t;

/**
 * @brief Runtime fixture for `dynamic_buffer_stream_write()` tests.
 *
 * Holds:
 * - the public stream handle under test,
 * - the injected adapter environment and configuration,
 * - the write payload buffer,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	stream_t *s;

	// injection
	dynamic_buffer_stream_env_t env;
	dynamic_buffer_stream_cfg_t cfg;

	// test payload
	uint8_t payload[64];

	const test_dynamic_buffer_stream_write_case_t *tc;
} test_dynamic_buffer_stream_write_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for
 * `dynamic_buffer_stream_write()` tests.
 */
static int setup_dynamic_buffer_stream_write(void **state)
{
	const test_dynamic_buffer_stream_write_case_t *tc =
		(const test_dynamic_buffer_stream_write_case_t *)(*state);

	test_dynamic_buffer_stream_write_fixture_t *fx =
		(test_dynamic_buffer_stream_write_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();

	// DI
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();

	fx->cfg.default_cap = tc->initial_cap;

	for (size_t i = 0; i < sizeof(fx->payload); ++i) {
		fx->payload[i] = (uint8_t)('A' + (i % 26));
	}

	if (dynamic_buffer_stream_create_stream(&fx->s, &fx->cfg, &fx->env) != STREAM_STATUS_OK) {
		free(fx);
		return -1;
	}

	/*
	 * Inject OOM after creation so the first subsequent allocation performed by
	 * growth/reserve fails.
	 */
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_RESERVE_OOM
		&& tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	*state = fx;
	return 0;
}

/**
 * @brief Release the `dynamic_buffer_stream_write()` test fixture and verify
 * memory invariants.
 */
static int teardown_dynamic_buffer_stream_write(void **state)
{
	test_dynamic_buffer_stream_write_fixture_t *fx =
		(test_dynamic_buffer_stream_write_fixture_t *)(*state);

	if (fx->s) {
		stream_destroy(&fx->s);
		fx->s = NULL;
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
 * @brief Execute one parametric test scenario for `dynamic_buffer_stream_write()`.
 *
 * Notes:
 * - This test intentionally exercises the private backend callback through the
 *   public `stream_write()` wrapper.
 * - It assumes the unit test has visibility on internal backend/layout types.
 */
static void test_dynamic_buffer_stream_write(void **state)
{
	test_dynamic_buffer_stream_write_fixture_t *fx =
		(test_dynamic_buffer_stream_write_fixture_t *)(*state);
	const test_dynamic_buffer_stream_write_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t st = STREAM_STATUS_INVALID;

	stream_t *s = fx->s;
	assert_non_null(s);
	assert_non_null(s->backend);

	dynamic_buffer_stream_t *dbs = (dynamic_buffer_stream_t *)s->backend;
	assert_non_null(dbs);

	dynamic_buffer_t *dbuf = &dbs->state.dbuf;
	assert_non_null(dbuf);

	/*
	 * Seed backend state.
	 *
	 * Backend fields used here:
	 * - buf
	 * - len
	 * - cap
	 * - read_pos
	 */
	assert_true(tc->prefill_len <= tc->initial_cap);
	if (tc->prefill_len > 0) {
		assert_non_null(dbuf->buf);
		memcpy(dbuf->buf, fx->payload, tc->prefill_len);
	}
	dbuf->len = tc->prefill_len;
	dbuf->read_pos = tc->prefill_read_pos;

	if (tc->scenario == DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_SIZE_OVERFLOW) {
		dbuf->len = SIZE_MAX;
		dbuf->read_pos = tc->prefill_read_pos;
	}

	const void *buf_arg = fx->payload;
	size_t n_arg = tc->write_len;

	if (tc->scenario == DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_BUF_NULL_ZERO_LEN) {
		buf_arg = NULL;
		n_arg = 0;
	}
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_BUF_NULL_NONZERO) {
		buf_arg = NULL;
	}
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_ZERO_LEN) {
		n_arg = 0;
	}

	// Snapshot backend state for failure / invariance assertions.
	uint8_t before_bytes[64];
	const size_t snap_n =
		(dbuf->len < sizeof(before_bytes)) ? dbuf->len : sizeof(before_bytes);
	if (snap_n > 0 && dbuf->buf) {
		memcpy(before_bytes, dbuf->buf, snap_n);
	}

	const void *buf_snapshot = dbuf->buf;
	const size_t len_snapshot = dbuf->len;
	const size_t cap_snapshot = dbuf->cap;
	const size_t read_pos_snapshot = dbuf->read_pos;

	// ACT
	size_t written = stream_write(s, buf_arg, n_arg, &st);

	// ASSERT
	assert_int_equal((int)written, (int)tc->expected_written);
	assert_int_equal((int)st, (int)tc->expected_st);

	if (tc->expect_backend_unchanged) {
		assert_ptr_equal(dbuf->buf, buf_snapshot);
		assert_int_equal((int)dbuf->len, (int)len_snapshot);
		assert_int_equal((int)dbuf->cap, (int)cap_snapshot);
		assert_int_equal((int)dbuf->read_pos, (int)read_pos_snapshot);

		if (snap_n > 0 && dbuf->buf) {
			assert_memory_equal(dbuf->buf, before_bytes, snap_n);
		}
		return;
	}

	// Success paths.
	assert_int_equal((int)dbuf->read_pos, (int)read_pos_snapshot);
	assert_int_equal((int)dbuf->len, (int)(len_snapshot + tc->write_len));

	if (tc->expect_cap_growth) {
		assert_true(dbuf->cap > cap_snapshot);
	}
	else {
		assert_true(dbuf->cap >= cap_snapshot);
	}

	if (len_snapshot > 0) {
		assert_memory_equal(dbuf->buf, before_bytes, snap_n);
	}
	if (tc->write_len > 0) {
		assert_memory_equal(
			(uint8_t *)dbuf->buf + len_snapshot,
			fx->payload,
			tc->write_len);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_dynamic_buffer_stream_write_case_t
CASE_DYNAMIC_BUFFER_STREAM_WRITE_OK_NO_GROW = {
	.name = "dynamic_buffer_stream_write_ok_no_grow",
	.scenario = DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_OK_NO_GROW,
	.initial_cap = 16u,
	.prefill_len = 3u,
	.prefill_read_pos = 1u,
	.write_len = 5u,
	.fail_call_idx = 0u,

	.expected_written = 5u,
	.expected_st = STREAM_STATUS_OK,
	.expect_cap_growth = false,
	.expect_backend_unchanged = false
};

static const test_dynamic_buffer_stream_write_case_t
CASE_DYNAMIC_BUFFER_STREAM_WRITE_ZERO_LEN = {
	.name = "dynamic_buffer_stream_write_zero_len",
	.scenario = DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_ZERO_LEN,
	.initial_cap = 16u,
	.prefill_len = 3u,
	.prefill_read_pos = 1u,
	.write_len = 0u,
	.fail_call_idx = 0u,

	.expected_written = 0u,
	.expected_st = STREAM_STATUS_OK,
	.expect_cap_growth = false,
	.expect_backend_unchanged = true
};

static const test_dynamic_buffer_stream_write_case_t
CASE_DYNAMIC_BUFFER_STREAM_WRITE_BUF_NULL_ZERO_LEN = {
	.name = "dynamic_buffer_stream_write_buf_null_zero_len",
	.scenario = DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_BUF_NULL_ZERO_LEN,
	.initial_cap = 16u,
	.prefill_len = 3u,
	.prefill_read_pos = 1u,
	.write_len = 0u,
	.fail_call_idx = 0u,

	.expected_written = 0u,
	.expected_st = STREAM_STATUS_OK,
	.expect_cap_growth = false,
	.expect_backend_unchanged = true
};

static const test_dynamic_buffer_stream_write_case_t
CASE_DYNAMIC_BUFFER_STREAM_WRITE_OK_GROW = {
	.name = "dynamic_buffer_stream_write_ok_grow",
	.scenario = DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_OK_GROW,
	.initial_cap = 4u,
	.prefill_len = 4u,
	.prefill_read_pos = 2u,
	.write_len = 5u,
	.fail_call_idx = 0u,

	.expected_written = 5u,
	.expected_st = STREAM_STATUS_OK,
	.expect_cap_growth = true,
	.expect_backend_unchanged = false
};

static const test_dynamic_buffer_stream_write_case_t
CASE_DYNAMIC_BUFFER_STREAM_WRITE_BUF_NULL_NONZERO = {
	.name = "dynamic_buffer_stream_write_buf_null_nonzero",
	.scenario = DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_BUF_NULL_NONZERO,
	.initial_cap = 16u,
	.prefill_len = 3u,
	.prefill_read_pos = 1u,
	.write_len = 5u,
	.fail_call_idx = 0u,

	.expected_written = 0u,
	.expected_st = STREAM_STATUS_INVALID,
	.expect_cap_growth = false,
	.expect_backend_unchanged = true
};

static const test_dynamic_buffer_stream_write_case_t
CASE_DYNAMIC_BUFFER_STREAM_WRITE_RESERVE_OOM = {
	.name = "dynamic_buffer_stream_write_reserve_oom",
	.scenario = DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_RESERVE_OOM,
	.initial_cap = 4u,
	.prefill_len = 4u,
	.prefill_read_pos = 2u,
	.write_len = 5u,
	.fail_call_idx = 1u,

	.expected_written = 0u,
	.expected_st = STREAM_STATUS_OOM,
	.expect_cap_growth = false,
	.expect_backend_unchanged = true
};

static const test_dynamic_buffer_stream_write_case_t
CASE_DYNAMIC_BUFFER_STREAM_WRITE_SIZE_OVERFLOW = {
	.name = "dynamic_buffer_stream_write_size_overflow",
	.scenario = DYNAMIC_BUFFER_STREAM_WRITE_SCENARIO_SIZE_OVERFLOW,
	.initial_cap = 16u,
	.prefill_len = 0u,
	.prefill_read_pos = 0u,
	.write_len = 1u,
	.fail_call_idx = 0u,

	.expected_written = 0u,
	.expected_st = STREAM_STATUS_INVALID,
	.expect_cap_growth = false,
	.expect_backend_unchanged = true
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define DYNAMIC_BUFFER_STREAM_WRITE_CASES(X) \
X(CASE_DYNAMIC_BUFFER_STREAM_WRITE_OK_NO_GROW) \
X(CASE_DYNAMIC_BUFFER_STREAM_WRITE_ZERO_LEN) \
X(CASE_DYNAMIC_BUFFER_STREAM_WRITE_BUF_NULL_ZERO_LEN) \
X(CASE_DYNAMIC_BUFFER_STREAM_WRITE_OK_GROW) \
X(CASE_DYNAMIC_BUFFER_STREAM_WRITE_BUF_NULL_NONZERO) \
X(CASE_DYNAMIC_BUFFER_STREAM_WRITE_RESERVE_OOM) \
X(CASE_DYNAMIC_BUFFER_STREAM_WRITE_SIZE_OVERFLOW)

#define DYNAMIC_BUFFER_STREAM_MAKE_WRITE_TEST(case_sym) \
LEXLEO_MAKE_TEST(dynamic_buffer_stream_write, case_sym)

static const struct CMUnitTest dynamic_buffer_stream_write_tests[] = {
	DYNAMIC_BUFFER_STREAM_WRITE_CASES(DYNAMIC_BUFFER_STREAM_MAKE_WRITE_TEST)
};

#undef DYNAMIC_BUFFER_STREAM_WRITE_CASES
#undef DYNAMIC_BUFFER_STREAM_MAKE_WRITE_TEST

/** @endcond */

/**
 * @brief Scenarios for `dynamic_buffer_stream_read()`.
 *
 * static size_t dynamic_buffer_stream_read(
 *     void *backend,
 *     void *buf,
 *     size_t n,
 *     stream_status_t *st );
 *
 * This private callback is exercised through the public `stream_read()` API
 * on streams created by `dynamic_buffer_stream_create_stream()`.
 *
 * Invalid arguments:
 * - `buf == NULL && n > 0`
 *
 * Success:
 * - Returns the number of bytes read.
 * - Copies read bytes from the backend buffer into `buf`.
 * - Advances `read_pos` by the number of bytes returned.
 * - Leaves `len` unchanged.
 * - Sets `*st` to `STREAM_STATUS_OK` when `st != NULL`.
 *
 * Failure:
 * - Returns:
 *     - `STREAM_STATUS_INVALID` for invalid arguments
 *     - `STREAM_STATUS_EOF` when no unread byte remains
 * - Leaves the backend unchanged on failure paths covered here.
 *
 * Doubles:
 * - none
 *
 * Isolation:
 * - the callback is not called directly
 * - behavior is exercised through the public `stream` port wrapper
 *
 * See also:
 * - @ref testing_foundation_dynamic_buffer_stream_unit_read "dynamic_buffer_stream_read() unit tests section"
 * - @ref specifications_dynamic_buffer_stream_read "dynamic_buffer_stream_read() specifications"
 * - @ref specifications_stream_read "stream_read() specifications"
 *
 * The scenarios below define the test oracle for `dynamic_buffer_stream_read()`.
 */
typedef enum {
	/**
	 * WHEN `stream_read(s, buf, n, &st)` is called with valid arguments
	 * AND `st != NULL`
	 * AND `0 < n <= dbuf->len - dbuf->read_pos`
	 * EXPECT:
	 * - returns `n`
	 * - `*st == STREAM_STATUS_OK`
	 * - exactly `n` bytes are copied from the backend buffer starting at
	 *   `dbuf->read_pos`
	 * - `dbuf->read_pos` is advanced by `n`
	 * - `dbuf->len` is unchanged
	 */
	DYNAMIC_BUFFER_STREAM_READ_SCENARIO_OK_EXACT = 0,

	/**
	 * WHEN `stream_read(s, buf, n, &st)` is called with valid arguments
	 * AND `st != NULL`
	 * AND `n > dbuf->len - dbuf->read_pos`
	 * EXPECT:
	 * - returns `dbuf->len - dbuf->read_pos`
	 * - `*st == STREAM_STATUS_OK`
	 * - all unread bytes are copied from the backend buffer
	 * - `dbuf->read_pos` is advanced to `dbuf->len`
	 * - `dbuf->len` is unchanged
	 */
	DYNAMIC_BUFFER_STREAM_READ_SCENARIO_OK_CLAMP_TO_AVAILABLE,

	/**
	 * WHEN `stream_read(s, buf, 0, &st)` is called
	 * AND `buf != NULL`
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `*st == STREAM_STATUS_OK`
	 * - backend is left unchanged
	 */
	DYNAMIC_BUFFER_STREAM_READ_SCENARIO_ZERO_LEN,

	/**
	 * WHEN `stream_read(s, NULL, 0, &st)` is called
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `*st == STREAM_STATUS_OK`
	 * - backend is left unchanged
	 */
	DYNAMIC_BUFFER_STREAM_READ_SCENARIO_BUF_NULL_ZERO_LEN,

	/**
	 * WHEN `stream_read(s, NULL, n, &st)` is called with `n > 0`
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `*st == STREAM_STATUS_INVALID`
	 * - backend is left unchanged
	 */
	DYNAMIC_BUFFER_STREAM_READ_SCENARIO_BUF_NULL_NONZERO,

	/**
	 * WHEN `stream_read(s, buf, n, &st)` is called with valid arguments
	 * AND `st != NULL`
	 * AND `dbuf->read_pos >= dbuf->len`
	 * EXPECT:
	 * - returns `0`
	 * - `*st == STREAM_STATUS_EOF`
	 * - backend is left unchanged
	 */
	DYNAMIC_BUFFER_STREAM_READ_SCENARIO_EOF
} dynamic_buffer_stream_read_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `dynamic_buffer_stream_read()`.
 *
 * Notes:
 * - `initial_cap` controls the configuration used when creating the stream.
 * - `prefill_len` and `prefill_read_pos` are used to seed backend state before
 *   the read under test.
 * - `read_len` is the size passed to `stream_read()`.
 */
typedef struct {
	const char *name;

	// arrange
	dynamic_buffer_stream_read_scenario_t scenario;
	size_t initial_cap;
	size_t prefill_len;
	size_t prefill_read_pos;
	size_t read_len;

	// assert
	size_t expected_read;
	stream_status_t expected_st;
	bool expect_backend_unchanged;
} test_dynamic_buffer_stream_read_case_t;

/**
 * @brief Runtime fixture for `dynamic_buffer_stream_read()` tests.
 *
 * Holds:
 * - the public stream handle under test,
 * - the injected adapter environment and configuration,
 * - the seeded backend payload,
 * - the read destination buffer,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	stream_t *s;

	// injection
	dynamic_buffer_stream_env_t env;
	dynamic_buffer_stream_cfg_t cfg;

	// test buffers
	uint8_t payload[64];
	uint8_t read_buf[64];

	const test_dynamic_buffer_stream_read_case_t *tc;
} test_dynamic_buffer_stream_read_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for
 * `dynamic_buffer_stream_read()` tests.
 */
static int setup_dynamic_buffer_stream_read(void **state)
{
	const test_dynamic_buffer_stream_read_case_t *tc =
		(const test_dynamic_buffer_stream_read_case_t *)(*state);

	test_dynamic_buffer_stream_read_fixture_t *fx =
		(test_dynamic_buffer_stream_read_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();

	// DI
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();

	fx->cfg.default_cap = tc->initial_cap;

	for (size_t i = 0; i < sizeof(fx->payload); ++i) {
		fx->payload[i] = (uint8_t)('A' + (i % 26));
	}
	for (size_t i = 0; i < sizeof(fx->read_buf); ++i) {
		fx->read_buf[i] = (uint8_t)0xCDu;
	}

	if (dynamic_buffer_stream_create_stream(&fx->s, &fx->cfg, &fx->env)
		!= STREAM_STATUS_OK) {
		free(fx);
		return -1;
	}

	*state = fx;
	return 0;
}

/**
 * @brief Release the `dynamic_buffer_stream_read()` test fixture and verify
 * memory invariants.
 */
static int teardown_dynamic_buffer_stream_read(void **state)
{
	test_dynamic_buffer_stream_read_fixture_t *fx =
		(test_dynamic_buffer_stream_read_fixture_t *)(*state);

	if (fx->s) {
		stream_destroy(&fx->s);
		fx->s = NULL;
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
 * @brief Execute one parametric test scenario for
 * `dynamic_buffer_stream_read()`.
 *
 * Notes:
 * - This test intentionally exercises the private backend callback through the
 *   public `stream_read()` wrapper.
 * - It assumes the unit test has visibility on internal backend/layout types.
 */
static void test_dynamic_buffer_stream_read(void **state)
{
	test_dynamic_buffer_stream_read_fixture_t *fx =
		(test_dynamic_buffer_stream_read_fixture_t *)(*state);
	const test_dynamic_buffer_stream_read_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t st = STREAM_STATUS_INVALID;

	stream_t *s = fx->s;
	assert_non_null(s);
	assert_non_null(s->backend);

	dynamic_buffer_stream_t *dbs = (dynamic_buffer_stream_t *)s->backend;
	assert_non_null(dbs);

	dynamic_buffer_t *dbuf = &dbs->state.dbuf;
	assert_non_null(dbuf);

	/*
	 * Seed backend state.
	 *
	 * Backend fields used here:
	 * - buf
	 * - len
	 * - cap
	 * - read_pos
	 */
	assert_true(tc->prefill_len <= dbuf->cap);
	if (tc->prefill_len > 0) {
		assert_non_null(dbuf->buf);
		memcpy(dbuf->buf, fx->payload, tc->prefill_len);
	}
	dbuf->len = tc->prefill_len;
	dbuf->read_pos = tc->prefill_read_pos;

	void *buf_arg = fx->read_buf;
	size_t n_arg = tc->read_len;

	if (tc->scenario == DYNAMIC_BUFFER_STREAM_READ_SCENARIO_BUF_NULL_ZERO_LEN) {
		buf_arg = NULL;
		n_arg = 0u;
	}
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_READ_SCENARIO_BUF_NULL_NONZERO) {
		buf_arg = NULL;
	}
	if (tc->scenario == DYNAMIC_BUFFER_STREAM_READ_SCENARIO_ZERO_LEN) {
		n_arg = 0u;
	}

	const size_t unread_before =
		(dbuf->read_pos < dbuf->len) ? (dbuf->len - dbuf->read_pos) : 0u;
	const size_t expected_copy =
		(tc->read_len < unread_before) ? tc->read_len : unread_before;

	uint8_t before_bytes[64];
	const size_t snap_n =
		(dbuf->len < sizeof(before_bytes)) ? dbuf->len : sizeof(before_bytes);
	if (snap_n > 0 && dbuf->buf) {
		memcpy(before_bytes, dbuf->buf, snap_n);
	}

	uint8_t read_buf_snapshot[64];
	memcpy(read_buf_snapshot, fx->read_buf, sizeof(read_buf_snapshot));

	const void *buf_snapshot = dbuf->buf;
	const size_t len_snapshot = dbuf->len;
	const size_t cap_snapshot = dbuf->cap;
	const size_t read_pos_snapshot = dbuf->read_pos;

	// ACT
	size_t rd = stream_read(s, buf_arg, n_arg, &st);

	// ASSERT
	assert_int_equal((int)rd, (int)tc->expected_read);
	assert_int_equal((int)st, (int)tc->expected_st);

	if (tc->expect_backend_unchanged) {
		assert_ptr_equal(dbuf->buf, buf_snapshot);
		assert_int_equal((int)dbuf->len, (int)len_snapshot);
		assert_int_equal((int)dbuf->cap, (int)cap_snapshot);
		assert_int_equal((int)dbuf->read_pos, (int)read_pos_snapshot);

		if (snap_n > 0 && dbuf->buf) {
			assert_memory_equal(dbuf->buf, before_bytes, snap_n);
		}

		if (buf_arg != NULL) {
			assert_memory_equal(fx->read_buf, read_buf_snapshot, sizeof(fx->read_buf));
		}
		return;
	}

	// Success paths with data consumption.
	assert_ptr_equal(dbuf->buf, buf_snapshot);
	assert_int_equal((int)dbuf->len, (int)len_snapshot);
	assert_int_equal((int)dbuf->cap, (int)cap_snapshot);
	assert_int_equal((int)dbuf->read_pos, (int)(read_pos_snapshot + expected_copy));

	if (snap_n > 0 && dbuf->buf) {
		assert_memory_equal(dbuf->buf, before_bytes, snap_n);
	}

	if (expected_copy > 0) {
		assert_memory_equal(
			fx->read_buf,
			fx->payload + read_pos_snapshot,
			expected_copy);
	}
	if (expected_copy < sizeof(fx->read_buf)) {
		assert_memory_equal(
			fx->read_buf + expected_copy,
			read_buf_snapshot + expected_copy,
			sizeof(fx->read_buf) - expected_copy);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_dynamic_buffer_stream_read_case_t
CASE_DYNAMIC_BUFFER_STREAM_READ_OK_EXACT = {
	.name = "dynamic_buffer_stream_read_ok_exact",
	.scenario = DYNAMIC_BUFFER_STREAM_READ_SCENARIO_OK_EXACT,
	.initial_cap = 16u,
	.prefill_len = 8u,
	.prefill_read_pos = 2u,
	.read_len = 3u,

	.expected_read = 3u,
	.expected_st = STREAM_STATUS_OK,
	.expect_backend_unchanged = false
};

static const test_dynamic_buffer_stream_read_case_t
CASE_DYNAMIC_BUFFER_STREAM_READ_OK_CLAMP_TO_AVAILABLE = {
	.name = "dynamic_buffer_stream_read_ok_clamp_to_available",
	.scenario = DYNAMIC_BUFFER_STREAM_READ_SCENARIO_OK_CLAMP_TO_AVAILABLE,
	.initial_cap = 16u,
	.prefill_len = 8u,
	.prefill_read_pos = 6u,
	.read_len = 5u,

	.expected_read = 2u,
	.expected_st = STREAM_STATUS_OK,
	.expect_backend_unchanged = false
};

static const test_dynamic_buffer_stream_read_case_t
CASE_DYNAMIC_BUFFER_STREAM_READ_ZERO_LEN = {
	.name = "dynamic_buffer_stream_read_zero_len",
	.scenario = DYNAMIC_BUFFER_STREAM_READ_SCENARIO_ZERO_LEN,
	.initial_cap = 16u,
	.prefill_len = 8u,
	.prefill_read_pos = 2u,
	.read_len = 0u,

	.expected_read = 0u,
	.expected_st = STREAM_STATUS_OK,
	.expect_backend_unchanged = true
};

static const test_dynamic_buffer_stream_read_case_t
CASE_DYNAMIC_BUFFER_STREAM_READ_BUF_NULL_ZERO_LEN = {
	.name = "dynamic_buffer_stream_read_buf_null_zero_len",
	.scenario = DYNAMIC_BUFFER_STREAM_READ_SCENARIO_BUF_NULL_ZERO_LEN,
	.initial_cap = 16u,
	.prefill_len = 8u,
	.prefill_read_pos = 2u,
	.read_len = 0u,

	.expected_read = 0u,
	.expected_st = STREAM_STATUS_OK,
	.expect_backend_unchanged = true
};

static const test_dynamic_buffer_stream_read_case_t
CASE_DYNAMIC_BUFFER_STREAM_READ_BUF_NULL_NONZERO = {
	.name = "dynamic_buffer_stream_read_buf_null_nonzero",
	.scenario = DYNAMIC_BUFFER_STREAM_READ_SCENARIO_BUF_NULL_NONZERO,
	.initial_cap = 16u,
	.prefill_len = 8u,
	.prefill_read_pos = 2u,
	.read_len = 3u,

	.expected_read = 0u,
	.expected_st = STREAM_STATUS_INVALID,
	.expect_backend_unchanged = true
};

static const test_dynamic_buffer_stream_read_case_t
CASE_DYNAMIC_BUFFER_STREAM_READ_EOF = {
	.name = "dynamic_buffer_stream_read_eof",
	.scenario = DYNAMIC_BUFFER_STREAM_READ_SCENARIO_EOF,
	.initial_cap = 16u,
	.prefill_len = 8u,
	.prefill_read_pos = 8u,
	.read_len = 3u,

	.expected_read = 0u,
	.expected_st = STREAM_STATUS_EOF,
	.expect_backend_unchanged = true
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define DYNAMIC_BUFFER_STREAM_READ_CASES(X) \
X(CASE_DYNAMIC_BUFFER_STREAM_READ_OK_EXACT) \
X(CASE_DYNAMIC_BUFFER_STREAM_READ_OK_CLAMP_TO_AVAILABLE) \
X(CASE_DYNAMIC_BUFFER_STREAM_READ_ZERO_LEN) \
X(CASE_DYNAMIC_BUFFER_STREAM_READ_BUF_NULL_ZERO_LEN) \
X(CASE_DYNAMIC_BUFFER_STREAM_READ_BUF_NULL_NONZERO) \
X(CASE_DYNAMIC_BUFFER_STREAM_READ_EOF)

#define DYNAMIC_BUFFER_STREAM_MAKE_READ_TEST(case_sym) \
LEXLEO_MAKE_TEST(dynamic_buffer_stream_read, case_sym)

static const struct CMUnitTest dynamic_buffer_stream_read_tests[] = {
	DYNAMIC_BUFFER_STREAM_READ_CASES(DYNAMIC_BUFFER_STREAM_MAKE_READ_TEST)
};

#undef DYNAMIC_BUFFER_STREAM_READ_CASES
#undef DYNAMIC_BUFFER_STREAM_MAKE_READ_TEST

/** @endcond */

/**
 * @brief Test `dynamic_buffer_stream_flush()`.
 *
 * static stream_status_t dynamic_buffer_stream_flush(void *backend);
 *
 * Success:
 * - returns `STREAM_STATUS_OK`
 * - leaves the backend unchanged
 *
 * Failure:
 * - None.
 *
 * Doubles:
 * - none
 *
 * See also:
 * - @ref testing_foundation_dynamic_buffer_stream_unit_flush "dynamic_buffer_stream_flush() unit tests section"
 * - @ref specifications_dynamic_buffer_stream_flush "dynamic_buffer_stream_flush() specifications"
 */
static void test_dynamic_buffer_stream_flush(void **state)
{
	(void)state;

	stream_t *s = NULL;

	const dynamic_buffer_stream_cfg_t cfg = {
		.default_cap = 16u
	};

	const dynamic_buffer_stream_env_t env = {
		.mem = osal_mem_test_fake_ops(),
		.port_env = {
			.mem = osal_mem_test_fake_ops()
		}
	};

	assert_int_equal(
		dynamic_buffer_stream_create_stream(&s, &cfg, &env),
		STREAM_STATUS_OK);
	assert_non_null(s);
	assert_non_null(s->backend);

	dynamic_buffer_stream_t *dbs = (dynamic_buffer_stream_t *)s->backend;
	assert_non_null(dbs);

	dynamic_buffer_t *dbuf = &dbs->state.dbuf;
	assert_non_null(dbuf);
	assert_non_null(dbuf->buf);

	memcpy(dbuf->buf, "hello", 5);
	dbuf->len = 5u;
	dbuf->read_pos = 2u;

	char before_bytes[5];
	memcpy(before_bytes, dbuf->buf, 5);

	const void *buf_snapshot = dbuf->buf;
	const size_t len_snapshot = dbuf->len;
	const size_t cap_snapshot = dbuf->cap;
	const size_t read_pos_snapshot = dbuf->read_pos;

	assert_int_equal(stream_flush(s), STREAM_STATUS_OK);

	assert_ptr_equal(dbuf->buf, buf_snapshot);
	assert_int_equal((int)dbuf->len, (int)len_snapshot);
	assert_int_equal((int)dbuf->cap, (int)cap_snapshot);
	assert_int_equal((int)dbuf->read_pos, (int)read_pos_snapshot);
	assert_memory_equal(dbuf->buf, before_bytes, sizeof(before_bytes));

	stream_destroy(&s);
	assert_null(s);

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

/**
 * @brief Test `dynamic_buffer_stream_close()`.
 *
 * static void dynamic_buffer_stream_close(void *backend);
 *
 * Success:
 * - backend-owned resources are released during `stream_destroy()`
 * - the public stream handle is destroyed and set to `NULL`
 *
 * Failure:
 * - None.
 *
 * Doubles:
 * - fake_memory
 *
 * See also:
 * - @ref testing_foundation_dynamic_buffer_stream_unit_close "dynamic_buffer_stream_close() unit tests section"
 * - @ref specifications_dynamic_buffer_stream_close "dynamic_buffer_stream_close() specifications"
 * - @ref specifications_stream_destroy "stream_destroy() specifications"
 */
static void test_dynamic_buffer_stream_close(void **state)
{
	(void)state;

	fake_memory_reset();

	stream_t *s = NULL;

	const dynamic_buffer_stream_cfg_t cfg = {
		.default_cap = 16u
	};

	const dynamic_buffer_stream_env_t env = {
		.mem = osal_mem_test_fake_ops(),
		.port_env = {
			.mem = osal_mem_test_fake_ops()
		}
	};

	assert_int_equal(
		dynamic_buffer_stream_create_stream(&s, &cfg, &env),
		STREAM_STATUS_OK);
	assert_non_null(s);
	assert_non_null(s->backend);

	dynamic_buffer_stream_t *dbs = (dynamic_buffer_stream_t *)s->backend;
	assert_non_null(dbs);

	dynamic_buffer_t *dbuf = &dbs->state.dbuf;
	assert_non_null(dbuf);
	assert_non_null(dbuf->buf);

	/*
	 * Seed backend state so destruction exercises a non-trivial backend.
	 */
	memcpy(dbuf->buf, "hello", 5);
	dbuf->len = 5u;
	dbuf->read_pos = 2u;

	/*
	 * Exercise the private close callback through the public lifecycle API.
	 */
	stream_destroy(&s);

	assert_null(s);

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

/** @cond INTERNAL */

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	static const struct CMUnitTest dynamic_buffer_stream_stream_non_parametric_tests[] = {
		cmocka_unit_test(test_dynamic_buffer_stream_default_cfg),
		cmocka_unit_test(test_dynamic_buffer_stream_default_env),
		cmocka_unit_test(test_dynamic_buffer_stream_flush),
		cmocka_unit_test(test_dynamic_buffer_stream_close),
	};

	int failed = 0;
	failed += cmocka_run_group_tests(dynamic_buffer_stream_stream_non_parametric_tests, NULL, NULL);
	failed += cmocka_run_group_tests(dynamic_buffer_stream_create_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(create_desc_dynamic_buffer_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(dynamic_buffer_stream_write_tests, NULL, NULL);
	failed += cmocka_run_group_tests(dynamic_buffer_stream_read_tests, NULL, NULL);

	return failed;
}

/** @endcond */
