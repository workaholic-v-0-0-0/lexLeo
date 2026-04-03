/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_stdio_stream.c
 * @ingroup stdio_stream_unit_tests
 * @brief Unit tests implementation for the `stdio_stream` adapter.
 *
 * @details
 * This file implements the unit-level validation of the `stdio_stream` adapter
 * contracts.
 *
 * Covered surfaces:
 * - CR helpers: `stdio_stream_default_cfg()`, `stdio_stream_default_env()`
 * - direct creation: `stdio_stream_create_stream()`
 * - CR descriptor helper: `stdio_stream_create_desc()`
 * - descriptor constructor: `stdio_stream_ctor()`
 * - borrower-facing backend operations exercised through the public stream
 *   port wrappers:
 *   - `stdio_stream_read()`
 *   - `stdio_stream_write()`
 *   - `stdio_stream_flush()`
 *   - `stdio_stream_close()`
 *
 * Test strategy:
 * - CR helpers are tested directly.
 * - Construction helpers and descriptor helpers are tested with parametric
 *   scenario-driven unit tests.
 * - Private backend callbacks are not called directly; they are exercised
 *   through the public `stream` port wrappers.
 * - `fake_stdio` is used to observe standard-stream behavior and state
 *   transitions.
 * - `fake_memory` is used to inject allocation failures and to verify memory
 *   invariants at teardown.
 *
 * See also:
 * - @ref testing_foundation_stdio_stream_unit "stdio_stream unit tests page"
 * - @ref specifications_stdio_stream "stdio_stream specifications"
 */

#include "stdio_stream/cr/stdio_stream_cr_api.h"

#include "stream/lifecycle/stream_lifecycle.h"
#include "stream/borrowers/stream.h"
#include "osal/stdio/test/osal_stdio_fake_provider.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

// for white-box tests
#include "internal/stdio_stream_handle.h"
#include "internal/stream_handle.h"
#include "internal/stdio_stream_ctor_ud.h"

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
 * @brief Test `stdio_stream_default_cfg()`.
 *
 * stdio_stream_cfg_t stdio_stream_default_cfg(void);
 *
 * Success:
 * - Returns a value-initialized `stdio_stream_cfg_t`.
 * - `ret.reserved == 0`.
 *
 * Failure:
 * - None.
 *
 * See also:
 * - @ref testing_foundation_stdio_stream_unit_default_cfg "stdio_stream_default_cfg() unit tests section"
 * - @ref specifications_stdio_stream_default_cfg "stdio_stream_default_cfg() specifications"
 */
static void test_stdio_stream_default_cfg(void **state)
{
	(void)state;

	stdio_stream_cfg_t ret = stdio_stream_default_cfg();

	assert_int_equal(ret.reserved, 0);
}

/**
 * @brief Test `stdio_stream_default_env()`.
 *
 * stdio_stream_env_t stdio_stream_default_env(
 *     const osal_stdio_ops_t *stdio_ops,
 *     const osal_mem_ops_t *mem,
 *     const stream_env_t *port_env);
 *
 * Success:
 * - `ret.stdio_ops == stdio_ops`.
 * - `ret.mem == mem`.
 * - `ret.port_env == *port_env`.
 *
 * Failure:
 * - None.
 *
 * Doubles:
 * - dummy `osal_stdio_ops_t *`
 * - dummy `osal_mem_ops_t *`
 * - dummy `stream_env_t`
 *
 * See also:
 * - @ref testing_foundation_stdio_stream_unit_default_env "stdio_stream_default_env() unit tests section"
 * - @ref specifications_stdio_stream_default_env "stdio_stream_default_env() specifications".
 */
static void test_stdio_stream_default_env(void **state) {
	(void)state;

	const osal_stdio_ops_t *dummy_stdio_ops_p =
		(const osal_stdio_ops_t *)(uintptr_t)0x1111u;
	const osal_mem_ops_t *dummy_mem_p =
		(const osal_mem_ops_t *)(uintptr_t)0x1234u;

	const stream_env_t dummy_port_env = {0};
	const stream_env_t *dummy_port_env_p = &dummy_port_env;

	stdio_stream_env_t ret = stdio_stream_default_env(
		dummy_stdio_ops_p,
		dummy_mem_p,
		dummy_port_env_p);

	assert_ptr_equal(ret.stdio_ops, dummy_stdio_ops_p);
	assert_ptr_equal(ret.mem, dummy_mem_p);
	assert_memory_equal(&ret.port_env, dummy_port_env_p, sizeof(ret.port_env));
}

/**
 * @brief Scenarios for `stdio_stream_create_stream()`.
 *
 * stream_status_t stdio_stream_create_stream(
 *     stream_t **out,
 *     const stdio_stream_args_t *args,
 *     const stdio_stream_cfg_t *cfg,
 *     const stdio_stream_env_t *env);
 *
 * Contract:
 * - See @ref specifications_stdio_stream_create_stream
 *   "stdio_stream_create_stream() specifications".
 *
 * Doubles:
 * - fake_stdio
 * - fake_memory
 *
 * See also:
 * - @ref testing_foundation_stdio_stream_unit_create_stream
 *   "stdio_stream_create_stream() unit tests section"
 *
 * The scenarios below define the test oracle for
 * `stdio_stream_create_stream()`.
 */
typedef enum {
	/**
	 * WHEN `stdio_stream_create_stream(out, args, cfg, env)` is called with
	 * valid arguments and `args->kind == STDIO_STREAM_KIND_STDOUT`
	 * EXPECT:
	 * - returns `STREAM_STATUS_OK`
	 * - stores a non-NULL stream handle in `*out`
	 * - the produced stream forwards borrower-side write operations to the
	 *   standard output stream
	 */
	STDIO_STREAM_CREATE_STREAM_SCENARIO_OK = 0,

	/**
	 * WHEN `out == NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - no stream handle is produced
	 */
	STDIO_STREAM_CREATE_STREAM_SCENARIO_OUT_NULL,

	/**
	 * WHEN `args == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STDIO_STREAM_CREATE_STREAM_SCENARIO_ARGS_NULL,

	/**
	 * WHEN `cfg == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STDIO_STREAM_CREATE_STREAM_SCENARIO_CFG_NULL,

	/**
	 * WHEN `env == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STDIO_STREAM_CREATE_STREAM_SCENARIO_ENV_NULL,

	/**
	 * WHEN `args != NULL` but `args->kind` does not designate a supported
	 * `stdio_stream_kind_t` value and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STDIO_STREAM_CREATE_STREAM_SCENARIO_UNSUPPORTED_KIND,

	/**
	 * WHEN allocation required by `stdio_stream_create_stream()` fails
	 * EXPECT:
	 * - returns `STREAM_STATUS_OOM`
	 * - leaves `*out` unchanged
	 *
	 * Notes:
	 * - This scenario is exercised by configuring `fake_memory` to fail the
	 *   allocation performed during stream creation.
	 */
	STDIO_STREAM_CREATE_STREAM_SCENARIO_OOM,
} stdio_stream_create_stream_scenario_t;

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
 * @brief One parametric test case for `stdio_stream_create_stream()`.
 *
 * Notes:
 * - `fail_call_idx` is used by `fake_memory` to inject an allocation failure
 *   on a specific call number (used by the OOM scenario).
 */
typedef struct {
	const char *name;

	// arrange
	stdio_stream_create_stream_scenario_t scenario;
	stdio_stream_kind_t kind;
	size_t fail_call_idx; // 0 = no OOM, otherwise 1-based

	// assert
	stream_status_t expected_ret;
	out_expect_t out_expect;
} test_stdio_stream_create_stream_case_t;

/**
 * @brief Runtime fixture for `stdio_stream_create_stream()` tests.
 *
 * Holds:
 * - the stream handle under test,
 * - the injected adapter environment,
 * - the adapter arguments and configuration,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	stream_t *out;

	// injection
	stdio_stream_env_t env;

	stdio_stream_args_t args;
	stdio_stream_cfg_t cfg;

	const test_stdio_stream_create_stream_case_t *tc;
} test_stdio_stream_create_stream_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `stdio_stream_create_stream()` tests.
 */
static int setup_stdio_stream_create_stream(void **state)
{
	const test_stdio_stream_create_stream_case_t *tc =
		(const test_stdio_stream_create_stream_case_t *)(*state);

	test_stdio_stream_create_stream_fixture_t *fx =
		(test_stdio_stream_create_stream_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();
	if (tc->scenario == STDIO_STREAM_CREATE_STREAM_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	fake_stdio_reset();

	// DI
	fx->env.stdio_ops = osal_stdio_test_fake_ops();
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();

	fx->args.kind = tc->kind;

	fx->cfg.reserved = 0; /* Reserved for future use. */

	*state = fx;
	return 0;
}

/**
 * @brief Release the `stdio_stream_create_stream()` test fixture and verify memory invariants.
 */
static int teardown_stdio_stream_create_stream(void **state)
{
	test_stdio_stream_create_stream_fixture_t *fx =
		(test_stdio_stream_create_stream_fixture_t *)(*state);

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
 * @brief Execute one parametric test scenario for `stdio_stream_create_stream()`.
 */
static void test_stdio_stream_create_stream(void **state)
{
	test_stdio_stream_create_stream_fixture_t *fx =
		(test_stdio_stream_create_stream_fixture_t *)(*state);
	const test_stdio_stream_create_stream_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t st = STREAM_STATUS_INVALID;
	stream_status_t ret = STREAM_STATUS_INVALID;

	stream_t **out_arg = &fx->out;
	const stdio_stream_args_t *args_arg = &fx->args;
	const stdio_stream_cfg_t *cfg_arg = &fx->cfg;
	const stdio_stream_env_t *env_arg = &fx->env;

	// invalid args
	if (tc->scenario == STDIO_STREAM_CREATE_STREAM_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == STDIO_STREAM_CREATE_STREAM_SCENARIO_ARGS_NULL) args_arg = NULL;
	if (tc->scenario == STDIO_STREAM_CREATE_STREAM_SCENARIO_CFG_NULL) cfg_arg = NULL;
	if (tc->scenario == STDIO_STREAM_CREATE_STREAM_SCENARIO_ENV_NULL) env_arg = NULL;

    // ensure OUT_EXPECT_UNCHANGED is meaningful
    if (tc->out_expect == OUT_EXPECT_UNCHANGED && out_arg != NULL) {
        fx->out = (stream_t *)(uintptr_t)0xDEADC0DEu; // sentinel
    }

    stream_t *out_arg_snapshot = fx->out;

	// ACT
	ret = stdio_stream_create_stream(out_arg, args_arg, cfg_arg, env_arg);

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

	if (tc->scenario == STDIO_STREAM_CREATE_STREAM_SCENARIO_OK) {
		const uint8_t payload[] = { 'O', 'K' };
		size_t n = stream_write(fx->out, payload, sizeof(payload), &st);

		assert_int_equal(st, STREAM_STATUS_OK);
		assert_int_equal(n, sizeof(payload));

		assert_int_equal(
			fake_stdio_write_call_count(fake_stdio_stdout()),
			1u
		);
		assert_int_equal(
			fake_stdio_buffered_len(fake_stdio_stdout()),
			sizeof(payload)
		);
		assert_memory_equal(
			fake_stdio_buffered_backing(fake_stdio_stdout()),
			payload,
			sizeof(payload)
		);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_stdio_stream_create_stream_case_t
CASE_STDIO_STREAM_CREATE_STREAM_OUT_NULL = {
	.name = "stdio_stream_create_stream_out_null",
	.scenario = STDIO_STREAM_CREATE_STREAM_SCENARIO_OUT_NULL,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_CHECK_NONE
};

static const test_stdio_stream_create_stream_case_t
CASE_STDIO_STREAM_CREATE_STREAM_ARGS_NULL = {
	.name = "stdio_stream_create_stream_arg_null",
	.scenario = STDIO_STREAM_CREATE_STREAM_SCENARIO_ARGS_NULL,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stdio_stream_create_stream_case_t
CASE_STDIO_STREAM_CREATE_STREAM_CFG_NULL = {
	.name = "stdio_stream_create_stream_cfg_null",
	.scenario = STDIO_STREAM_CREATE_STREAM_SCENARIO_CFG_NULL,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stdio_stream_create_stream_case_t
CASE_STDIO_STREAM_CREATE_STREAM_ENV_NULL = {
	.name = "stdio_stream_create_stream_env_null",
	.scenario = STDIO_STREAM_CREATE_STREAM_SCENARIO_ENV_NULL,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stdio_stream_create_stream_case_t
CASE_STDIO_STREAM_CREATE_STREAM_UNSUPPORTED_KIND = {
	.name = "stdio_stream_create_stream_unsupported_kind",
	.scenario = STDIO_STREAM_CREATE_STREAM_SCENARIO_UNSUPPORTED_KIND,
	.kind = STDIO_STREAM_KIND_COUNT,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stdio_stream_create_stream_case_t
CASE_STDIO_STREAM_CREATE_STREAM_OOM_1 = {
	.name = "stdio_stream_create_stream_oom_1",
	.scenario = STDIO_STREAM_CREATE_STREAM_SCENARIO_OOM,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.fail_call_idx = 1,

	.expected_ret = STREAM_STATUS_OOM,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stdio_stream_create_stream_case_t
CASE_STDIO_STREAM_CREATE_STREAM_OK = {
	.name = "stdio_stream_create_stream_ok",
	.scenario = STDIO_STREAM_CREATE_STREAM_SCENARIO_OK,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_OK,
	.out_expect = OUT_EXPECT_NON_NULL
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STDIO_STREAM_CREATE_STREAM_CASES(X) \
X(CASE_STDIO_STREAM_CREATE_STREAM_OUT_NULL) \
X(CASE_STDIO_STREAM_CREATE_STREAM_ARGS_NULL) \
X(CASE_STDIO_STREAM_CREATE_STREAM_CFG_NULL) \
X(CASE_STDIO_STREAM_CREATE_STREAM_ENV_NULL) \
X(CASE_STDIO_STREAM_CREATE_STREAM_UNSUPPORTED_KIND) \
X(CASE_STDIO_STREAM_CREATE_STREAM_OOM_1) \
X(CASE_STDIO_STREAM_CREATE_STREAM_OK)

#define STDIO_STREAM_MAKE_CREATE_STREAM_TEST(case_sym) \
LEXLEO_MAKE_TEST(stdio_stream_create_stream, case_sym)

static const struct CMUnitTest stdio_stream_create_stream_tests[] = {
	STDIO_STREAM_CREATE_STREAM_CASES(
		STDIO_STREAM_MAKE_CREATE_STREAM_TEST)
};

#undef STDIO_STREAM_CREATE_STREAM_CASES
#undef STDIO_STREAM_MAKE_CREATE_STREAM_TEST

/** @endcond */

/**
 * @brief Scenarios for `stdio_stream_create_desc()`.
 *
 * stream_status_t stdio_stream_create_desc(
 *     stream_adapter_desc_t *out,
 *     stream_key_t key,
 *     const stdio_stream_cfg_t *cfg,
 *     const stdio_stream_env_t *env,
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
 * - fake_stdio
 * - fake_memory
 *
 * See also:
 * - @ref testing_foundation_stdio_stream_unit_create_desc
 *   "stdio_stream_create_desc() unit tests section"
 * - @ref specifications_stdio_stream_create_desc
 *   "stdio_stream_create_desc() specifications"
 *
 * The scenarios below define the test oracle for
 * `stdio_stream_create_desc()`.
 */
typedef enum {
	/**
	 * WHEN `stdio_stream_create_desc(out, key, cfg, env, mem)` is
	 * called with valid arguments
	 * EXPECT:
	 * - returns `STREAM_STATUS_OK`
	 * - stores a valid adapter descriptor in `*out`
	 * - the produced descriptor is eligible for later destruction via
	 *   `out->ud_dtor()`
	 */
	STDIO_STREAM_CREATE_DESC_SCENARIO_OK = 0,

	/**
	 * WHEN `out == NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - no descriptor is produced
	 */
	STDIO_STREAM_CREATE_DESC_SCENARIO_OUT_NULL,

	/**
	 * WHEN `key == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	STDIO_STREAM_CREATE_DESC_SCENARIO_KEY_NULL,

	/**
	 * WHEN `key` is an empty string and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	STDIO_STREAM_CREATE_DESC_SCENARIO_KEY_EMPTY,

	/**
	 * WHEN `cfg == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	STDIO_STREAM_CREATE_DESC_SCENARIO_CFG_NULL,

	/**
	 * WHEN `env == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	STDIO_STREAM_CREATE_DESC_SCENARIO_ENV_NULL,

	/**
	 * WHEN `mem == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - resets `*out` to an empty descriptor
	 */
	STDIO_STREAM_CREATE_DESC_SCENARIO_MEM_NULL,

	/**
	 * WHEN allocation required by `stdio_stream_create_desc()` fails
	 * EXPECT:
	 * - returns `STREAM_STATUS_OOM`
	 * - resets `*out` to an empty descriptor
	 *
	 * Notes:
	 * - This scenario is exercised by configuring `fake_memory` to fail the
	 *   allocation performed during descriptor construction.
	 */
	STDIO_STREAM_CREATE_DESC_SCENARIO_OOM
} stdio_stream_create_desc_scenario_t;

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
 * @brief One parametric test case for `stdio_stream_create_desc()`.
 *
 * Notes:
 * - `fail_call_idx` is used by `fake_memory` to inject an allocation failure
 *   on a specific call number (used by the OOM scenario).
 */
typedef struct {
	const char *name;

	// arrange
	stdio_stream_create_desc_scenario_t scenario;
	size_t fail_call_idx; // 0 = no OOM, otherwise 1-based

	// assert
	stream_status_t expected_ret;
	desc_expect_t desc_expect;
} test_stdio_stream_create_desc_case_t;

/**
 * @brief Runtime fixture for `stdio_stream_create_desc()` tests.
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
	stdio_stream_env_t env;
	const osal_mem_ops_t *mem;

	stream_key_t key;
	stdio_stream_cfg_t cfg;

	const test_stdio_stream_create_desc_case_t *tc;
} test_stdio_stream_create_desc_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for
 * `stdio_stream_create_desc()` tests.
 */
static int setup_stdio_stream_create_desc(void **state)
{
	const test_stdio_stream_create_desc_case_t *tc =
		(const test_stdio_stream_create_desc_case_t *)(*state);

	test_stdio_stream_create_desc_fixture_t *fx =
		(test_stdio_stream_create_desc_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();
	if (tc->scenario == STDIO_STREAM_CREATE_DESC_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	fake_stdio_reset();

	// DI
	fx->env.stdio_ops = osal_stdio_test_fake_ops();
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();
	fx->mem = osal_mem_test_fake_ops();

	fx->key = "stdio";

	fx->cfg.reserved = 0;

	*state = fx;
	return 0;
}

/**
 * @brief Release the `stdio_stream_create_desc()` test fixture and
 * verify memory invariants.
 */
static int teardown_stdio_stream_create_desc(void **state)
{
	test_stdio_stream_create_desc_fixture_t *fx =
		(test_stdio_stream_create_desc_fixture_t *)(*state);

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
 * `stdio_stream_create_desc()`.
 */
static void test_stdio_stream_create_desc(void **state)
{
	test_stdio_stream_create_desc_fixture_t *fx =
		(test_stdio_stream_create_desc_fixture_t *)(*state);
	const test_stdio_stream_create_desc_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t ret = STREAM_STATUS_INVALID;

	stream_adapter_desc_t *out_arg = &fx->out;
	stream_key_t key_arg = fx->key;
	const stdio_stream_cfg_t *cfg_arg = &fx->cfg;
	const stdio_stream_env_t *env_arg = &fx->env;
	const osal_mem_ops_t *mem_arg = fx->mem;

	// invalid args
	if (tc->scenario == STDIO_STREAM_CREATE_DESC_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == STDIO_STREAM_CREATE_DESC_SCENARIO_KEY_NULL) key_arg = NULL;
	if (tc->scenario == STDIO_STREAM_CREATE_DESC_SCENARIO_KEY_EMPTY) key_arg = "";
	if (tc->scenario == STDIO_STREAM_CREATE_DESC_SCENARIO_CFG_NULL) cfg_arg = NULL;
	if (tc->scenario == STDIO_STREAM_CREATE_DESC_SCENARIO_ENV_NULL) env_arg = NULL;
	if (tc->scenario == STDIO_STREAM_CREATE_DESC_SCENARIO_MEM_NULL) mem_arg = NULL;

	if (tc->desc_expect == DESC_EXPECT_EMPTY && out_arg != NULL) {
		fx->out.key = (stream_key_t)(uintptr_t)0xDEADC0DEu;
		fx->out.ctor = (void *)(uintptr_t)0xDEADC0DEu;
		fx->out.ud = (void *)(uintptr_t)0xDEADC0DEu;
		fx->out.ud_dtor = (void *)(uintptr_t)0xDEADC0DEu;
	}

	// ACT
	ret = stdio_stream_create_desc(out_arg, key_arg, cfg_arg, env_arg, mem_arg);

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

static const test_stdio_stream_create_desc_case_t
CASE_STDIO_STREAM_CREATE_DESC_OUT_NULL = {
	.name = "stdio_stream_create_desc_out_null",
	.scenario = STDIO_STREAM_CREATE_DESC_SCENARIO_OUT_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_CHECK_NONE
};

static const test_stdio_stream_create_desc_case_t
CASE_STDIO_STREAM_CREATE_DESC_KEY_NULL = {
	.name = "stdio_stream_create_desc_key_null",
	.scenario = STDIO_STREAM_CREATE_DESC_SCENARIO_KEY_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_stdio_stream_create_desc_case_t
CASE_STDIO_STREAM_CREATE_DESC_KEY_EMPTY = {
	.name = "stdio_stream_create_desc_key_empty",
	.scenario = STDIO_STREAM_CREATE_DESC_SCENARIO_KEY_EMPTY,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_stdio_stream_create_desc_case_t
CASE_STDIO_STREAM_CREATE_DESC_CFG_NULL = {
	.name = "stdio_stream_create_desc_cfg_null",
	.scenario = STDIO_STREAM_CREATE_DESC_SCENARIO_CFG_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_stdio_stream_create_desc_case_t
CASE_STDIO_STREAM_CREATE_DESC_ENV_NULL = {
	.name = "stdio_stream_create_desc_env_null",
	.scenario = STDIO_STREAM_CREATE_DESC_SCENARIO_ENV_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_stdio_stream_create_desc_case_t
CASE_STDIO_STREAM_CREATE_DESC_MEM_NULL = {
	.name = "stdio_stream_create_desc_mem_null",
	.scenario = STDIO_STREAM_CREATE_DESC_SCENARIO_MEM_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_stdio_stream_create_desc_case_t
CASE_STDIO_STREAM_CREATE_DESC_OOM_1 = {
	.name = "stdio_stream_create_desc_oom_1",
	.scenario = STDIO_STREAM_CREATE_DESC_SCENARIO_OOM,
	.fail_call_idx = 1,

	.expected_ret = STREAM_STATUS_OOM,
	.desc_expect = DESC_EXPECT_EMPTY
};

static const test_stdio_stream_create_desc_case_t
CASE_STDIO_STREAM_CREATE_DESC_OK = {
	.name = "stdio_stream_create_desc_ok",
	.scenario = STDIO_STREAM_CREATE_DESC_SCENARIO_OK,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_OK,
	.desc_expect = DESC_EXPECT_VALID
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STDIO_STREAM_CREATE_DESC_CASES(X) \
X(CASE_STDIO_STREAM_CREATE_DESC_OUT_NULL) \
X(CASE_STDIO_STREAM_CREATE_DESC_KEY_NULL) \
X(CASE_STDIO_STREAM_CREATE_DESC_KEY_EMPTY) \
X(CASE_STDIO_STREAM_CREATE_DESC_CFG_NULL) \
X(CASE_STDIO_STREAM_CREATE_DESC_ENV_NULL) \
X(CASE_STDIO_STREAM_CREATE_DESC_MEM_NULL) \
X(CASE_STDIO_STREAM_CREATE_DESC_OOM_1) \
X(CASE_STDIO_STREAM_CREATE_DESC_OK)

#define STDIO_STREAM_MAKE_CREATE_DESC_TEST(case_sym) \
LEXLEO_MAKE_TEST(stdio_stream_create_desc, case_sym)

static const struct CMUnitTest create_desc_stdio_stream_tests[] = {
	STDIO_STREAM_CREATE_DESC_CASES(
		STDIO_STREAM_MAKE_CREATE_DESC_TEST)
};

#undef STDIO_STREAM_CREATE_DESC_CASES
#undef STDIO_STREAM_MAKE_CREATE_DESC_TEST

/** @endcond */

/**
 * @brief Scenarios for `stdio_stream_ctor()`.
 *
 * stream_status_t stdio_stream_ctor(
 *     const void *ud,
 *     const void *args,
 *     stream_t **out);
 *
 * Contract:
 * - See @ref specifications_stdio_stream_ctor
 *   "stdio_stream_ctor() specifications".
 *
 * Doubles:
 * - fake_stdio
 * - fake_memory
 *
 * See also:
 * - @ref testing_foundation_stdio_stream_unit_ctor
 *   "stdio_stream_ctor() unit tests section"
 *
 * The scenarios below define the test oracle for
 * `stdio_stream_ctor()`.
 */
typedef enum {
	/**
	 * WHEN `stdio_stream_ctor(ud, args, out)` is called with
	 * valid arguments and `args->kind == STDIO_STREAM_KIND_STDOUT`
	 * EXPECT:
	 * - returns `STREAM_STATUS_OK`
	 * - stores a non-NULL stream handle in `*out`
	 * - the produced stream forwards borrower-side write operations to the
	 *   standard output stream
	 */
	STDIO_STREAM_CTOR_SCENARIO_OK = 0,

	/**
	 * WHEN `out == NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - no stream handle is produced
	 */
	STDIO_STREAM_CTOR_SCENARIO_OUT_NULL,

	/**
	 * WHEN `args == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STDIO_STREAM_CTOR_SCENARIO_ARGS_NULL,

	/**
	 * WHEN `ud == NULL` and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STDIO_STREAM_CTOR_SCENARIO_UD_NULL,

	/**
	 * WHEN `args != NULL` but `args->kind` does not designate a supported
	 * `stdio_stream_kind_t` value and `out != NULL`
	 * EXPECT:
	 * - returns `STREAM_STATUS_INVALID`
	 * - leaves `*out` unchanged
	 */
	STDIO_STREAM_CTOR_SCENARIO_UNSUPPORTED_KIND,

	/**
	 * WHEN allocation required by `stdio_stream_ctor()` fails
	 * EXPECT:
	 * - returns `STREAM_STATUS_OOM`
	 * - leaves `*out` unchanged
	 *
	 * Notes:
	 * - This scenario is exercised by configuring `fake_memory` to fail the
	 *   allocation performed during delegated stream creation.
	 */
	STDIO_STREAM_CTOR_SCENARIO_OOM,
} stdio_stream_ctor_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `stdio_stream_ctor()`.
 *
 * Notes:
 * - `fail_call_idx` is used by `fake_memory` to inject an allocation failure
 *   on a specific call number (used by the OOM scenario).
 */
typedef struct {
	const char *name;

	// arrange
	stdio_stream_ctor_scenario_t scenario;
	stdio_stream_kind_t kind;
	size_t fail_call_idx; // 0 = no OOM, otherwise 1-based

	// assert
	stream_status_t expected_ret;
	out_expect_t out_expect;
} test_stdio_stream_ctor_case_t;

/**
 * @brief Runtime fixture for `stdio_stream_ctor()` tests.
 *
 * Holds:
 * - the stream handle under test,
 * - the injected adapter environment,
 * - the adapter arguments and configuration,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	stream_t *out;
	stdio_stream_ctor_ud_t ud;

	// injection
	stdio_stream_env_t env;

	stdio_stream_args_t args;
	stdio_stream_cfg_t cfg;

	const test_stdio_stream_ctor_case_t *tc;
} test_stdio_stream_ctor_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `stdio_stream_ctor()` tests.
 */
static int setup_stdio_stream_ctor(void **state)
{
	const test_stdio_stream_ctor_case_t *tc =
		(const test_stdio_stream_ctor_case_t *)(*state);

	test_stdio_stream_ctor_fixture_t *fx =
		(test_stdio_stream_ctor_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();
	if (tc->scenario == STDIO_STREAM_CTOR_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	fake_stdio_reset();

	// DI
	fx->env.stdio_ops = osal_stdio_test_fake_ops();
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();
	fx->args.kind = tc->kind;
	fx->cfg.reserved = 0; /* Reserved for future use. */
	fx->ud.cfg = fx->cfg;
	fx->ud.env = fx->env;

	*state = fx;
	return 0;
}

/**
 * @brief Release the `stdio_stream_ctor()` test fixture and verify memory invariants.
 */
static int teardown_stdio_stream_ctor(void **state)
{
	test_stdio_stream_ctor_fixture_t *fx =
		(test_stdio_stream_ctor_fixture_t *)(*state);

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
 * @brief Execute one parametric test scenario for `stdio_stream_ctor()`.
 */
static void test_stdio_stream_ctor(void **state)
{
	test_stdio_stream_ctor_fixture_t *fx =
		(test_stdio_stream_ctor_fixture_t *)(*state);
	const test_stdio_stream_ctor_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t st = STREAM_STATUS_INVALID;
	stream_status_t ret = STREAM_STATUS_INVALID;

	stream_t **out_arg = &fx->out;
	const stdio_stream_args_t *args_arg = &fx->args;
	const void *ud_arg = (const void *)&fx->ud;

	// invalid args
	if (tc->scenario == STDIO_STREAM_CTOR_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == STDIO_STREAM_CTOR_SCENARIO_ARGS_NULL) args_arg = NULL;
	if (tc->scenario == STDIO_STREAM_CTOR_SCENARIO_UD_NULL) ud_arg = NULL;

    // ensure OUT_EXPECT_UNCHANGED is meaningful
    if (tc->out_expect == OUT_EXPECT_UNCHANGED && out_arg != NULL) {
        fx->out = (stream_t *)(uintptr_t)0xDEADC0DEu; // sentinel
    }

    stream_t *out_arg_snapshot = fx->out;

	// ACT
	ret = stdio_stream_ctor(ud_arg, args_arg, out_arg);

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

	if (tc->scenario == STDIO_STREAM_CTOR_SCENARIO_OK) {
		const uint8_t payload[] = { 'O', 'K' };
		size_t n = stream_write(fx->out, payload, sizeof(payload), &st);

		assert_int_equal(st, STREAM_STATUS_OK);
		assert_int_equal(n, sizeof(payload));

		assert_int_equal(
			fake_stdio_write_call_count(fake_stdio_stdout()),
			1u
		);
		assert_int_equal(
			fake_stdio_buffered_len(fake_stdio_stdout()),
			sizeof(payload)
		);
		assert_memory_equal(
			fake_stdio_buffered_backing(fake_stdio_stdout()),
			payload,
			sizeof(payload)
		);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_stdio_stream_ctor_case_t
CASE_STDIO_STREAM_CTOR_OUT_NULL = {
	.name = "stdio_stream_ctor_out_null",
	.scenario = STDIO_STREAM_CTOR_SCENARIO_OUT_NULL,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_CHECK_NONE
};

static const test_stdio_stream_ctor_case_t
CASE_STDIO_STREAM_CTOR_ARGS_NULL = {
	.name = "stdio_stream_ctor_args_null",
	.scenario = STDIO_STREAM_CTOR_SCENARIO_ARGS_NULL,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stdio_stream_ctor_case_t
CASE_STDIO_STREAM_CTOR_UD_NULL = {
	.name = "stdio_stream_ctor_ud_null",
	.scenario = STDIO_STREAM_CTOR_SCENARIO_UD_NULL,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stdio_stream_ctor_case_t
CASE_STDIO_STREAM_CTOR_UNSUPPORTED_KIND = {
	.name = "stdio_stream_ctor_unsupported_kind",
	.scenario = STDIO_STREAM_CTOR_SCENARIO_UNSUPPORTED_KIND,
	.kind = STDIO_STREAM_KIND_COUNT,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stdio_stream_ctor_case_t
CASE_STDIO_STREAM_CTOR_OOM_1 = {
	.name = "stdio_stream_ctor_oom_1",
	.scenario = STDIO_STREAM_CTOR_SCENARIO_OOM,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.fail_call_idx = 1,

	.expected_ret = STREAM_STATUS_OOM,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stdio_stream_ctor_case_t
CASE_STDIO_STREAM_CTOR_OK = {
	.name = "stdio_stream_ctor_ok",
	.scenario = STDIO_STREAM_CTOR_SCENARIO_OK,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_OK,
	.out_expect = OUT_EXPECT_NON_NULL
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STDIO_STREAM_CTOR_CASES(X) \
X(CASE_STDIO_STREAM_CTOR_OUT_NULL) \
X(CASE_STDIO_STREAM_CTOR_ARGS_NULL) \
X(CASE_STDIO_STREAM_CTOR_UD_NULL) \
X(CASE_STDIO_STREAM_CTOR_UNSUPPORTED_KIND) \
X(CASE_STDIO_STREAM_CTOR_OOM_1) \
X(CASE_STDIO_STREAM_CTOR_OK)

#define STDIO_STREAM_MAKE_CTOR_TEST(case_sym) \
LEXLEO_MAKE_TEST(stdio_stream_ctor, case_sym)

static const struct CMUnitTest stdio_stream_ctor_tests[] = {
	STDIO_STREAM_CTOR_CASES(
		STDIO_STREAM_MAKE_CTOR_TEST)
};

#undef STDIO_STREAM_CTOR_CASES
#undef STDIO_STREAM_MAKE_CTOR_TEST

/** @endcond */

/**
 * @brief Scenarios for `stdio_stream_write()`.
 *
 * static size_t stdio_stream_write(
 *     void *backend,
 *     const void *buf,
 *     size_t n,
 *     stream_status_t *st );
 *
 * This private callback is exercised through the public `stream_write()` API
 * on streams created by `stdio_stream_create_stream()`.
 *
 * Invalid arguments:
 * - `backend == NULL`
 * - `buf == NULL && n > 0`
 *
 * Success:
 * - Returns the number of bytes written.
 * - Appends bytes to the buffered backing of the wrapped standard stream when
 *   that stream supports writing.
 * - Sets `*st` to `STREAM_STATUS_OK` when `st != NULL`.
 *
 * Failure:
 * - Returns `0` and sets `*st` to `STREAM_STATUS_INVALID` for invalid
 *   arguments when `st != NULL`.
 * - Returns `0` and sets `*st` to `STREAM_STATUS_IO_ERROR` when the bound
 *   standard stream does not support writing.
 * - Leaves the fake buffered state unchanged on failure paths covered here.
 *
 * Doubles:
 * - fake_stdio
 * - fake_memory
 *
 * Fake behavior:
 * - fake_stdio models standard streams with an explicit internal buffer
 *   (`buffered_backing`) and a flushed output (`sink_backing`)
 * - write operations append to the buffered backing
 * - flush operations transfer buffered data to the sink backing
 * - no implicit flush occurs during write
 *
 * Isolation:
 * - the callback is not called directly
 * - behavior is exercised through the public `stream` port wrapper
 *
 * See also:
 * - @ref testing_foundation_stdio_stream_unit_write
 *   "stdio_stream_write() unit tests section"
 * - @ref specifications_stdio_stream_write
 *   "stdio_stream_write() specifications"
 * - @ref specifications_stream_write
 *   "stream_write() specifications"
 *
 * The scenarios below define the test oracle for `stdio_stream_write()`.
 */
typedef enum {
	/**
	 * WHEN `stream_write(s, buf, n, &st)` is called with valid arguments
	 * AND `args->kind == STDIO_STREAM_KIND_STDOUT`
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `n`
	 * - `st == STREAM_STATUS_OK`
	 * - the `n` bytes from `buf` are appended to the buffered backing of the
	 *   bound standard output stream
	 * - the sink backing of the bound standard output stream is unchanged
	 */
	STDIO_STREAM_WRITE_SCENARIO_OK_STDOUT = 0,

	/**
	 * WHEN `stream_write(s, buf, n, &st)` is called with valid arguments
	 * AND `args->kind == STDIO_STREAM_KIND_STDERR`
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `n`
	 * - `st == STREAM_STATUS_OK`
	 * - the `n` bytes from `buf` are appended to the buffered backing of the
	 *   bound standard error stream
	 * - the sink backing of the bound standard error stream is unchanged
	 */
	STDIO_STREAM_WRITE_SCENARIO_OK_STDERR,

	/**
	 * WHEN `stream_write(s, buf, 0, &st)` is called
	 * AND `args->kind` designates an output-oriented standard stream
	 * AND `buf != NULL`
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `st == STREAM_STATUS_OK`
	 * - the buffered backing of the bound standard stream is unchanged
	 */
	STDIO_STREAM_WRITE_SCENARIO_ZERO_LEN,

	/**
	 * WHEN `stream_write(s, NULL, 0, &st)` is called
	 * AND `args->kind` designates an output-oriented standard stream
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `st == STREAM_STATUS_OK`
	 * - the buffered backing of the bound standard stream is unchanged
	 */
	STDIO_STREAM_WRITE_SCENARIO_BUF_NULL_ZERO_LEN,

	/**
	 * WHEN `stream_write(s, NULL, n, &st)` is called with `n > 0`
	 * AND `args->kind` designates an output-oriented standard stream
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `st == STREAM_STATUS_INVALID`
	 * - the buffered backing of the bound standard stream is unchanged
	 */
	STDIO_STREAM_WRITE_SCENARIO_BUF_NULL_NONZERO,

	/**
	 * WHEN `stream_write(s, buf, n, NULL)` is called with valid arguments
	 * AND `args->kind` designates an output-oriented standard stream
	 * EXPECT:
	 * - returns `n`
	 * - the `n` bytes from `buf` are appended to the buffered backing of the
	 *   bound standard output stream
	 * - the sink backing of the bound standard output stream is unchanged
	 * - no status is written
	 */
	STDIO_STREAM_WRITE_SCENARIO_ST_NULL_STDOUT,

	/**
	 * WHEN `stream_write(s, buf, n, NULL)` is called with valid arguments
	 * AND `args->kind == STDIO_STREAM_KIND_STDERR`
	 * EXPECT:
	 * - returns `n`
	 * - the `n` bytes from `buf` are appended to the buffered backing of the
	 *   bound standard error stream
	 * - the sink backing of the bound standard error stream is unchanged
	 * - no status is written
	 */
	STDIO_STREAM_WRITE_SCENARIO_ST_NULL_STDERR,

	/**
	 * WHEN `stream_write(s, buf, n, &st)` is called with valid arguments
	 * AND `args->kind == STDIO_STREAM_KIND_STDIN`
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `st == STREAM_STATUS_IO_ERROR`
	 * - the buffered backing of the bound standard stream is unchanged
	 */
	STDIO_STREAM_WRITE_SCENARIO_STDIN_FORBIDDEN
} stdio_stream_write_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `stdio_stream_write()`.
 *
 * Notes:
 * - `kind` selects which standard stream is wrapped by the adapter instance
 *   created for the test.
 * - `write_data` and `write_len` define the byte sequence passed to
 *   `stream_write()`.
 * - `pass_status_ptr` tells whether the call under test passes a non-NULL
 *   status pointer.
 * - `expected_ret` is the value expected from `stream_write()`.
 * - `expected_st` is asserted only when `pass_status_ptr == true`.
 * - `expected_buffered_backing` and `expected_buffered_len` describe the
 *   expected final state of the fake buffered backing after the call under
 *   test.
 * - `expect_backing_invariance` tells whether the test must assert that the
 *   whole fake buffered backing remains unchanged after the call under test.
 */
typedef struct {
	const char *name;

	/* arrange */
	stdio_stream_write_scenario_t scenario;
	stdio_stream_kind_t kind;
	uint8_t write_data[FAKE_STDIO_BUF_SIZE];
	size_t write_len;
	bool pass_status_ptr;

	/* assert */
	size_t expected_ret;
	stream_status_t expected_st;
	uint8_t expected_buffered_backing[FAKE_STDIO_BUF_SIZE];
	size_t expected_buffered_len;
	bool expect_backing_invariance;
} test_stdio_stream_write_case_t;

/**
 * @brief Runtime fixture for `stdio_stream_write()` tests.
 *
 * Holds:
 * - the public stream handle under test,
 * - a snapshot of the fake buffered backing taken before the call under test,
 * - the injected adapter environment and configuration,
 * - the creation arguments selecting the bound standard stream,
 * - a status object used when the test passes a non-NULL status pointer,
 * - a pointer to the active parametric test case.
 *
 * Notes:
 * - the bytes passed to `stream_write()` are provided by the active test case,
 * - the buffered backing snapshot is used to preserve the fake state observed
 *   before the call under test when needed by the test logic.
 */
typedef struct {
	/* runtime resources */
	stream_t *s;
	uint8_t buffered_backing_snapshot[FAKE_STDIO_BUF_SIZE];
	size_t buffered_backing_snapshot_len;

	/* DI */
	stdio_stream_env_t env;
	stdio_stream_cfg_t cfg;

	stdio_stream_args_t args;

	/* status storage used when `st != NULL` */
	stream_status_t st;

	const test_stdio_stream_write_case_t *tc;
} test_stdio_stream_write_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for
 * `stdio_stream_write()` tests.
 */
static int setup_stdio_stream_write(void **state)
{
	const test_stdio_stream_write_case_t *tc =
		(const test_stdio_stream_write_case_t *)(*state);

	test_stdio_stream_write_fixture_t *fx =
		(test_stdio_stream_write_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_stdio_reset();
	fake_memory_reset();

	// DI
	fx->env.stdio_ops = osal_stdio_test_fake_ops();
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();

	// creation of the stream handle with stdio_stream backend
	fx->cfg.reserved = 0;
	fx->args.kind = tc->kind;
	if (stdio_stream_create_stream(&fx->s, &fx->args, &fx->cfg, &fx->env) != STREAM_STATUS_OK) {
		free(fx);
		return -1;
	}

	*state = fx;
	return 0;
}

/**
 * @brief Release the `stdio_stream_write()` test fixture and verify
 * memory invariants.
 */
static int teardown_stdio_stream_write(void **state)
{
	test_stdio_stream_write_fixture_t *fx =
		(test_stdio_stream_write_fixture_t *)(*state);

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
 * @brief Execute one parametric test scenario for `stdio_stream_write()`.
 *
 * Notes:
 * - This test intentionally exercises the private backend callback through the
 *   public `stream_write()` wrapper.
 * - It assumes the unit test has visibility on internal backend/layout types.
 */
static void test_stdio_stream_write(void **state)
{
	test_stdio_stream_write_fixture_t *fx =
		(test_stdio_stream_write_fixture_t *)(*state);
	const test_stdio_stream_write_case_t *tc = fx->tc;

	OSAL_STDIO *stdio = ((stdio_stream_t *)fx->s->backend)->stdio;

	// ARRANGE
	size_t ret = 0;
	stream_status_t st = STREAM_STATUS_OK;

	const void *buf_arg = tc->write_data;
	size_t n_arg = tc->write_len;
	stream_status_t *st_arg = &st;

	if (tc->scenario == STDIO_STREAM_WRITE_SCENARIO_ZERO_LEN) {
		n_arg = 0;
	}
	if (tc->scenario == STDIO_STREAM_WRITE_SCENARIO_BUF_NULL_ZERO_LEN) {
		buf_arg = NULL;
		n_arg = 0;
	}
	if (tc->scenario == STDIO_STREAM_WRITE_SCENARIO_BUF_NULL_NONZERO) {
		buf_arg = NULL;
	}
	if (!tc->pass_status_ptr) st_arg = NULL;

	if (tc->expect_backing_invariance) {
		fx->buffered_backing_snapshot_len = fake_stdio_buffered_len(stdio);
		memcpy(
			fx->buffered_backing_snapshot,
			fake_stdio_buffered_backing(stdio),
			FAKE_STDIO_BUF_SIZE
		);
	}

	// ACT
	ret = stream_write(fx->s, buf_arg, n_arg, st_arg);

	// ASSERT
	assert_int_equal((int)ret, (int)tc->expected_ret);
	if (tc->pass_status_ptr) {
		assert_int_equal((int)st, (int)tc->expected_st);
	}

	assert_int_equal(
		(int)fake_stdio_buffered_len(stdio),
		(int)tc->expected_buffered_len);
	if (tc->expected_buffered_len > 0) {
		assert_memory_equal(
			fake_stdio_buffered_backing(stdio),
			tc->expected_buffered_backing,
			tc->expected_buffered_len
		);
	}

	if (tc->expect_backing_invariance) {
		assert_int_equal(
			(int)fake_stdio_buffered_len(stdio),
			(int)fx->buffered_backing_snapshot_len
		);
		assert_memory_equal(
			fake_stdio_buffered_backing(stdio),
			fx->buffered_backing_snapshot,
			FAKE_STDIO_BUF_SIZE
		);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_stdio_stream_write_case_t
CASE_STDIO_STREAM_WRITE_ZERO_LEN = {
	.name = "stdio_stream_write_zero_len",
	.scenario = STDIO_STREAM_WRITE_SCENARIO_ZERO_LEN,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.pass_status_ptr = true,

	.expected_ret = 0,
	.expected_st = STREAM_STATUS_OK,
	.expected_buffered_len = 0,
	.expect_backing_invariance = true
};

static const test_stdio_stream_write_case_t
CASE_STDIO_STREAM_WRITE_BUF_NULL_ZERO_LEN = {
	.name = "stdio_stream_write_buf_null_zero_len",
	.scenario = STDIO_STREAM_WRITE_SCENARIO_BUF_NULL_ZERO_LEN,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.pass_status_ptr = true,

	.expected_ret = 0,
	.expected_st = STREAM_STATUS_OK,
	.expected_buffered_len = 0,
	.expect_backing_invariance = true
};

static const test_stdio_stream_write_case_t
CASE_STDIO_STREAM_WRITE_BUF_NULL_NONZERO = {
	.name = "stdio_stream_write_buf_null_nonzero",
	.scenario = STDIO_STREAM_WRITE_SCENARIO_BUF_NULL_NONZERO,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.write_len = 1,
	.pass_status_ptr = true,

	.expected_ret = 0,
	.expected_st = STREAM_STATUS_INVALID,
	.expected_buffered_len = 0,
	.expect_backing_invariance = true
};

static const test_stdio_stream_write_case_t
CASE_STDIO_STREAM_WRITE_STDIN_FORBIDDEN = {
	.name = "stdio_stream_write_stdin_forbidden",
	.scenario = STDIO_STREAM_WRITE_SCENARIO_STDIN_FORBIDDEN,
	.kind = STDIO_STREAM_KIND_STDIN,
	.write_data ={ 0x01, 0x02, 0x03 },
	.write_len = 3,
	.pass_status_ptr = true,

	.expected_ret = 0,
	.expected_st = STREAM_STATUS_IO_ERROR,
	.expect_backing_invariance = true
};

static const test_stdio_stream_write_case_t
CASE_STDIO_STREAM_WRITE_ST_NULL_STDOUT = {
	.name = "stdio_stream_write_st_null_stdout",
	.scenario = STDIO_STREAM_WRITE_SCENARIO_ST_NULL_STDOUT,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.write_data ={ 0x01, 0x02, 0x03 },
	.write_len = 3,
	.pass_status_ptr = false,

	.expected_ret = 3,
	.expected_buffered_backing ={ 0x01, 0x02, 0x03 },
	.expected_buffered_len = 3,
};

static const test_stdio_stream_write_case_t
CASE_STDIO_STREAM_WRITE_ST_NULL_STDERR = {
	.name = "stdio_stream_write_st_null_stderr",
	.scenario = STDIO_STREAM_WRITE_SCENARIO_ST_NULL_STDERR,
	.kind = STDIO_STREAM_KIND_STDERR,
	.write_data ={ 0x01, 0x02, 0x03 },
	.write_len = 3,
	.pass_status_ptr = false,

	.expected_ret = 3,
	.expected_buffered_backing ={ 0x01, 0x02, 0x03 },
	.expected_buffered_len = 3,
};

static const test_stdio_stream_write_case_t
CASE_STDIO_STREAM_WRITE_OK_STDOUT = {
	.name = "stdio_stream_write_ok_stdout",
	.scenario = STDIO_STREAM_WRITE_SCENARIO_OK_STDOUT,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.write_data ={ 0x01, 0x02, 0x03 },
	.write_len = 3,
	.pass_status_ptr = true,

	.expected_ret = 3,
	.expected_st = STREAM_STATUS_OK,
	.expected_buffered_backing ={ 0x01, 0x02, 0x03 },
	.expected_buffered_len = 3,
};

static const test_stdio_stream_write_case_t
CASE_STDIO_STREAM_WRITE_OK_STDERR = {
	.name = "stdio_stream_write_ok_stderr",
	.scenario = STDIO_STREAM_WRITE_SCENARIO_OK_STDERR,
	.kind = STDIO_STREAM_KIND_STDERR,
	.write_data ={ 0x01, 0x02, 0x03 },
	.write_len = 3,
	.pass_status_ptr = true,

	.expected_ret = 3,
	.expected_st = STREAM_STATUS_OK,
	.expected_buffered_backing ={ 0x01, 0x02, 0x03 },
	.expected_buffered_len = 3,
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STDIO_STREAM_WRITE_CASES(X) \
X(CASE_STDIO_STREAM_WRITE_ZERO_LEN) \
X(CASE_STDIO_STREAM_WRITE_BUF_NULL_ZERO_LEN) \
X(CASE_STDIO_STREAM_WRITE_BUF_NULL_NONZERO) \
X(CASE_STDIO_STREAM_WRITE_STDIN_FORBIDDEN) \
X(CASE_STDIO_STREAM_WRITE_ST_NULL_STDOUT) \
X(CASE_STDIO_STREAM_WRITE_ST_NULL_STDERR) \
X(CASE_STDIO_STREAM_WRITE_OK_STDOUT) \
X(CASE_STDIO_STREAM_WRITE_OK_STDERR)

#define STDIO_STREAM_MAKE_WRITE_TEST(case_sym) \
LEXLEO_MAKE_TEST(stdio_stream_write, case_sym)

static const struct CMUnitTest stdio_stream_write_tests[] = {
	STDIO_STREAM_WRITE_CASES(STDIO_STREAM_MAKE_WRITE_TEST)
};

#undef STDIO_STREAM_WRITE_CASES
#undef STDIO_STREAM_MAKE_WRITE_TEST

/** @endcond */

/**
 * @brief Scenarios for `stdio_stream_read()`.
 *
 * static size_t stdio_stream_read(
 *     void *backend,
 *     void *buf,
 *     size_t n,
 *     stream_status_t *st );
 *
 * This private callback is exercised through the public `stream_read()` API
 * on streams created by `stdio_stream_create_stream()`.
 *
 * Invalid arguments:
 * - `backend == NULL`
 * - `buf == NULL && n > 0`
 *
 * Success:
 * - Returns the number of bytes read.
 * - Copies bytes from the buffered backing of the wrapped standard stream into
 *   `buf` when that stream supports reading.
 * - Advances the fake read position by the number of bytes actually read.
 * - Sets `*st` to `STREAM_STATUS_OK` when `st != NULL`.
 *
 * Failure:
 * - Returns `0` and sets `*st` to `STREAM_STATUS_INVALID` for invalid
 *   arguments when `st != NULL`.
 * - Returns `0` and sets `*st` to `STREAM_STATUS_IO_ERROR` when the bound
 *   standard stream does not support reading.
 * - Leaves the buffered backing unchanged.
 *
 * Doubles:
 * - fake_stdio
 * - fake_memory
 *
 * Fake behavior:
 * - fake_stdio models readable standard-stream data with an explicit internal
 *   buffer (`buffered_backing`)
 * - read operations copy bytes from the buffered backing into the caller
 *   buffer
 * - read operations advance the fake read cursor without mutating the
 *   buffered backing itself
 * - the fake read cursor is tracked with `read_pos`
 *
 * Isolation:
 * - the callback is not called directly
 * - behavior is exercised through the public `stream` port wrapper
 *
 * See also:
 * - @ref testing_foundation_stdio_stream_unit_read
 *   "stdio_stream_read() unit tests section"
 * - @ref specifications_stdio_stream_read
 *   "stdio_stream_read() specifications"
 * - @ref specifications_stream_read
 *   "stream_read() specifications"
 *
 * The scenarios below define the test oracle for `stdio_stream_read()`.
 */
typedef enum {
	/**
	 * WHEN `stream_read(s, buf, 0, &st)` is called
	 * AND `args->kind == STDIO_STREAM_KIND_STDIN`
	 * AND `buf != NULL`
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `st == STREAM_STATUS_OK`
	 * - the fake read position of the bound standard input stream is unchanged
	 * - `buf` is unchanged
	 * - the buffered backing of the bound standard input stream is unchanged
	 */
	STDIO_STREAM_READ_SCENARIO_ZERO_LEN = 0,

	/**
	 * WHEN `stream_read(s, NULL, 0, &st)` is called
	 * AND `args->kind == STDIO_STREAM_KIND_STDIN`
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `st == STREAM_STATUS_OK`
	 * - the fake read position of the bound standard input stream is unchanged
	 * - the buffered backing of the bound standard input stream is unchanged
	 */
	STDIO_STREAM_READ_SCENARIO_BUF_NULL_ZERO_LEN,

	/**
	 * WHEN `stream_read(s, NULL, n, &st)` is called with `n > 0`
	 * AND `args->kind == STDIO_STREAM_KIND_STDIN`
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `st == STREAM_STATUS_INVALID`
	 * - the fake read position of the bound standard input stream is unchanged
	 * - the buffered backing of the bound standard input stream is unchanged
	 */
	STDIO_STREAM_READ_SCENARIO_BUF_NULL_NONZERO,

	/**
	 * WHEN `stream_read(s, buf, n, &st)` is called with valid arguments
	 * AND `args->kind == STDIO_STREAM_KIND_STDOUT`
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `st == STREAM_STATUS_IO_ERROR`
	 * - the fake read position of the bound standard stream is unchanged
	 * - `buf` is unchanged
	 * - the buffered backing of the bound standard stream is unchanged
	 */
	STDIO_STREAM_READ_SCENARIO_STDOUT_FORBIDDEN,

	/**
	 * WHEN `stream_read(s, buf, n, &st)` is called with valid arguments
	 * AND `args->kind == STDIO_STREAM_KIND_STDERR`
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns `0`
	 * - `st == STREAM_STATUS_IO_ERROR`
	 * - the fake read position of the bound standard stream is unchanged
	 * - `buf` is unchanged
	 * - the buffered backing of the bound standard stream is unchanged
	 */
	STDIO_STREAM_READ_SCENARIO_STDERR_FORBIDDEN,

	/**
	 * WHEN `stream_read(s, buf, n, NULL)` is called with valid arguments
	 * AND `args->kind == STDIO_STREAM_KIND_STDIN`
	 * EXPECT:
	 * - returns the number of bytes actually read, up to `n`
	 * - the bytes read from the buffered backing of the bound standard input
	 *   stream are copied into `buf`
	 * - the fake read position of the bound standard input stream is advanced
	 *   by the number of bytes actually read
	 * - the buffered backing of the bound standard input stream is unchanged
	 * - no status is written
	 */
	STDIO_STREAM_READ_SCENARIO_ST_NULL_STDIN,

	/**
	 * WHEN `stream_read(s, buf, n, &st)` is called with valid arguments
	 * AND `args->kind == STDIO_STREAM_KIND_STDIN`
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns the number of bytes actually read, up to `n`
	 * - `st == STREAM_STATUS_OK`
	 * - the bytes read from the buffered backing of the bound standard input
	 *   stream are copied into `buf`
	 * - the fake read position of the bound standard input stream is advanced
	 *   by the number of bytes actually read
	 * - the buffered backing of the bound standard input stream is unchanged
	 */
	STDIO_STREAM_READ_SCENARIO_OK_STDIN,

	/**
	 * WHEN `stream_read(s, buf, n, &st)` is called with valid arguments
	 * AND `args->kind == STDIO_STREAM_KIND_STDIN`
	 * AND `n` is greater than the number of bytes currently available in the
	 *   buffered backing
	 * AND `st != NULL`
	 * EXPECT:
	 * - returns the number of bytes actually available in the buffered backing
	 * - `st == STREAM_STATUS_OK`
	 * - only the available bytes are copied into `buf`
	 * - the fake read position of the bound standard input stream is advanced
	 *   by the number of bytes actually read
	 * - the buffered backing of the bound standard input stream is unchanged
	 */
	STDIO_STREAM_READ_SCENARIO_PARTIAL_STDIN
} stdio_stream_read_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `stdio_stream_read()`.
 *
 * Notes:
 * - `kind` selects which standard stream is wrapped by the adapter instance
 *   created for the test.
 * - `buffered_backing` and `buffered_len` describe the initial fake readable
 *   backing state before the call under test.
 * - `read_len` defines the number of bytes requested through `stream_read()`.
 * - `pass_status_ptr` tells whether the call under test passes a non-NULL
 *   status pointer.
 * - `expected_ret` is the value expected from `stream_read()`.
 * - `expected_st` is asserted only when `pass_status_ptr == true`.
 * - `expected_buf` describes the bytes expected to be copied into the caller
 *   buffer on the `expected_ret` leading bytes.
 * - `expected_pos` is the expected fake read position after the call under
 *   test.
 * - `expect_buf_invariance` tells whether the test must assert that the whole
 *   caller buffer remains unchanged after the call under test.
 */
typedef struct {
	const char *name;

	/* arrange */
	stdio_stream_read_scenario_t scenario;
	stdio_stream_kind_t kind;
	uint8_t buffered_backing[FAKE_STDIO_BUF_SIZE];
	size_t buffered_len;
	size_t read_len;
	bool pass_status_ptr;

	/* assert */
	size_t expected_ret;
	stream_status_t expected_st;
	uint8_t expected_buf[FAKE_STDIO_BUF_SIZE];
	size_t expected_pos;
	bool expect_buf_invariance;
} test_stdio_stream_read_case_t;

/**
 * @brief Runtime fixture for `stdio_stream_read()` tests.
 *
 * Holds:
 * - the public stream handle under test,
 * - a snapshot of the fake buffered backing taken before the call under test,
 * - the injected adapter environment and configuration,
 * - the creation arguments selecting the bound standard stream,
 * - a status object used when the test passes a non-NULL status pointer,
 * - a pointer to the active parametric test case.
 *
 * Notes:
 * - the fake buffered backing snapshot preserves the readable state observed
 *   before the call under test.
 * - the caller buffer used by `stream_read()` is allocated inside the test
 *   body and is therefore not stored in the fixture.
 */
typedef struct {
	/* runtime resources */
	stream_t *s;
	uint8_t buffered_backing_snapshot[FAKE_STDIO_BUF_SIZE];
	size_t buffered_backing_snapshot_len;

	/* DI */
	stdio_stream_env_t env;
	stdio_stream_cfg_t cfg;

	stdio_stream_args_t args;

	/* status storage used when `st != NULL` */
	stream_status_t st;

	const test_stdio_stream_read_case_t *tc;
} test_stdio_stream_read_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for
 * `stdio_stream_read()` tests.
 */
static int setup_stdio_stream_read(void **state)
{
	const test_stdio_stream_read_case_t *tc =
		(const test_stdio_stream_read_case_t *)(*state);

	test_stdio_stream_read_fixture_t *fx =
		(test_stdio_stream_read_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_stdio_reset();
	fake_memory_reset();

	// DI
	fx->env.stdio_ops = osal_stdio_test_fake_ops();
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();

	// creation of the stream handle with stdio_stream backend
	fx->cfg.reserved = 0;
	fx->args.kind = tc->kind;
	if (stdio_stream_create_stream(&fx->s, &fx->args, &fx->cfg, &fx->env) != STREAM_STATUS_OK) {
		free(fx);
		return -1;
	}

	// seed fake stdio stream
	OSAL_STDIO *stdio = ((stdio_stream_t *)fx->s->backend)->stdio;
	fake_stdio_set_buffered_backing(
		stdio,
		tc->buffered_backing,
		tc->buffered_len
	);
	assert_int_equal(
		(int)fake_stdio_read_pos(stdio),
		0
	);

	*state = fx;
	return 0;
}

/**
 * @brief Release the `stdio_stream_read()` test fixture and verify
 * memory invariants.
 */
static int teardown_stdio_stream_read(void **state)
{
	test_stdio_stream_read_fixture_t *fx =
		(test_stdio_stream_read_fixture_t *)(*state);

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
 * @brief Execute one parametric test scenario for `stdio_stream_read()`.
 *
 * Notes:
 * - This test intentionally exercises the private backend callback through the
 *   public `stream_read()` wrapper.
 * - It assumes the unit test has visibility on internal backend/layout types.
 */
static void test_stdio_stream_read(void **state)
{
	test_stdio_stream_read_fixture_t *fx =
		(test_stdio_stream_read_fixture_t *)(*state);
	const test_stdio_stream_read_case_t *tc = fx->tc;

	OSAL_STDIO *stdio = ((stdio_stream_t *)fx->s->backend)->stdio;

	// ARRANGE
	size_t ret = 0;
	uint8_t buf[FAKE_STDIO_BUF_SIZE] = { 0 };
	void *buf_arg = buf;
	size_t n_arg = tc->read_len;
	stream_status_t st = STREAM_STATUS_OK;
	stream_status_t *st_arg = &st;

	if (tc->scenario == STDIO_STREAM_READ_SCENARIO_ZERO_LEN) {
		n_arg = 0;
	}
	if (tc->scenario == STDIO_STREAM_READ_SCENARIO_BUF_NULL_ZERO_LEN) {
		buf_arg = NULL;
		n_arg = 0;
	}
	if (tc->scenario == STDIO_STREAM_READ_SCENARIO_BUF_NULL_NONZERO) {
		buf_arg = NULL;
	}
	if (!tc->pass_status_ptr) st_arg = NULL;


	fx->buffered_backing_snapshot_len = fake_stdio_buffered_len(stdio);
	memcpy(
		fx->buffered_backing_snapshot,
		fake_stdio_buffered_backing(stdio),
		FAKE_STDIO_BUF_SIZE
	);

	// ACT
	ret = stream_read(fx->s, buf_arg, n_arg, st_arg);

	// ASSERT
	assert_int_equal(
		(int)ret,
		(int)tc->expected_ret
	);
	if (tc->pass_status_ptr) {
		assert_int_equal(
			(int)st,
			(int)tc->expected_st
		);
	}

	const uint8_t *buffered_backing = fake_stdio_buffered_backing(stdio);
	const uint8_t zeros_buf[FAKE_STDIO_BUF_SIZE] = { 0 };

	if (tc->expected_ret > 0) {
		assert_memory_equal(
			buf,
			tc->expected_buf,
			tc->expected_ret
		);
		assert_memory_equal(
			buf + tc->expected_ret,
			zeros_buf + tc->expected_ret,
			FAKE_STDIO_BUF_SIZE - tc->expected_ret
		);
	}

	assert_int_equal(
		(int)fake_stdio_read_pos(stdio),
		(int)tc->expected_pos
	);

	if (tc->expect_buf_invariance) {
		assert_memory_equal(
			buf,
			zeros_buf,
			FAKE_STDIO_BUF_SIZE
		);
	}

	// assert backing buffer invariance
	assert_int_equal(
		(int)fake_stdio_buffered_len(stdio),
		(int)fx->buffered_backing_snapshot_len
	);
	assert_memory_equal(
		buffered_backing,
		fx->buffered_backing_snapshot,
		FAKE_STDIO_BUF_SIZE
	);

}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_stdio_stream_read_case_t
CASE_STDIO_READ_READ_ZERO_LEN = {
	.name = "stdio_stream_read_zero_len",
	.scenario = STDIO_STREAM_READ_SCENARIO_ZERO_LEN,
	.kind = STDIO_STREAM_KIND_STDIN,
	.buffered_backing = { 0x01, 0x02, 0x03 },
	.buffered_len = 3,
	.read_len = 0,
	.pass_status_ptr = true,

	.expected_ret = 0,
	.expected_st = STREAM_STATUS_OK,
	.expected_buf = { 0 },
	.expected_pos = 0,
	.expect_buf_invariance = true
};

static const test_stdio_stream_read_case_t
CASE_STDIO_READ_READ_BUF_NULL_ZERO_LEN = {
	.name = "stdio_stream_read_buf_null_zero_len",
	.scenario = STDIO_STREAM_READ_SCENARIO_BUF_NULL_ZERO_LEN,
	.kind = STDIO_STREAM_KIND_STDIN,
	.buffered_backing = { 0x01, 0x02, 0x03 },
	.buffered_len = 3,
	.read_len = 0,
	.pass_status_ptr = true,

	.expected_ret = 0,
	.expected_st = STREAM_STATUS_OK,
	.expected_buf = { 0 },
	.expected_pos = 0,
	.expect_buf_invariance = true
};

static const test_stdio_stream_read_case_t
CASE_STDIO_READ_READ_BUF_NULL_NONZERO = {
	.name = "stdio_stream_read_buf_null_nonzero",
	.scenario = STDIO_STREAM_READ_SCENARIO_BUF_NULL_NONZERO,
	.kind = STDIO_STREAM_KIND_STDIN,
	.buffered_backing = { 0x01, 0x02, 0x03 },
	.buffered_len = 3,
	.read_len = 2,
	.pass_status_ptr = true,

	.expected_ret = 0,
	.expected_st = STREAM_STATUS_INVALID,
	.expected_buf = { 0 },
	.expected_pos = 0,
	.expect_buf_invariance = true
};

static const test_stdio_stream_read_case_t
CASE_STDIO_READ_READ_STDOUT_FORBIDDEN = {
	.name = "stdio_stream_read_stdout_forbidden",
	.scenario = STDIO_STREAM_READ_SCENARIO_STDOUT_FORBIDDEN,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.buffered_backing = { 0x01, 0x02, 0x03 },
	.buffered_len = 3,
	.read_len = 2,
	.pass_status_ptr = true,

	.expected_ret = 0,
	.expected_st = STREAM_STATUS_IO_ERROR,
	.expected_buf = { 0 },
	.expected_pos = 0,
	.expect_buf_invariance = true
};

static const test_stdio_stream_read_case_t
CASE_STDIO_READ_READ_STDERR_FORBIDDEN = {
	.name = "stdio_stream_read_stderr_forbidden",
	.scenario = STDIO_STREAM_READ_SCENARIO_STDERR_FORBIDDEN,
	.kind = STDIO_STREAM_KIND_STDERR,
	.buffered_backing = { 0x01, 0x02, 0x03 },
	.buffered_len = 3,
	.read_len = 2,
	.pass_status_ptr = true,

	.expected_ret = 0,
	.expected_st = STREAM_STATUS_IO_ERROR,
	.expected_buf = { 0 },
	.expected_pos = 0,
	.expect_buf_invariance = true
};

static const test_stdio_stream_read_case_t
CASE_STDIO_READ_READ_ST_NULL_STDIN = {
	.name = "stdio_stream_read_st_null_stdin",
	.scenario = STDIO_STREAM_READ_SCENARIO_ST_NULL_STDIN,
	.kind = STDIO_STREAM_KIND_STDIN,
	.buffered_backing = { 0x01, 0x02, 0x03 },
	.buffered_len = 3,
	.read_len = 2,
	.pass_status_ptr = false,

	.expected_ret = 2,
	.expected_buf = { 0x01, 0x02 },
	.expected_pos = 2,
	.expect_buf_invariance = false
};

static const test_stdio_stream_read_case_t
CASE_STDIO_READ_READ_OK_STDIN = {
	.name = "stdio_stream_read_ok_stdin",
	.scenario = STDIO_STREAM_READ_SCENARIO_OK_STDIN,
	.kind = STDIO_STREAM_KIND_STDIN,
	.buffered_backing = { 0x01, 0x02, 0x03 },
	.buffered_len = 3,
	.read_len = 2,
	.pass_status_ptr = true,

	.expected_ret = 2,
	.expected_st = STREAM_STATUS_OK,
	.expected_buf = { 0x01, 0x02 },
	.expected_pos = 2,
	.expect_buf_invariance = false
};

static const test_stdio_stream_read_case_t
CASE_STDIO_READ_READ_PARTIAL_STDIN = {
	.name = "stdio_stream_read_partial_stdin",
	.scenario = STDIO_STREAM_READ_SCENARIO_PARTIAL_STDIN,
	.kind = STDIO_STREAM_KIND_STDIN,
	.buffered_backing = { 0x01, 0x02, 0x03 },
	.buffered_len = 3,
	.read_len = 4,
	.pass_status_ptr = true,

	.expected_ret = 3,
	.expected_st = STREAM_STATUS_OK,
	.expected_buf = { 0x01, 0x02, 0x03 },
	.expected_pos = 3,
	.expect_buf_invariance = false
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STDIO_STREAM_READ_CASES(X) \
X(CASE_STDIO_READ_READ_ZERO_LEN) \
X(CASE_STDIO_READ_READ_BUF_NULL_ZERO_LEN) \
X(CASE_STDIO_READ_READ_BUF_NULL_NONZERO) \
X(CASE_STDIO_READ_READ_STDOUT_FORBIDDEN) \
X(CASE_STDIO_READ_READ_STDERR_FORBIDDEN) \
X(CASE_STDIO_READ_READ_ST_NULL_STDIN) \
X(CASE_STDIO_READ_READ_OK_STDIN) \
X(CASE_STDIO_READ_READ_PARTIAL_STDIN)

#define STDIO_STREAM_MAKE_READ_TEST(case_sym) \
LEXLEO_MAKE_TEST(stdio_stream_read, case_sym)

static const struct CMUnitTest stdio_stream_read_tests[] = {
	STDIO_STREAM_READ_CASES(STDIO_STREAM_MAKE_READ_TEST)
};

#undef STDIO_STREAM_READ_CASES
#undef STDIO_STREAM_MAKE_READ_TEST

/** @endcond */

/**
 * @brief Scenarios for `stdio_stream_flush()`.
 *
 * static stream_status_t stdio_stream_flush(
 *     void *backend );
 *
 * This private callback is exercised through the public `stream_flush()` API
 * on streams created by `stdio_stream_create_stream()`.
 *
 * Invalid arguments:
 * - `backend == NULL`
 *
 * Success:
 * - Returns `STREAM_STATUS_OK`.
 * - Transfers buffered data from the wrapped standard stream to its sink
 *   backing when that stream supports flushing.
 *
 * Failure:
 * - Returns `STREAM_STATUS_INVALID` for invalid arguments.
 * - Returns `STREAM_STATUS_IO_ERROR` when the bound standard stream does not
 *   support flushing.
 * - Leaves the fake buffered state unchanged on failure paths covered here.
 *
 * Doubles:
 * - fake_stdio
 * - fake_memory
 *
 * Fake behavior:
 * - fake_stdio models standard streams with an explicit internal buffer
 *   (`buffered_backing`) and a flushed output (`sink_backing`)
 * - write operations append to the buffered backing
 * - flush operations transfer as many buffered bytes as possible to the sink
 *   backing
 * - transferred bytes are removed from the buffered backing
 * - if the sink backing has insufficient remaining capacity, unflushed bytes
 *   remain buffered
 * - no implicit flush occurs outside the explicit flush operation
 *
 * Isolation:
 * - the callback is not called directly
 * - behavior is exercised through the public `stream` port wrapper
 *
 * See also:
 * - @ref testing_foundation_stdio_stream_unit_flush
 *   "stdio_stream_flush() unit tests section"
 * - @ref specifications_stdio_stream_flush
 *   "stdio_stream_flush() specifications"
 * - @ref specifications_stream_flush
 *   "stream_flush() specifications"
 *
 * The scenarios below define the test oracle for `stdio_stream_flush()`.
 */
typedef enum {
	/**
	 * WHEN `stream_flush(s)` is called with valid arguments
	 * AND `args->kind == STDIO_STREAM_KIND_STDIN`
	 * EXPECT:
	 * - returns `STREAM_STATUS_IO_ERROR`
	 * - the buffered backing of the bound standard stream is unchanged
	 * - the sink backing of the bound standard stream is unchanged
	 */
	STDIO_STREAM_FLUSH_SCENARIO_STDIN_FORBIDDEN,

	/**
	 * WHEN `stream_flush(s)` is called with valid arguments
	 * AND `args->kind == STDIO_STREAM_KIND_STDOUT`
	 * EXPECT:
	 * - returns `STREAM_STATUS_OK`
	 * - buffered data from the bound standard output stream is transferred
	 *   to its sink backing according to the fake flush behavior
	 */
	STDIO_STREAM_FLUSH_SCENARIO_OK_STDOUT,

	/**
	 * WHEN `stream_flush(s)` is called with valid arguments
	 * AND `args->kind == STDIO_STREAM_KIND_STDERR`
	 * EXPECT:
	 * - returns `STREAM_STATUS_OK`
	 * - buffered data from the bound standard error stream is transferred
	 *   to its sink backing according to the fake flush behavior
	 */
	STDIO_STREAM_FLUSH_SCENARIO_OK_STDERR,
} stdio_stream_flush_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `stdio_stream_flush()`.
 *
 * Notes:
 * - `kind` selects which standard stream is wrapped by the adapter instance
 *   created for the test.
 * - `initial_buffered_backing` and `initial_buffered_len` describe the
 *   initial fake buffered backing state before the call under test.
 * - `initial_sink_backing` and `initial_sink_len` describe the initial fake
 *   sink backing state before the call under test.
 * - `expected_status` is the status expected from `stream_flush()`.
 * - `expected_buffered_backing` and `expected_buffered_len` describe the
 *   expected final state of the fake buffered backing after the call under
 *   test.
 * - `expected_sink_backing` and `expected_sink_len` describe the expected
 *   final state of the fake sink backing after the call under test.
 * - `expect_buffered_invariance` tells whether the test must assert that the
 *   whole fake buffered backing remains unchanged after the call under test.
 * - `expect_sink_invariance` tells whether the test must assert that the
 *   whole fake sink backing remains unchanged after the call under test.
 */
typedef struct {
	const char *name;

	/* arrange */
	stdio_stream_flush_scenario_t scenario;
	stdio_stream_kind_t kind;
	uint8_t initial_buffered_backing[FAKE_STDIO_BUF_SIZE];
	size_t initial_buffered_len;
	uint8_t initial_sink_backing[FAKE_STDIO_BUF_SIZE];
	size_t initial_sink_len;

	/* assert */
	stream_status_t expected_status;
	uint8_t expected_buffered_backing[FAKE_STDIO_BUF_SIZE];
	size_t expected_buffered_len;
	uint8_t expected_sink_backing[FAKE_STDIO_BUF_SIZE];
	size_t expected_sink_len;
	bool expect_buffered_invariance;
	bool expect_sink_invariance;
} test_stdio_stream_flush_case_t;

/**
 * @brief Runtime fixture for `stdio_stream_flush()` tests.
 *
 * Holds:
 * - the public stream handle under test,
 * - a snapshot of the fake buffered backing taken before the call under test,
 * - a snapshot of the fake sink backing taken before the call under test,
 * - the injected adapter environment and configuration,
 * - the creation arguments selecting the bound standard stream,
 * - a pointer to the active parametric test case.
 *
 * Notes:
 * - the buffered backing snapshot is used to preserve the fake state observed
 *   before the call under test when needed by the test logic.
 * - the sink backing snapshot is used to preserve the fake flushed-output
 *   state observed before the call under test when needed by the test logic.
 */
typedef struct {
	/* runtime resources */
	stream_t *s;
	uint8_t buffered_backing_snapshot[FAKE_STDIO_BUF_SIZE];
	size_t buffered_backing_snapshot_len;
	uint8_t sink_backing_snapshot[FAKE_STDIO_BUF_SIZE];
	size_t sink_backing_snapshot_len;

	/* DI */
	stdio_stream_env_t env;
	stdio_stream_cfg_t cfg;

	stdio_stream_args_t args;

	const test_stdio_stream_flush_case_t *tc;
} test_stdio_stream_flush_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for
 * `stdio_stream_flush()` tests.
 */
static int setup_stdio_stream_flush(void **state)
{
	const test_stdio_stream_flush_case_t *tc =
		(const test_stdio_stream_flush_case_t *)(*state);

	test_stdio_stream_flush_fixture_t *fx =
		(test_stdio_stream_flush_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_stdio_reset();
	fake_memory_reset();

	// DI
	fx->env.stdio_ops = osal_stdio_test_fake_ops();
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();

	// creation of the stream handle with stdio_stream backend
	fx->cfg.reserved = 0;
	fx->args.kind = tc->kind;
	if (stdio_stream_create_stream(&fx->s, &fx->args, &fx->cfg, &fx->env) != STREAM_STATUS_OK) {
		free(fx);
		return -1;
	}

	// seed fake stdio stream
	OSAL_STDIO *stdio = ((stdio_stream_t *)fx->s->backend)->stdio;
	fake_stdio_set_buffered_backing(
		stdio,
		tc->initial_buffered_backing,
		tc->initial_buffered_len
	);
	fake_stdio_set_sink_backing(
		stdio,
		tc->initial_sink_backing,
		tc->initial_sink_len
	);

	*state = fx;
	return 0;
}

/**
 * @brief Release the `stdio_stream_flush()` test fixture and verify
 * memory invariants.
 */
static int teardown_stdio_stream_flush(void **state)
{
	test_stdio_stream_flush_fixture_t *fx =
		(test_stdio_stream_flush_fixture_t *)(*state);

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
 * @brief Execute one parametric test scenario for `stdio_stream_flush()`.
 *
 * Notes:
 * - This test intentionally exercises the private backend callback through the
 *   public `stream_flush()` wrapper.
 * - It assumes the unit test has visibility on internal backend/layout types.
 */
static void test_stdio_stream_flush(void **state)
{
	test_stdio_stream_flush_fixture_t *fx =
		(test_stdio_stream_flush_fixture_t *)(*state);
	const test_stdio_stream_flush_case_t *tc = fx->tc;

	OSAL_STDIO *stdio = ((stdio_stream_t *)fx->s->backend)->stdio;

	// ARRANGE
	stream_status_t status = STREAM_STATUS_OK;

	fx->buffered_backing_snapshot_len = fake_stdio_buffered_len(stdio);
	memcpy(
		fx->buffered_backing_snapshot,
		fake_stdio_buffered_backing(stdio),
		FAKE_STDIO_BUF_SIZE
	);

	fx->sink_backing_snapshot_len = fake_stdio_sink_len(stdio);
	memcpy(
		fx->sink_backing_snapshot,
		fake_stdio_sink_backing(stdio),
		FAKE_STDIO_BUF_SIZE
	);

	// ACT
	status = stream_flush(fx->s);

	// ASSERT
	assert_int_equal(
		(int)status,
		(int)tc->expected_status
	);

	const uint8_t *buffered_backing = fake_stdio_buffered_backing(stdio);
	const uint8_t *sink_backing = fake_stdio_sink_backing(stdio);

	assert_int_equal(
		(int)fake_stdio_buffered_len(stdio),
		(int)tc->expected_buffered_len
	);
	assert_memory_equal(
		buffered_backing,
		tc->expected_buffered_backing,
		FAKE_STDIO_BUF_SIZE
	);

	assert_int_equal(
		(int)fake_stdio_sink_len(stdio),
		(int)tc->expected_sink_len
	);
	assert_memory_equal(
		sink_backing,
		tc->expected_sink_backing,
		FAKE_STDIO_BUF_SIZE
	);

	if (tc->expect_buffered_invariance) {
		assert_int_equal(
			(int)fake_stdio_buffered_len(stdio),
			(int)fx->buffered_backing_snapshot_len
		);
		assert_memory_equal(
			buffered_backing,
			fx->buffered_backing_snapshot,
			FAKE_STDIO_BUF_SIZE
		);
	}

	if (tc->expect_sink_invariance) {
		assert_int_equal(
			(int)fake_stdio_sink_len(stdio),
			(int)fx->sink_backing_snapshot_len
		);
		assert_memory_equal(
			sink_backing,
			fx->sink_backing_snapshot,
			FAKE_STDIO_BUF_SIZE
		);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_stdio_stream_flush_case_t
CASE_STDIO_STREAM_FLUSH_STDIN_FORBIDDEN = {
	.name = "stdio_stream_flush_stdin_forbidden",
	.scenario = STDIO_STREAM_FLUSH_SCENARIO_STDIN_FORBIDDEN,
	.kind = STDIO_STREAM_KIND_STDIN,
	.initial_buffered_backing = { 0x01, 0x02, 0x03 },
	.initial_buffered_len = 3,
	.initial_sink_backing = { 0x0a, 0x0b },
	.initial_sink_len = 2,

	.expected_status = STREAM_STATUS_IO_ERROR,
	.expected_buffered_backing = { 0x01, 0x02, 0x03 },
	.expected_buffered_len = 3,
	.expected_sink_backing = { 0x0a, 0x0b },
	.expected_sink_len = 2,
	.expect_buffered_invariance = true,
	.expect_sink_invariance = true
};

static const test_stdio_stream_flush_case_t
CASE_STDIO_STREAM_FLUSH_OK_STDOUT = {
	.name = "stdio_stream_flush_ok_stdout",
	.scenario = STDIO_STREAM_FLUSH_SCENARIO_OK_STDOUT,
	.kind = STDIO_STREAM_KIND_STDOUT,
	.initial_buffered_backing = { 0x01, 0x02, 0x03 },
	.initial_buffered_len = 3,
	.initial_sink_backing = { 0x0a, 0x0b },
	.initial_sink_len = 2,

	.expected_status = STREAM_STATUS_OK,
	.expected_buffered_backing = { 0 },
	.expected_buffered_len = 0,
	.expected_sink_backing = { 0x0a, 0x0b, 0x01, 0x02, 0x03 },
	.expected_sink_len = 5,
	.expect_buffered_invariance = false,
	.expect_sink_invariance = false
};

static const test_stdio_stream_flush_case_t
CASE_STDIO_STREAM_FLUSH_OK_STDERR = {
	.name = "stdio_stream_flush_ok_stderr",
	.scenario = STDIO_STREAM_FLUSH_SCENARIO_OK_STDERR,
	.kind = STDIO_STREAM_KIND_STDERR,
	.initial_buffered_backing = { 0x01, 0x02, 0x03 },
	.initial_buffered_len = 3,
	.initial_sink_backing = { 0x0a, 0x0b },
	.initial_sink_len = 2,

	.expected_status = STREAM_STATUS_OK,
	.expected_buffered_backing = { 0 },
	.expected_buffered_len = 0,
	.expected_sink_backing = { 0x0a, 0x0b, 0x01, 0x02, 0x03 },
	.expected_sink_len = 5,
	.expect_buffered_invariance = false,
	.expect_sink_invariance = false
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STDIO_STREAM_FLUSH_CASES(X) \
X(CASE_STDIO_STREAM_FLUSH_STDIN_FORBIDDEN) \
X(CASE_STDIO_STREAM_FLUSH_OK_STDOUT) \
X(CASE_STDIO_STREAM_FLUSH_OK_STDERR)

#define STDIO_STREAM_MAKE_FLUSH_TEST(case_sym) \
LEXLEO_MAKE_TEST(stdio_stream_flush, case_sym)

static const struct CMUnitTest stdio_stream_flush_tests[] = {
	STDIO_STREAM_FLUSH_CASES(STDIO_STREAM_MAKE_FLUSH_TEST)
};

#undef STDIO_STREAM_FLUSH_CASES
#undef STDIO_STREAM_MAKE_FLUSH_TEST

/** @endcond */

/**
 * @brief Scenarios for `stdio_stream_close()`.
 *
 * static stream_status_t stdio_stream_close(
 *     void *backend );
 *
 * This private callback is exercised through the public `stream_destroy()` API
 * on streams created by `stdio_stream_create_stream()`.
 *
 * Invalid arguments:
 * - `backend == NULL`
 *
 * Success:
 * - Returns `STREAM_STATUS_OK`.
 * - Releases the `stdio_stream` backend container through its injected memory
 *   operations.
 *
 * Failure:
 * - Returns `STREAM_STATUS_INVALID` for invalid arguments.
 *
 * Doubles:
 * - fake_memory
 *
 * Fake behavior:
 * - the `stdio_stream` backend wraps a borrowed standard stream
 * - the underlying standard stream is not owned by the backend container
 * - close releases only the backend container
 * - close does not close the underlying standard stream itself
 *
 * Isolation:
 * - the callback is not called directly
 * - behavior is exercised through the public `stream` port wrapper
 *
 * See also:
 * - @ref testing_foundation_stdio_stream_unit_close
 *   "stdio_stream_close() unit tests section"
 * - @ref specifications_stdio_stream_close
 *   "stdio_stream_close() specifications"
 * - @ref specifications_stream_destroy
 *   "stream_destroy() specifications"
 *
 * The scenarios below define the test oracle for `stdio_stream_close()`.
 */
typedef enum {
	/**
	 * WHEN `stream_destroy(&s)` is called with a valid stream handle
	 * AND `args->kind == STDIO_STREAM_KIND_STDIN`
	 * EXPECT:
	 * - the backend container bound to the stream is released
	 * - the stream handle is destroyed
	 * - the caller-visible handle is set to `NULL`
	 */
	STDIO_STREAM_CLOSE_SCENARIO_OK_STDIN = 0,

	/**
	 * WHEN `stream_destroy(&s)` is called with a valid stream handle
	 * AND `args->kind == STDIO_STREAM_KIND_STDOUT`
	 * EXPECT:
	 * - the backend container bound to the stream is released
	 * - the stream handle is destroyed
	 * - the caller-visible handle is set to `NULL`
	 */
	STDIO_STREAM_CLOSE_SCENARIO_OK_STDOUT,

	/**
	 * WHEN `stream_destroy(&s)` is called with a valid stream handle
	 * AND `args->kind == STDIO_STREAM_KIND_STDERR`
	 * EXPECT:
	 * - the backend container bound to the stream is released
	 * - the stream handle is destroyed
	 * - the caller-visible handle is set to `NULL`
	 */
	STDIO_STREAM_CLOSE_SCENARIO_OK_STDERR,
} stdio_stream_close_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `stdio_stream_close()`.
 *
 * Notes:
 * - `kind` selects which standard stream is wrapped by the adapter instance
 *   created for the test.
 * - `expected_handle_is_null` tells whether the caller-visible stream handle
 *   is expected to be set to `NULL` after the call under test.
 * - `expect_no_leak` tells whether the test must assert that the close path
 *   releases all memory owned by the stream handle and backend container.
 */
typedef struct {
	const char *name;

	/* arrange */
	stdio_stream_close_scenario_t scenario;
	stdio_stream_kind_t kind;

	/* assert */
	bool expected_handle_is_null;
	bool expect_no_leak;
} test_stdio_stream_close_case_t;

/**
 * @brief Runtime fixture for `stdio_stream_close()` tests.
 *
 * Holds:
 * - the public stream handle under test,
 * - the injected adapter environment and configuration,
 * - the creation arguments selecting the bound standard stream,
 * - a pointer to the active parametric test case.
 *
 * Notes:
 * - the stream handle is created through `stdio_stream_create_stream()`
 *   before the call under test.
 * - the call under test is exercised through the public `stream_destroy()`
 *   wrapper.
 */
typedef struct {
	/* runtime resources */
	stream_t *s;

	/* DI */
	stdio_stream_env_t env;
	stdio_stream_cfg_t cfg;

	stdio_stream_args_t args;

	const test_stdio_stream_close_case_t *tc;
} test_stdio_stream_close_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for
 * `stdio_stream_close()` tests.
 */
static int setup_stdio_stream_close(void **state)
{
	const test_stdio_stream_close_case_t *tc =
		(const test_stdio_stream_close_case_t *)(*state);

	test_stdio_stream_close_fixture_t *fx =
		(test_stdio_stream_close_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_stdio_reset();
	fake_memory_reset();

	// DI
	fx->env.stdio_ops = osal_stdio_test_fake_ops();
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.port_env.mem = osal_mem_test_fake_ops();

	// creation of the stream handle with stdio_stream backend
	fx->cfg.reserved = 0;
	fx->args.kind = tc->kind;
	if (stdio_stream_create_stream(&fx->s, &fx->args, &fx->cfg, &fx->env) != STREAM_STATUS_OK) {
		free(fx);
		return -1;
	}

	*state = fx;
	return 0;
}

/**
 * @brief Release the `stdio_stream_close()` test fixture and verify
 * memory invariants.
 */
static int teardown_stdio_stream_close(void **state)
{
	test_stdio_stream_close_fixture_t *fx =
		(test_stdio_stream_close_fixture_t *)(*state);

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
 * @brief Execute one parametric test scenario for `stdio_stream_close()`.
 *
 * Notes:
 * - This test intentionally exercises the private backend callback through the
 *   public `stream_destroy()` wrapper.
 * - It assumes the unit test has visibility on internal backend/layout types.
 */
static void test_stdio_stream_close(void **state)
{
	test_stdio_stream_close_fixture_t *fx =
		(test_stdio_stream_close_fixture_t *)(*state);
	const test_stdio_stream_close_case_t *tc = fx->tc;

	// ARRANGE
	stream_t *s_before = fx->s;

	// ACT
	stream_destroy(&fx->s);

	// ASSERT
	(void)s_before;

	assert_int_equal(
		(int)(fx->s == NULL),
		(int)tc->expected_handle_is_null
	);
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_stdio_stream_close_case_t
CASE_STDIO_STREAM_CLOSE_OK_STDIN = {
	.name = "stdio_stream_close_ok_stdin",
	.scenario = STDIO_STREAM_CLOSE_SCENARIO_OK_STDIN,
	.kind = STDIO_STREAM_KIND_STDIN,

	.expected_handle_is_null = true,
	.expect_no_leak = true
};

static const test_stdio_stream_close_case_t
CASE_STDIO_STREAM_CLOSE_OK_STDOUT = {
	.name = "stdio_stream_close_ok_stdout",
	.scenario = STDIO_STREAM_CLOSE_SCENARIO_OK_STDOUT,
	.kind = STDIO_STREAM_KIND_STDOUT,

	.expected_handle_is_null = true,
	.expect_no_leak = true
};

static const test_stdio_stream_close_case_t
CASE_STDIO_STREAM_CLOSE_OK_STDERR = {
	.name = "stdio_stream_close_ok_stderr",
	.scenario = STDIO_STREAM_CLOSE_SCENARIO_OK_STDERR,
	.kind = STDIO_STREAM_KIND_STDERR,

	.expected_handle_is_null = true,
	.expect_no_leak = true
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STDIO_STREAM_CLOSE_CASES(X) \
X(CASE_STDIO_STREAM_CLOSE_OK_STDIN) \
X(CASE_STDIO_STREAM_CLOSE_OK_STDOUT) \
X(CASE_STDIO_STREAM_CLOSE_OK_STDERR)

#define STDIO_STREAM_MAKE_CLOSE_TEST(case_sym) \
LEXLEO_MAKE_TEST(stdio_stream_close, case_sym)

static const struct CMUnitTest stdio_stream_close_tests[] = {
	STDIO_STREAM_CLOSE_CASES(STDIO_STREAM_MAKE_CLOSE_TEST)
};

#undef STDIO_STREAM_CLOSE_CASES
#undef STDIO_STREAM_MAKE_CLOSE_TEST

/** @endcond */

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	static const struct CMUnitTest stdio_stream_non_parametric_tests[] = {
		cmocka_unit_test(test_stdio_stream_default_cfg),
		cmocka_unit_test(test_stdio_stream_default_env)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(stdio_stream_non_parametric_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stdio_stream_create_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(create_desc_stdio_stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stdio_stream_ctor_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stdio_stream_write_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stdio_stream_read_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stdio_stream_flush_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stdio_stream_close_tests, NULL, NULL);

	return failed;
}

/** @endcond */
