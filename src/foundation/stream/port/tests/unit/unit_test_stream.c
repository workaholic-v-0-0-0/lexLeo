/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_stream.c
 * @ingroup stream_unit_tests
 * @brief Unit tests implementation for the stream port.
 *
 * @details
 * This file implements the unit-level validation of the stream port contracts.
 *
 * Covered surfaces:
 * - CR helpers: `stream_default_ops()`, `stream_default_env()`
 * - lifecycle: `stream_create()`, `stream_destroy()`
 * - borrower API: `stream_read()`, `stream_write()`, `stream_flush()`
 *
 * Test strategy:
 * - parametric scenario-based testing
 * - explicit validation of argument checking and forwarding behavior
 * - allocator fault injection through `fake_memory`
 * - spy/fake backend verification through `fake_stream_backend_t`
 *
 * See also:
 * - @ref testing_foundation_stream_unit "stream unit tests page"
 * - @ref specifications_stream "stream specifications"
 */

#include "stream/borrowers/stream.h"

#include "stream/lifecycle/stream_lifecycle.h"
#include "stream/adapters/stream_adapters_api.h"
#include "stream/cr/stream_cr_api.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/** @cond INTERNAL */

//-----------------------------------------------------------------------------
// LOCAL TEST DOUBLES
//-----------------------------------------------------------------------------

/**
 * @brief Spy/fake backend used exclusively by stream port unit tests.
 *
 * @details
 * This test double plays two roles:
 * - spy: records calls and captured arguments
 * - fake: exposes programmable return values for backend operations
 *
 * It is used to verify:
 * - correct forwarding to the backend vtable
 * - absence of unexpected backend calls
 * - propagation of backend statuses and return values
 *
 * This helper is local to this test translation unit.
 */
typedef struct fake_stream_backend_t {
    /** call counters */
    int read_called;
    int write_called;
    int flush_called;
    int close_called;

    /** last arguments */
    void *last_read_buf;
    const void *last_write_buf;
    size_t last_read_n;
    size_t last_write_n;
	void *last_backend;
    stream_status_t *last_read_st;
    stream_status_t *last_write_st;

	/** configurable behavior */
	stream_status_t read_st_to_set;
	stream_status_t write_st_to_set;
    size_t read_ret;
    size_t write_ret;
    stream_status_t flush_ret;
    stream_status_t close_ret;
} fake_stream_backend_t;

/**
 * @brief Reset the fake stream backend to its default spy state.
 *
 * @param[in,out] b Backend spy/fake to reset. Must not be NULL.
 */
static void fake_stream_backend_reset(fake_stream_backend_t *b) {
	assert_non_null(b);
	b->read_called = 0;
    b->write_called = 0;
    b->flush_called = 0;
    b->close_called = 0;

    // last arguments
    b->last_read_buf = NULL;
    b->last_write_buf = NULL;
    b->last_read_n = (size_t)-1;
    b->last_write_n = (size_t)-1;
	b->last_backend = NULL;
    b->last_read_st = NULL;
    b->last_write_st = NULL;

    // configurable behavior
	b->read_st_to_set = STREAM_STATUS_INVALID;
	b->write_st_to_set = STREAM_STATUS_INVALID;
    b->read_ret = (size_t)-1;
    b->write_ret = (size_t)-1;
    b->flush_ret = STREAM_STATUS_INVALID;
    b->close_ret = STREAM_STATUS_OK;
}

/**
 * @brief Fake/spy implementation of the backend read operation.
 *
 * @details
 * Records the call and captured arguments into `fake_stream_backend_t`,
 * then returns the programmable result configured in the spy/fake state.
 *
 * If `st` is non-NULL, the function stores `b->read_st_to_set` into `*st`.
 */
static size_t fake_stream_read(
    void *backend,
    void *buf,
    size_t n,
    stream_status_t *st)
{
    fake_stream_backend_t *b = (fake_stream_backend_t *)backend;
    assert_non_null(b);

    b->read_called++;

	b->last_backend = backend;
    b->last_read_buf = buf;
    b->last_read_n = n;
	b->last_read_st = st;

    if (st) *st = b->read_st_to_set;
    return b->read_ret;
}

/**
 * @brief Fake/spy implementation of the backend write operation.
 *
 * @details
 * Records the call and captured arguments into `fake_stream_backend_t`,
 * then returns the programmable result configured in the spy/fake state.
 *
 * If `st` is non-NULL, the function stores `b->write_st_to_set` into `*st`.
 */
static size_t fake_stream_write(
    void *backend,
    const void *buf,
    size_t n,
    stream_status_t *st)
{
    fake_stream_backend_t *b = (fake_stream_backend_t *)backend;
    assert_non_null(b);

    b->write_called++;

	b->last_backend = backend;
    b->last_write_buf = buf;
    b->last_write_n = n;
	b->last_write_st = st;

    if (st) *st = b->write_st_to_set;
    return b->write_ret;
}

/**
 * @brief Fake/spy implementation of the backend flush operation.
 *
 * @details
 * Records the call into `fake_stream_backend_t` and returns the
 * programmable flush status configured in the spy/fake state.
 */
static stream_status_t fake_stream_flush(void *backend)
{
    fake_stream_backend_t *b = (fake_stream_backend_t *)backend;
    assert_non_null(b);

    b->flush_called++;

	b->last_backend = backend;

    return b->flush_ret;
}

/**
 * @brief Fake/spy implementation of the backend close operation.
 *
 * @details
 * Records the call into `fake_stream_backend_t` and returns the
 * programmable close status configured in the spy/fake state.
 *
 * If `backend` is NULL, the function returns `STREAM_STATUS_NO_BACKEND`
 * without recording any call.
 */
static stream_status_t fake_stream_close(void *backend)
{
	if (!backend) return STREAM_STATUS_NO_BACKEND;
	fake_stream_backend_t *b = (fake_stream_backend_t *)backend;

    b->close_called++;

	b->last_backend = backend;

    return b->close_ret;
}

/**
 * @brief Spy/fake backend vtable used by stream port unit tests.
 *
 * @details
 * Binds the local fake backend operations to a `stream_vtbl_t`
 * so that test-created `stream_t` instances route borrower calls
 * through `fake_stream_backend_t`.
 */
static const stream_vtbl_t fake_stream_vtbl = {
    .read = fake_stream_read,
    .write = fake_stream_write,
    .flush = fake_stream_flush,
    .close = fake_stream_close
};

/** @endcond */

/**
 * @brief Test `stream_default_ops()`.
 *
 * const stream_ops_t *stream_default_ops(void);
 *
 * Success:
 * - `ret`, `ret->read`, `ret->write` and `ret->flush` are non-`NULL`.
 *
 * Failure:
 * - None.
 *
 * See also:
 * - @ref testing_foundation_stream_unit_stream_default_ops "stream_default_ops() unit-tests section"
 * - @ref specifications_stream_default_ops "stream_default_ops() specifications"
 */
static void test_stream_default_ops(void **state) {
	(void)state;
	const stream_ops_t *ret = stream_default_ops();
	assert_non_null(ret);
	assert_non_null(ret->read);
	assert_non_null(ret->write);
	assert_non_null(ret->flush);
}

/**
 * @brief Test `stream_default_env()`.
 *
 * stream_env_t stream_default_env(const osal_mem_ops_t *mem_ops);
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
 * - @ref testing_foundation_stream_unit_stream_default_env "stream_default_env() unit tests section"
 * - @ref specifications_stream_default_env "stream_default_env() specifications".
 */
static void test_stream_default_env(void **state) {
	(void)state;

	const osal_mem_ops_t dummy = {0};
	const osal_mem_ops_t *dummy_p = &dummy;

	stream_env_t ret = stream_default_env(dummy_p);

	assert_ptr_equal(ret.mem, dummy_p);
}

/**
 * @brief Scenarios for `stream_create()` / `stream_destroy()`.
 *
 * stream_status_t stream_create(
 *	   stream_t **out,
 *	   const stream_vtbl_t *vtbl,
 *	   void *backend,
 *	   const stream_env_t *env );
 *
 * void stream_destroy(stream_t **s);
 *
 * Invalid arguments:
 * - `out`, `vtbl`, `env` must not be NULL.
 * - `vtbl->read`, `vtbl->write`, `vtbl->flush`, `vtbl->close` must not be NULL.
 * - `env->mem` must not be NULL.
 *
 * Success:
 * - Returns STREAM_STATUS_OK.
 * - Stores a valid stream in `*out`.
 * - The produced stream must be destroyed via `stream_destroy()`.
 *
 * Failure:
 * - Returns:
 *     - STREAM_STATUS_INVALID for invalid arguments
 *     - STREAM_STATUS_OOM on allocation failure
 * - Leaves `*out` unchanged if `out` is not NULL.
 *
 * Lifecycle:
 * - `stream_destroy()` does nothing if `s` is NULL or `*s` is NULL.
 * - Otherwise, it releases the stream object and sets `*s` to NULL.
 *
 * Doubles:
 * - fake_memory
 *
 * See also:
 * - @ref testing_foundation_stream_unit_stream_create_stream_destroy "stream_create() / stream_destroy() unit tests section"
 * - @ref specifications_stream_create "stream_create() specifications"
 * - @ref specifications_stream_destroy "stream_destroy() specifications"
 *
 * The scenarios below define the test oracle for `stream_create()` and `stream_destroy()`.
 */
typedef enum {
    /**
     * WHEN `stream_create(out, vtbl, backend, env)` is called with valid arguments
     * EXPECT:
     * - returns `STREAM_STATUS_OK`
     * - stores a non-NULL stream handle in `*out`
     * - the produced handle is eligible for destruction by `stream_destroy()`
     */
    STREAM_LIFECYCLE_SCENARIO_OK = 0,

    /**
     * WHEN `out == NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     * - no stream handle is produced
     */
    STREAM_LIFECYCLE_SCENARIO_OUT_NULL,

    /**
     * WHEN `vtbl == NULL` and `out != NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     * - leaves `*out` unchanged
     */
    STREAM_LIFECYCLE_SCENARIO_VTBL_NULL,

    /**
     * WHEN `env == NULL` and `out != NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     * - leaves `*out` unchanged
     */
    STREAM_LIFECYCLE_SCENARIO_ENV_NULL,

    /**
     * WHEN `env != NULL` but `env->mem == NULL` and `out != NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     * - leaves `*out` unchanged
     */
    STREAM_LIFECYCLE_SCENARIO_ENV_MEM_NULL,

    /**
     * WHEN allocation of the stream handle fails (allocator reports OOM)
     * EXPECT:
     * - returns `STREAM_STATUS_OOM`
     * - leaves `*out` unchanged
     *
     * Notes:
     * - This scenario is exercised by configuring `fake_memory` to fail the
     *   allocation performed by `stream_create()`.
     */
    STREAM_LIFECYCLE_SCENARIO_OOM,

    /**
     * WHEN `stream_create()` succeeds and `stream_destroy()` is called twice
     * EXPECT:
     * - first `stream_destroy(&s)` releases the handle and sets `s` to NULL
     * - second `stream_destroy(&s)` is a no-op and keeps `s` as NULL
     *
     * Notes:
     * - This scenario checks the idempotence guarantee of `stream_destroy()`.
     */
    STREAM_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT,

    /**
     * WHEN `vtbl != NULL` but `vtbl->read == NULL` and `out != NULL`
     * EXPECT:
     * - returns `STREAM_STATUS_INVALID`
     * - leaves `*out` unchanged
     *
     * Notes:
     * - The stream port requires a well-formed vtbl at creation time.
     */
    STREAM_LIFECYCLE_SCENARIO_VTBL_READ_NULL,
} stream_lifecycle_scenario_t;

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
 * @brief One parametric test case for the stream lifecycle contract.
 *
 * Notes:
 * - `fail_call_idx` is used by `fake_memory` to inject an allocation failure
 *   on a specific call number (used by the OOM scenario).
 */
typedef struct {
	const char *name;

	stream_lifecycle_scenario_t scenario;
	size_t fail_call_idx;

	stream_status_t expected_ret;
	out_expect_t out_expect;
} test_stream_lifecycle_case_t;

/**
 * @brief Runtime fixture for `stream_create()` / `stream_destroy()` tests.
 *
 * Holds:
 * - the stream handle under test,
 * - the injected environment,
 * - the fake adapter backend,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	stream_t *out;

	// injection
	stream_env_t env;

	fake_stream_backend_t backend;

	const test_stream_lifecycle_case_t *tc;
} test_stream_lifecycle_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for lifecycle tests.
 */
static int setup_stream_lifecycle(void **state)
{
	const test_stream_lifecycle_case_t *tc =
		(const test_stream_lifecycle_case_t *)(*state);

	test_stream_lifecycle_fixture_t *fx =
		(test_stream_lifecycle_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();
	if (tc->scenario == STREAM_LIFECYCLE_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	// DI
	fx->env.mem = osal_mem_test_fake_ops();

	osal_memset(&fx->backend, 0, sizeof(fx->backend));
    fx->backend.close_ret = STREAM_STATUS_OK;

	*state = fx;
	return 0;
}

/**
 * @brief Release the lifecycle test fixture and verify memory invariants.
 */
static int teardown_stream_lifecycle(void **state)
{
	test_stream_lifecycle_fixture_t *fx =
		(test_stream_lifecycle_fixture_t *)(*state);

	if (fx->out) {
		stream_destroy(&fx->out);
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
 * @brief Execute one parametric test scenario for `stream_create()` / `stream_destroy()`.
 */
static void test_stream_lifecycle(void **state)
{
	test_stream_lifecycle_fixture_t *fx =
		(test_stream_lifecycle_fixture_t *)(*state);
	const test_stream_lifecycle_case_t *tc = fx->tc;

	// ARRANGE
	stream_status_t ret = STREAM_STATUS_INVALID;

	stream_t **out_arg = &fx->out;
	const stream_vtbl_t *vtbl_arg = &fake_stream_vtbl;
	stream_vtbl_t vtbl_local;
	void *backend_arg = &fx->backend;
	const stream_env_t *env_arg = &fx->env;

	// invalid args
	if (tc->scenario == STREAM_LIFECYCLE_SCENARIO_OUT_NULL) out_arg = NULL;
	if (tc->scenario == STREAM_LIFECYCLE_SCENARIO_VTBL_NULL) vtbl_arg = NULL;
	if (tc->scenario == STREAM_LIFECYCLE_SCENARIO_ENV_NULL) env_arg = NULL;
	if (tc->scenario == STREAM_LIFECYCLE_SCENARIO_ENV_MEM_NULL) fx->env.mem = NULL;
	if (tc->scenario == STREAM_LIFECYCLE_SCENARIO_VTBL_READ_NULL) {
    	vtbl_local = fake_stream_vtbl;
    	vtbl_local.read = NULL;
		vtbl_arg = &vtbl_local;
	}

    // ensure OUT_EXPECT_UNCHANGED is meaningful
    if (tc->out_expect == OUT_EXPECT_UNCHANGED && out_arg != NULL) {
        fx->out = (stream_t *)(uintptr_t)0xDEADC0DEu; // sentinel
    }

    stream_t *out_arg_snapshot = fx->out;

	// ACT
	ret = stream_create(out_arg, vtbl_arg, backend_arg, env_arg);
	if (tc->scenario == STREAM_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT) {
		assert_int_equal(ret, STREAM_STATUS_OK);
		assert_non_null(fx->out);

		stream_destroy(&fx->out);
		assert_null(fx->out);

		stream_destroy(&fx->out);
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

static const test_stream_lifecycle_case_t CASE_STREAM_LIFECYCLE_OUT_NULL = {
	.name = "stream_lifecycle_out_null",
	.scenario = STREAM_LIFECYCLE_SCENARIO_OUT_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_CHECK_NONE
};

static const test_stream_lifecycle_case_t CASE_STREAM_LIFECYCLE_VTBL_NULL = {
	.name = "stream_lifecycle_vtbl_null",
	.scenario = STREAM_LIFECYCLE_SCENARIO_VTBL_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_lifecycle_case_t CASE_STREAM_LIFECYCLE_ENV_NULL = {
	.name = "stream_lifecycle_env_null",
	.scenario = STREAM_LIFECYCLE_SCENARIO_ENV_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_lifecycle_case_t CASE_STREAM_LIFECYCLE_ENV_MEM_NULL = {
	.name = "stream_lifecycle_env_mem_null",
	.scenario = STREAM_LIFECYCLE_SCENARIO_ENV_MEM_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_lifecycle_case_t CASE_STREAM_LIFECYCLE_OOM = {
	.name = "stream_lifecycle_oom",
	.scenario = STREAM_LIFECYCLE_SCENARIO_OOM,
	.fail_call_idx = 1,

	.expected_ret = STREAM_STATUS_OOM,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_stream_lifecycle_case_t CASE_STREAM_LIFECYCLE_DESTROY_IDEMPOTENT = {
	.name = "stream_lifecycle_destroy_idempotent",
	.scenario = STREAM_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_OK,
	.out_expect = OUT_EXPECT_NULL
};

static const test_stream_lifecycle_case_t CASE_STREAM_LIFECYCLE_OK = {
	.name = "stream_lifecycle_ok",
	.scenario = STREAM_LIFECYCLE_SCENARIO_OK,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_OK,
	.out_expect = OUT_EXPECT_NON_NULL
};

static const test_stream_lifecycle_case_t CASE_STREAM_LIFECYCLE_VTBL_READ_NULL = {
	.name = "stream_lifecycle_vtbl_read_null",
	.scenario = STREAM_LIFECYCLE_SCENARIO_VTBL_READ_NULL,
	.fail_call_idx = 0,

	.expected_ret = STREAM_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STREAM_LIFECYCLE_CASES(X) \
X(CASE_STREAM_LIFECYCLE_OUT_NULL) \
X(CASE_STREAM_LIFECYCLE_VTBL_NULL) \
X(CASE_STREAM_LIFECYCLE_ENV_NULL) \
X(CASE_STREAM_LIFECYCLE_ENV_MEM_NULL) \
X(CASE_STREAM_LIFECYCLE_OOM) \
X(CASE_STREAM_LIFECYCLE_DESTROY_IDEMPOTENT) \
X(CASE_STREAM_LIFECYCLE_OK) \
X(CASE_STREAM_LIFECYCLE_VTBL_READ_NULL)

#define STREAM_MAKE_LIFECYCLE_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_lifecycle, case_sym)

static const struct CMUnitTest stream_lifecycle_tests[] = {
	STREAM_LIFECYCLE_CASES(STREAM_MAKE_LIFECYCLE_TEST)
};

#undef STREAM_LIFECYCLE_CASES
#undef STREAM_MAKE_LIFECYCLE_TEST

/** @endcond */

/**
 * @brief Scenarios for `stream_read()`.
 *
 * size_t stream_read(stream_t *s, void *buf, size_t n, stream_status_t *st);
 *
 * Precondition:
 * - If `s != NULL`, `s` has been created by stream_create() with
 *   `fake_stream_vtbl` and `fake_stream_backend`.
 *
 * Doubles:
 * - fake_stream_backend_t
 * - fake_stream_vtbl
 * - fake_memory
 *
 * See also:
 * - @ref testing_foundation_stream_unit_stream_read "stream_read() unit tests section"
 * - @ref specifications_stream_read "stream_read() specifications"
 *
 * The scenarios below define the test oracle for `stream_read()`.
 */
typedef enum {
    /**
     * WHEN n == 0 and st != NULL
     * EXPECT:
     * - returns 0
     * - sets *st = STREAM_STATUS_OK
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_READ_SCENARIO_N_ZERO = 0,

    /**
     * WHEN n == 0 and st == NULL
     * EXPECT:
     * - returns 0
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_READ_SCENARIO_N_ZERO_ST_NULL,

    /**
     * WHEN n > 0 and s == NULL and st != NULL
     * EXPECT:
     * - returns 0
     * - sets *st = STREAM_STATUS_INVALID
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_READ_SCENARIO_S_NULL,

    /**
     * WHEN n > 0 and s == NULL and st == NULL
     * EXPECT:
     * - returns 0
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_READ_SCENARIO_S_NULL_ST_NULL,

    /**
     * WHEN n > 0 and buf == NULL and st != NULL
     * EXPECT:
     * - returns 0
     * - sets *st = STREAM_STATUS_INVALID
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_READ_SCENARIO_BUF_NULL,

    /**
     * WHEN n > 0 and buf == NULL and st == NULL
     * EXPECT:
     * - returns 0
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_READ_SCENARIO_BUF_NULL_ST_NULL,

    /**
     * WHEN n > 0 and s != NULL and buf != NULL and s->backend == NULL and st != NULL
     * EXPECT:
     * - returns 0
     * - sets *st = STREAM_STATUS_NO_BACKEND
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_READ_SCENARIO_BACKEND_NULL,

    /**
     * WHEN n > 0 and s != NULL and buf != NULL and s->backend == NULL and st == NULL
     * EXPECT:
     * - returns 0
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_READ_SCENARIO_BACKEND_NULL_ST_NULL,

    /**
     * WHEN n > 0 and s != NULL and buf != NULL and s->backend != NULL and st != NULL
     * AND fake_stream_backend is configured as:
     *   - read_ret = n
     *   - read_st_to_set = STREAM_STATUS_OK
     * EXPECT:
     * - calls fake_stream_vtbl.read(fake_stream_backend, buf, n, st) exactly once
     * - does not call fake_stream_vtbl.write/flush/close
     * - returns n
     * - sets *st = STREAM_STATUS_OK
     */
    STREAM_READ_SCENARIO_NOMINAL_OK,

    /**
     * WHEN n > 0 and s != NULL and buf != NULL and s->backend != NULL and st != NULL
     * AND fake_stream_backend is configured as:
     *   - read_ret == 0
     *   - read_st_to_set = STREAM_STATUS_EOF
     * EXPECT:
     * - calls fake_stream_vtbl.read(fake_stream_backend, buf, n, st) exactly once
     * - does not call fake_stream_vtbl.write/flush/close
     * - returns 0
     * - sets *st = STREAM_STATUS_EOF
     */
    STREAM_READ_SCENARIO_NOMINAL_EOF,

    /**
     * WHEN n > 0 and s != NULL and buf != NULL and s->backend != NULL and st == NULL
     * AND fake_stream_backend is configured as:
     *   - read_ret == 5
     * EXPECT:
     * - calls fake_stream_vtbl.read(fake_stream_backend, buf, n, st=NULL) exactly once
     * - does not call fake_stream_vtbl.write/flush/close
     * - returns 5
     */
    STREAM_READ_SCENARIO_NOMINAL_ST_NULL
} stream_read_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `stream_read()`.
 *
 * The scenario enum documents the oracle (WHEN/EXPECT). This struct only
 * carries the case name used by the test runner and the selected scenario.
 */
typedef struct {
    const char *name;
    stream_read_scenario_t scenario;
} test_stream_read_case_t;

/**
 * @brief Runtime fixture for `stream_read()` tests.
 *
 * Holds:
 * - the streams under test (nominal and "no backend" variant),
 * - injected environment (allocator),
 * - the fake adapter backend (spy/fake),
 * - input buffer storage and status storage,
 * - a pointer to the active parametric test case.
 */
typedef struct {
    stream_t *stream;
	stream_t *stream_no_backend;
    stream_env_t stream_env;
    fake_stream_backend_t fake_backend;
    uint8_t buf[32];
    stream_status_t st;
    const test_stream_read_case_t *tc;
} test_stream_read_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `stream_read()` tests.
 */
static int setup_stream_read(void **state) {
	const test_stream_read_case_t *tc = (const test_stream_read_case_t *) *state;
	test_stream_read_fixture_t *fx = (test_stream_read_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;
	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();

	fx->stream_env.mem = osal_mem_test_fake_ops();

	fx->fake_backend.close_ret = STREAM_STATUS_OK;

	assert_int_equal(
	    stream_create(&fx->stream, &fake_stream_vtbl, &fx->fake_backend, &fx->stream_env),
    	STREAM_STATUS_OK
	);

	assert_int_equal(
    	stream_create(&fx->stream_no_backend, &fake_stream_vtbl, NULL, &fx->stream_env),
    	STREAM_STATUS_OK
	);

	*state = fx;
	return 0;
}

/**
 * @brief Release the `stream_read()` test fixture and verify memory invariants.
 */
static int teardown_stream_read(void **state) {
	test_stream_read_fixture_t *fx = (test_stream_read_fixture_t *)*state;
    if (!fx) return 0;

	stream_destroy(&fx->stream);
	stream_destroy(&fx->stream_no_backend);

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
 * @brief Execute the active parametric test scenario for `stream_read()`.
 */
static void test_stream_read(void **state) {
	test_stream_read_fixture_t *fx = (test_stream_read_fixture_t *)*state;

	// ARRANGE
	size_t ret = (size_t)-1;
	stream_t *s_arg = fx->stream;
	void *buf_arg = fx->buf;
	size_t n_arg = 5;
	stream_status_t *st_arg = &fx->st;

	// poison status so tests are meaningful
	*st_arg = (stream_status_t)-1;

	fake_stream_backend_reset(&fx->fake_backend);

	switch (fx->tc->scenario) {
	case STREAM_READ_SCENARIO_N_ZERO: n_arg = 0; break;
	case STREAM_READ_SCENARIO_N_ZERO_ST_NULL: n_arg = 0; st_arg = NULL; break;
	case STREAM_READ_SCENARIO_S_NULL: s_arg = NULL; break;
	case STREAM_READ_SCENARIO_S_NULL_ST_NULL: s_arg = NULL; st_arg = NULL; break;
	case STREAM_READ_SCENARIO_BUF_NULL: buf_arg = NULL; break;
	case STREAM_READ_SCENARIO_BUF_NULL_ST_NULL: buf_arg = NULL; st_arg = NULL; break;
	case STREAM_READ_SCENARIO_BACKEND_NULL: s_arg = fx->stream_no_backend; break;
	case STREAM_READ_SCENARIO_BACKEND_NULL_ST_NULL: s_arg = fx->stream_no_backend; st_arg = NULL; break;
	case STREAM_READ_SCENARIO_NOMINAL_OK:
		fx->fake_backend.read_ret = n_arg;
		fx->fake_backend.read_st_to_set = STREAM_STATUS_OK;
		break;
	case STREAM_READ_SCENARIO_NOMINAL_EOF:
		fx->fake_backend.read_ret = 0;
		fx->fake_backend.read_st_to_set = STREAM_STATUS_EOF;
		break;
	case STREAM_READ_SCENARIO_NOMINAL_ST_NULL:
		fx->fake_backend.read_ret = 5;
		st_arg = NULL;
		break;
	default: fail();
	}

	// ACT
	ret = stream_read(s_arg, buf_arg, n_arg, st_arg);

	// ASSERT
	switch (fx->tc->scenario) {
	case STREAM_READ_SCENARIO_N_ZERO:
		assert_int_equal((int)ret, 0);
		assert_int_equal(*st_arg, STREAM_STATUS_OK);
		assert_int_equal(fx->fake_backend.read_called, 0);
		break;
	case STREAM_READ_SCENARIO_N_ZERO_ST_NULL:
		assert_int_equal((int)ret, 0);
		assert_int_equal(fx->fake_backend.read_called, 0);
		break;
	case STREAM_READ_SCENARIO_S_NULL:
		assert_int_equal((int)ret, 0);
		assert_int_equal(*st_arg, STREAM_STATUS_INVALID);
		assert_int_equal(fx->fake_backend.read_called, 0);
		break;
	case STREAM_READ_SCENARIO_S_NULL_ST_NULL:
		assert_int_equal((int)ret, 0);
		assert_int_equal(fx->fake_backend.read_called, 0);
		break;
	case STREAM_READ_SCENARIO_BUF_NULL:
		assert_int_equal((int)ret, 0);
		assert_int_equal(*st_arg, STREAM_STATUS_INVALID);
		assert_int_equal(fx->fake_backend.read_called, 0);
		break;
	case STREAM_READ_SCENARIO_BUF_NULL_ST_NULL:
		assert_int_equal((int)ret, 0);
		assert_int_equal(fx->fake_backend.read_called, 0);
		break;
	case STREAM_READ_SCENARIO_BACKEND_NULL:
		assert_int_equal((int)ret, 0);
		assert_int_equal(*st_arg, STREAM_STATUS_NO_BACKEND);
		assert_int_equal(fx->fake_backend.read_called, 0);
		break;
	case STREAM_READ_SCENARIO_BACKEND_NULL_ST_NULL:
		assert_int_equal((int)ret, 0);
		assert_int_equal(fx->fake_backend.read_called, 0);
		break;
	case STREAM_READ_SCENARIO_NOMINAL_OK:
		assert_int_equal((int)ret, (int)fx->fake_backend.read_ret);
		assert_int_equal(*st_arg, STREAM_STATUS_OK);
		assert_int_equal(fx->fake_backend.read_called, 1);
		assert_ptr_equal(fx->fake_backend.last_backend, &fx->fake_backend);
		assert_ptr_equal(fx->fake_backend.last_read_buf, buf_arg);
		assert_int_equal((int)fx->fake_backend.last_read_n, (int)n_arg);
		assert_ptr_equal(fx->fake_backend.last_read_st, st_arg);
		break;
	case STREAM_READ_SCENARIO_NOMINAL_EOF:
		assert_int_equal((int)ret, (int)fx->fake_backend.read_ret);
		assert_int_equal(*st_arg, STREAM_STATUS_EOF);
		assert_int_equal(fx->fake_backend.read_called, 1);
		assert_ptr_equal(fx->fake_backend.last_backend, &fx->fake_backend);
		assert_ptr_equal(fx->fake_backend.last_read_buf, buf_arg);
		assert_int_equal((int)fx->fake_backend.last_read_n, (int)n_arg);
		assert_ptr_equal(fx->fake_backend.last_read_st, st_arg);
		break;
	case STREAM_READ_SCENARIO_NOMINAL_ST_NULL:
		assert_int_equal((int)ret, (int)fx->fake_backend.read_ret);
		assert_int_equal(fx->fake_backend.read_called, 1);
		assert_ptr_equal(fx->fake_backend.last_backend, &fx->fake_backend);
		assert_ptr_equal(fx->fake_backend.last_read_buf, buf_arg);
		assert_int_equal((int)fx->fake_backend.last_read_n, (int)n_arg);
		assert_ptr_equal(fx->fake_backend.last_read_st, NULL);
		break;
	default: fail();
	}

	assert_int_equal(fx->fake_backend.write_called, 0);
	assert_int_equal(fx->fake_backend.flush_called, 0);
	assert_int_equal(fx->fake_backend.close_called, 0);
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_stream_read_case_t CASE_STREAM_READ_N_ZERO = {
	.name = "stream_read_n_zero",
	.scenario = STREAM_READ_SCENARIO_N_ZERO,
};

static const test_stream_read_case_t CASE_STREAM_READ_N_ZERO_ST_NULL = {
	.name = "stream_read_n_zero_st_null",
	.scenario = STREAM_READ_SCENARIO_N_ZERO_ST_NULL,
};

static const test_stream_read_case_t CASE_STREAM_READ_S_NULL = {
	.name = "stream_read_s_null",
	.scenario = STREAM_READ_SCENARIO_S_NULL,
};

static const test_stream_read_case_t CASE_STREAM_READ_S_NULL_ST_NULL = {
	.name = "stream_read_s_null_st_null",
	.scenario = STREAM_READ_SCENARIO_S_NULL_ST_NULL,
};

static const test_stream_read_case_t CASE_STREAM_READ_BUF_NULL = {
	.name = "stream_read_buf_null",
	.scenario = STREAM_READ_SCENARIO_BUF_NULL,
};

static const test_stream_read_case_t CASE_STREAM_READ_BUF_NULL_ST_NULL = {
	.name = "stream_read_buf_null_st_null",
	.scenario = STREAM_READ_SCENARIO_BUF_NULL_ST_NULL,
};

static const test_stream_read_case_t CASE_STREAM_READ_BACKEND_NULL = {
	.name = "stream_read_backend_null",
	.scenario = STREAM_READ_SCENARIO_BACKEND_NULL,
};

static const test_stream_read_case_t CASE_STREAM_READ_BACKEND_NULL_ST_NULL = {
	.name = "stream_read_backend_null_st_null",
	.scenario = STREAM_READ_SCENARIO_BACKEND_NULL_ST_NULL,
};

static const test_stream_read_case_t CASE_STREAM_READ_NOMINAL_OK = {
	.name = "stream_read_nominal_ok",
	.scenario = STREAM_READ_SCENARIO_NOMINAL_OK,
};

static const test_stream_read_case_t CASE_STREAM_READ_NOMINAL_EOF = {
	.name = "stream_read_nominal_eof",
	.scenario = STREAM_READ_SCENARIO_NOMINAL_EOF,
};

static const test_stream_read_case_t CASE_STREAM_READ_NOMINAL_ST_NULL = {
	.name = "stream_read_nominal_st_null",
	.scenario = STREAM_READ_SCENARIO_NOMINAL_ST_NULL,
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STREAM_READ_CASES(X) \
X(CASE_STREAM_READ_N_ZERO) \
X(CASE_STREAM_READ_N_ZERO_ST_NULL) \
X(CASE_STREAM_READ_S_NULL) \
X(CASE_STREAM_READ_S_NULL_ST_NULL) \
X(CASE_STREAM_READ_BUF_NULL) \
X(CASE_STREAM_READ_BUF_NULL_ST_NULL) \
X(CASE_STREAM_READ_BACKEND_NULL) \
X(CASE_STREAM_READ_BACKEND_NULL_ST_NULL) \
X(CASE_STREAM_READ_NOMINAL_OK) \
X(CASE_STREAM_READ_NOMINAL_EOF) \
X(CASE_STREAM_READ_NOMINAL_ST_NULL)

#define STREAM_MAKE_STREAM_READ_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_read, case_sym)

static const struct CMUnitTest stream_read_tests[] = {
	STREAM_READ_CASES(STREAM_MAKE_STREAM_READ_TEST)
};

#undef STREAM_READ_CASES
#undef STREAM_MAKE_STREAM_READ_TEST

/** @endcond */

/**
 * @brief Scenarios for `stream_write()`.
 *
 * size_t stream_write(stream_t *s, const void *buf, size_t n, stream_status_t *st);
 *
 * Precondition:
 * - If `s != NULL`, `s` has been created by stream_create() with
 *   `fake_stream_vtbl` and `fake_stream_backend`.
 *
 * Doubles:
 * - fake_stream_backend_t, fake_stream_vtbl
 *
 * See also:
 * - @ref testing_foundation_stream_unit_stream_write "stream_write() unit tests section"
 * - @ref specifications_stream_write "stream_write() specifications"
 *
 * The scenarios below define the test oracle for `stream_write()`.
 */
typedef enum {
    /**
     * WHEN n == 0 and st != NULL
     * EXPECT:
     * - returns 0
     * - sets *st = STREAM_STATUS_OK
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_WRITE_SCENARIO_N_ZERO = 0,

    /**
     * WHEN n == 0 and st == NULL
     * EXPECT:
     * - returns 0
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_WRITE_SCENARIO_N_ZERO_ST_NULL,

    /**
     * WHEN n > 0 and s == NULL and st != NULL
     * EXPECT:
     * - returns 0
     * - sets *st = STREAM_STATUS_INVALID
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_WRITE_SCENARIO_S_NULL,

    /**
     * WHEN n > 0 and s == NULL and st == NULL
     * EXPECT:
     * - returns 0
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_WRITE_SCENARIO_S_NULL_ST_NULL,

    /**
     * WHEN n > 0 and buf == NULL and st != NULL
     * EXPECT:
     * - returns 0
     * - sets *st = STREAM_STATUS_INVALID
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_WRITE_SCENARIO_BUF_NULL,

    /**
     * WHEN n > 0 and buf == NULL and st == NULL
     * EXPECT:
     * - returns 0
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_WRITE_SCENARIO_BUF_NULL_ST_NULL,

    /**
     * WHEN n > 0 and s != NULL and buf != NULL and s->backend == NULL and st != NULL
     * EXPECT:
     * - returns 0
     * - sets *st = STREAM_STATUS_NO_BACKEND
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_WRITE_SCENARIO_BACKEND_NULL,

    /**
     * WHEN n > 0 and s != NULL and buf != NULL and s->backend == NULL and st == NULL
     * EXPECT:
     * - returns 0
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_WRITE_SCENARIO_BACKEND_NULL_ST_NULL,

    /**
     * WHEN n > 0 and s != NULL and buf != NULL and s->backend != NULL and st != NULL
     * AND fake_stream_backend is configured as:
     *   - write_ret = n
     *   - write_st_to_set = STREAM_STATUS_OK
     * EXPECT:
     * - calls fake_stream_vtbl.write(fake_stream_backend, buf, n, st) exactly once
     * - does not call fake_stream_vtbl.read/flush/close
     * - returns n
     * - sets *st = STREAM_STATUS_OK
     */
    STREAM_WRITE_SCENARIO_NOMINAL_OK,

    /**
     * WHEN n > 0 and s != NULL and buf != NULL and s->backend != NULL and st != NULL
     * AND fake_stream_backend is configured as:
     *   - write_ret == 0
     *   - write_st_to_set = STREAM_STATUS_IO_ERROR
     * EXPECT:
     * - calls fake_stream_vtbl.write(fake_stream_backend, buf, n, st) exactly once
     * - does not call fake_stream_vtbl.read/flush/close
     * - returns 0
     * - sets *st = STREAM_STATUS_IO_ERROR
     */
    STREAM_WRITE_SCENARIO_NOMINAL_IO_ERROR,

    /**
     * WHEN n > 0 and s != NULL and buf != NULL and s->backend != NULL and st == NULL
     * AND fake_stream_backend is configured as:
     *   - write_ret == 5
     * EXPECT:
     * - calls fake_stream_vtbl.write(fake_stream_backend, buf, n, st=NULL) exactly once
     * - does not call fake_stream_vtbl.read/flush/close
     * - returns 5
     */
    STREAM_WRITE_SCENARIO_NOMINAL_ST_NULL
} stream_write_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `stream_write()`.
 *
 * The scenario enum documents the oracle (WHEN/EXPECT). This struct only
 * carries the case name used by the test runner and the selected scenario.
 */
typedef struct {
    const char *name;
    stream_write_scenario_t scenario;
} test_stream_write_case_t;

/**
 * @brief Runtime fixture for `stream_write()` tests.
 *
 * Holds:
 * - the streams under test (nominal and "no backend" variant),
 * - injected environment (allocator),
 * - the fake adapter backend (spy/fake),
 * - input buffer storage and status storage,
 * - a pointer to the active parametric test case.
 */
typedef struct {
    stream_t *stream;
	stream_t *stream_no_backend;
    stream_env_t stream_env;
    fake_stream_backend_t fake_backend;
    uint8_t buf[32];
    stream_status_t st;
    const test_stream_write_case_t *tc;
} test_stream_write_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `stream_write()` tests.
 */
static int setup_stream_write(void **state) {
	const test_stream_write_case_t *tc = (const test_stream_write_case_t *) *state;
	test_stream_write_fixture_t *fx = (test_stream_write_fixture_t *)malloc(sizeof(*fx));
	if (!fx) return -1;
	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();

	fx->stream_env.mem = osal_mem_test_fake_ops();

	fx->fake_backend.close_ret = STREAM_STATUS_OK;

	assert_int_equal(
	    stream_create(&fx->stream, &fake_stream_vtbl, &fx->fake_backend, &fx->stream_env),
    	STREAM_STATUS_OK
	);

	assert_int_equal(
    	stream_create(&fx->stream_no_backend, &fake_stream_vtbl, NULL, &fx->stream_env),
    	STREAM_STATUS_OK
	);

	*state = fx;
	return 0;
}

/**
 * @brief Release the `stream_write()` test fixture and verify memory invariants.
 */
static int teardown_stream_write(void **state) {
	test_stream_write_fixture_t *fx = (test_stream_write_fixture_t *)*state;
    if (!fx) return 0;

	stream_destroy(&fx->stream);
	stream_destroy(&fx->stream_no_backend);

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
 * @brief Execute the active parametric test scenario for `stream_write()`.
 */
static void test_stream_write(void **state) {
	test_stream_write_fixture_t *fx = (test_stream_write_fixture_t *)*state;

	// ARRANGE
	size_t ret = (size_t)-1;
	stream_t *s_arg = fx->stream;
	const void *buf_arg = fx->buf;
	size_t n_arg = 5;
	stream_status_t *st_arg = &fx->st;

	// poison status so tests are meaningful
	*st_arg = (stream_status_t)-1;

	fake_stream_backend_reset(&fx->fake_backend);

	switch (fx->tc->scenario) {
	case STREAM_WRITE_SCENARIO_N_ZERO: n_arg = 0; break;
	case STREAM_WRITE_SCENARIO_N_ZERO_ST_NULL: n_arg = 0; st_arg = NULL; break;
	case STREAM_WRITE_SCENARIO_S_NULL: s_arg = NULL; break;
	case STREAM_WRITE_SCENARIO_S_NULL_ST_NULL: s_arg = NULL; st_arg = NULL; break;
	case STREAM_WRITE_SCENARIO_BUF_NULL: buf_arg = NULL; break;
	case STREAM_WRITE_SCENARIO_BUF_NULL_ST_NULL: buf_arg = NULL; st_arg = NULL; break;
	case STREAM_WRITE_SCENARIO_BACKEND_NULL: s_arg = fx->stream_no_backend; break;
	case STREAM_WRITE_SCENARIO_BACKEND_NULL_ST_NULL: s_arg = fx->stream_no_backend; st_arg = NULL; break;
	case STREAM_WRITE_SCENARIO_NOMINAL_OK:
		fx->fake_backend.write_ret = n_arg;
		fx->fake_backend.write_st_to_set = STREAM_STATUS_OK;
		break;
	case STREAM_WRITE_SCENARIO_NOMINAL_IO_ERROR:
		fx->fake_backend.write_ret = 0;
		fx->fake_backend.write_st_to_set = STREAM_STATUS_IO_ERROR;
		break;
	case STREAM_WRITE_SCENARIO_NOMINAL_ST_NULL:
		fx->fake_backend.write_ret = 5;
		st_arg = NULL;
		break;
	default: fail();
	}

	// ACT
	ret = stream_write(s_arg, buf_arg, n_arg, st_arg);

	// ASSERT
	switch (fx->tc->scenario) {
	case STREAM_WRITE_SCENARIO_N_ZERO:
		assert_int_equal((int)ret, 0);
		assert_int_equal(*st_arg, STREAM_STATUS_OK);
		assert_int_equal(fx->fake_backend.write_called, 0);
		break;
	case STREAM_WRITE_SCENARIO_N_ZERO_ST_NULL:
		assert_int_equal((int)ret, 0);
		assert_int_equal(fx->fake_backend.write_called, 0);
		break;
	case STREAM_WRITE_SCENARIO_S_NULL:
		assert_int_equal((int)ret, 0);
		assert_int_equal(*st_arg, STREAM_STATUS_INVALID);
		assert_int_equal(fx->fake_backend.write_called, 0);
		break;
	case STREAM_WRITE_SCENARIO_S_NULL_ST_NULL:
		assert_int_equal((int)ret, 0);
		assert_int_equal(fx->fake_backend.write_called, 0);
		break;
	case STREAM_WRITE_SCENARIO_BUF_NULL:
		assert_int_equal((int)ret, 0);
		assert_int_equal(*st_arg, STREAM_STATUS_INVALID);
		assert_int_equal(fx->fake_backend.write_called, 0);
		break;
	case STREAM_WRITE_SCENARIO_BUF_NULL_ST_NULL:
		assert_int_equal((int)ret, 0);
		assert_int_equal(fx->fake_backend.write_called, 0);
		break;
	case STREAM_WRITE_SCENARIO_BACKEND_NULL:
		assert_int_equal((int)ret, 0);
		assert_int_equal(*st_arg, STREAM_STATUS_NO_BACKEND);
		assert_int_equal(fx->fake_backend.write_called, 0);
		break;
	case STREAM_WRITE_SCENARIO_BACKEND_NULL_ST_NULL:
		assert_int_equal((int)ret, 0);
		assert_int_equal(fx->fake_backend.write_called, 0);
		break;
	case STREAM_WRITE_SCENARIO_NOMINAL_OK:
		assert_int_equal((int)ret, (int)fx->fake_backend.write_ret);
		assert_int_equal(*st_arg, STREAM_STATUS_OK);
		assert_int_equal(fx->fake_backend.write_called, 1);
		assert_ptr_equal(fx->fake_backend.last_backend, &fx->fake_backend);
		assert_ptr_equal(fx->fake_backend.last_write_buf, buf_arg);
		assert_int_equal((int)fx->fake_backend.last_write_n, (int)n_arg);
		assert_ptr_equal(fx->fake_backend.last_write_st, st_arg);
		break;
	case STREAM_WRITE_SCENARIO_NOMINAL_IO_ERROR:
		assert_int_equal((int)ret, (int)fx->fake_backend.write_ret);
		assert_int_equal(*st_arg, STREAM_STATUS_IO_ERROR);
		assert_int_equal(fx->fake_backend.write_called, 1);
		assert_ptr_equal(fx->fake_backend.last_backend, &fx->fake_backend);
		assert_ptr_equal(fx->fake_backend.last_write_buf, buf_arg);
		assert_int_equal((int)fx->fake_backend.last_write_n, (int)n_arg);
		assert_ptr_equal(fx->fake_backend.last_write_st, st_arg);
		break;
	case STREAM_WRITE_SCENARIO_NOMINAL_ST_NULL:
		assert_int_equal((int)ret, (int)fx->fake_backend.write_ret);
		assert_int_equal(fx->fake_backend.write_called, 1);
		assert_ptr_equal(fx->fake_backend.last_backend, &fx->fake_backend);
		assert_ptr_equal(fx->fake_backend.last_write_buf, buf_arg);
		assert_int_equal((int)fx->fake_backend.last_write_n, (int)n_arg);
		assert_ptr_equal(fx->fake_backend.last_write_st, NULL);
		break;
	default: fail();
	}

	assert_int_equal(fx->fake_backend.read_called, 0);
	assert_int_equal(fx->fake_backend.flush_called, 0);
	assert_int_equal(fx->fake_backend.close_called, 0);
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_stream_write_case_t CASE_STREAM_WRITE_N_ZERO = {
	.name = "stream_write_n_zero",
	.scenario = STREAM_WRITE_SCENARIO_N_ZERO,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_N_ZERO_ST_NULL = {
	.name = "stream_write_n_zero_st_null",
	.scenario = STREAM_WRITE_SCENARIO_N_ZERO_ST_NULL,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_S_NULL = {
	.name = "stream_write_s_null",
	.scenario = STREAM_WRITE_SCENARIO_S_NULL,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_S_NULL_ST_NULL = {
	.name = "stream_write_s_null_st_null",
	.scenario = STREAM_WRITE_SCENARIO_S_NULL_ST_NULL,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_BUF_NULL = {
	.name = "stream_write_buf_null",
	.scenario = STREAM_WRITE_SCENARIO_BUF_NULL,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_BUF_NULL_ST_NULL = {
	.name = "stream_write_buf_null_st_null",
	.scenario = STREAM_WRITE_SCENARIO_BUF_NULL_ST_NULL,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_BACKEND_NULL = {
	.name = "stream_write_backend_null",
	.scenario = STREAM_WRITE_SCENARIO_BACKEND_NULL,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_BACKEND_NULL_ST_NULL = {
	.name = "stream_write_backend_null_st_null",
	.scenario = STREAM_WRITE_SCENARIO_BACKEND_NULL_ST_NULL,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_NOMINAL_OK = {
	.name = "stream_write_nominal_ok",
	.scenario = STREAM_WRITE_SCENARIO_NOMINAL_OK,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_NOMINAL_IO_ERROR = {
	.name = "stream_write_nominal_io_error",
	.scenario = STREAM_WRITE_SCENARIO_NOMINAL_IO_ERROR,
};

static const test_stream_write_case_t CASE_STREAM_WRITE_NOMINAL_ST_NULL = {
	.name = "stream_write_nominal_st_null",
	.scenario = STREAM_WRITE_SCENARIO_NOMINAL_ST_NULL,
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STREAM_WRITE_CASES(X) \
X(CASE_STREAM_WRITE_N_ZERO) \
X(CASE_STREAM_WRITE_N_ZERO_ST_NULL) \
X(CASE_STREAM_WRITE_S_NULL) \
X(CASE_STREAM_WRITE_S_NULL_ST_NULL) \
X(CASE_STREAM_WRITE_BUF_NULL) \
X(CASE_STREAM_WRITE_BUF_NULL_ST_NULL) \
X(CASE_STREAM_WRITE_BACKEND_NULL) \
X(CASE_STREAM_WRITE_BACKEND_NULL_ST_NULL) \
X(CASE_STREAM_WRITE_NOMINAL_OK) \
X(CASE_STREAM_WRITE_NOMINAL_IO_ERROR) \
X(CASE_STREAM_WRITE_NOMINAL_ST_NULL)

#define STREAM_MAKE_STREAM_WRITE_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_write, case_sym)

static const struct CMUnitTest stream_write_tests[] = {
	STREAM_WRITE_CASES(STREAM_MAKE_STREAM_WRITE_TEST)
};

#undef STREAM_WRITE_CASES
#undef STREAM_MAKE_STREAM_WRITE_TEST

/** @endcond */

/**
 * @brief Scenarios for `stream_flush()`.
 *
 * stream_status_t stream_flush(stream_t *s);
 *
 * Precondition:
 * - If `s != NULL`, `s` has been created by stream_create() with
 *   `fake_stream_vtbl` and `fake_stream_backend`.
 *
 * Doubles:
 * - fake_stream_backend_t, fake_stream_vtbl
 *
 * See also:
 * - @ref testing_foundation_stream_unit_stream_flush "stream_flush() unit tests section"
 * - @ref specifications_stream_flush "stream_flush() specifications"
 *
 * The scenarios below define the test oracle for `stream_flush()`.
 */
typedef enum {
    /**
     * WHEN s == NULL
     * EXPECT:
     * - returns STREAM_STATUS_INVALID
     * - does not call fake_stream_vtbl.read/write/flush/close
     */
    STREAM_FLUSH_SCENARIO_S_NULL = 0,

    /**
     * WHEN s != NULL and s->backend == NULL
     * EXPECT:
     * - returns STREAM_STATUS_NO_BACKEND
     * - does not call fake_stream_vtbl.flush
     */
    STREAM_FLUSH_SCENARIO_BACKEND_NULL,

    /**
     * WHEN s != NULL and s->backend != NULL
     * AND fake_stream_backend is configured as:
     *   - flush_ret = STREAM_STATUS_OK
     * EXPECT:
     * - returns STREAM_STATUS_OK
     * - calls fake_stream_vtbl.flush(fake_stream_backend) exactly once
     * - does not call fake_stream_vtbl.read/write/close
     */
    STREAM_FLUSH_SCENARIO_NOMINAL_OK,

    /**
     * WHEN s != NULL and s->backend != NULL
     * AND fake_stream_backend is configured as:
     *   - flush_ret = STREAM_STATUS_IO_ERROR
     * EXPECT:
     * - returns STREAM_STATUS_IO_ERROR
     * - calls fake_stream_vtbl.flush(fake_stream_backend) exactly once
     * - does not call fake_stream_vtbl.read/write/close
     */
    STREAM_FLUSH_SCENARIO_NOMINAL_IO_ERROR
} stream_flush_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `stream_flush()`.
 *
 * The scenario enum documents the oracle (WHEN/EXPECT). This struct only
 * carries the case name used by the test runner and the selected scenario.
 */
typedef struct {
    const char *name;
    stream_flush_scenario_t scenario;
} test_stream_flush_case_t;

/**
 * @brief Runtime fixture for `stream_flush()` tests.
 *
 * Holds:
 * - the streams under test (nominal and "no backend" variant),
 * - injected environment (allocator),
 * - the fake adapter backend (spy/fake),
 * - a pointer to the active parametric test case.
 */
typedef struct {
    stream_t *stream;
    stream_t *stream_no_backend;
    stream_env_t env;
    fake_stream_backend_t backend;
    const test_stream_flush_case_t *tc;
} test_stream_flush_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `stream_flush()` tests.
 */
static int setup_stream_flush(void **state)
{
    const test_stream_flush_case_t *tc = (const test_stream_flush_case_t *)(*state);

    test_stream_flush_fixture_t *fx = (test_stream_flush_fixture_t *)malloc(sizeof(*fx));
    if (!fx) return -1;

    osal_memset(fx, 0, sizeof(*fx));
    fx->tc = tc;

    fake_memory_reset();

    fx->env.mem = osal_mem_test_fake_ops();

    fake_stream_backend_reset(&fx->backend);
    fx->backend.close_ret = STREAM_STATUS_OK;

    assert_int_equal(
        stream_create(&fx->stream, &fake_stream_vtbl, &fx->backend, &fx->env),
        STREAM_STATUS_OK
    );

    assert_int_equal(
        stream_create(&fx->stream_no_backend, &fake_stream_vtbl, NULL, &fx->env),
        STREAM_STATUS_OK
    );

    *state = fx;
    return 0;
}

/**
 * @brief Release the `stream_flush()` test fixture and verify memory invariants.
 */
static int teardown_stream_flush(void **state)
{
    test_stream_flush_fixture_t *fx = (test_stream_flush_fixture_t *)(*state);
    if (!fx) return 0;

    stream_destroy(&fx->stream);
    stream_destroy(&fx->stream_no_backend);

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
 * @brief Execute the active parametric test scenario for `stream_flush()`.
 */
static void test_stream_flush(void **state)
{
    test_stream_flush_fixture_t *fx = (test_stream_flush_fixture_t *)*state;

    stream_t *s_arg = fx->stream;

    fake_stream_backend_reset(&fx->backend);

    switch (fx->tc->scenario) {
        case STREAM_FLUSH_SCENARIO_S_NULL:
            s_arg = NULL;
            break;
        case STREAM_FLUSH_SCENARIO_BACKEND_NULL:
            s_arg = fx->stream_no_backend;
            break;
        case STREAM_FLUSH_SCENARIO_NOMINAL_OK:
            fx->backend.flush_ret = STREAM_STATUS_OK;
            break;
        case STREAM_FLUSH_SCENARIO_NOMINAL_IO_ERROR:
            fx->backend.flush_ret = STREAM_STATUS_IO_ERROR;
            break;
        default:
            fail();
    }

    stream_status_t ret = stream_flush(s_arg);

    switch (fx->tc->scenario) {
        case STREAM_FLUSH_SCENARIO_S_NULL:
            assert_int_equal(ret, STREAM_STATUS_INVALID);
            assert_int_equal(fx->backend.flush_called, 0);
            break;
        case STREAM_FLUSH_SCENARIO_BACKEND_NULL:
            assert_int_equal(ret, STREAM_STATUS_NO_BACKEND);
            assert_int_equal(fx->backend.flush_called, 0);
            break;
        case STREAM_FLUSH_SCENARIO_NOMINAL_OK:
            assert_int_equal(ret, STREAM_STATUS_OK);
            assert_int_equal(fx->backend.flush_called, 1);
            assert_ptr_equal(fx->backend.last_backend, &fx->backend);
            break;
        case STREAM_FLUSH_SCENARIO_NOMINAL_IO_ERROR:
            assert_int_equal(ret, STREAM_STATUS_IO_ERROR);
            assert_int_equal(fx->backend.flush_called, 1);
            assert_ptr_equal(fx->backend.last_backend, &fx->backend);
            break;
        default:
            fail();
    }

    assert_int_equal(fx->backend.read_called, 0);
    assert_int_equal(fx->backend.write_called, 0);
    assert_int_equal(fx->backend.close_called, 0);
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_stream_flush_case_t CASE_STREAM_FLUSH_S_NULL = {
	.name = "stream_flush_s_null",
	.scenario = STREAM_FLUSH_SCENARIO_S_NULL,
};

static const test_stream_flush_case_t CASE_STREAM_FLUSH_BACKEND_NULL = {
	.name = "stream_flush_backend_null",
	.scenario = STREAM_FLUSH_SCENARIO_BACKEND_NULL,
};

static const test_stream_flush_case_t CASE_STREAM_FLUSH_NOMINAL_OK = {
	.name = "stream_flush_nominal_ok",
	.scenario = STREAM_FLUSH_SCENARIO_NOMINAL_OK,
};

static const test_stream_flush_case_t CASE_STREAM_FLUSH_NOMINAL_IO_ERROR = {
	.name = "stream_flush_nominal_io_error",
	.scenario = STREAM_FLUSH_SCENARIO_NOMINAL_IO_ERROR,
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STREAM_FLUSH_CASES(X) \
X(CASE_STREAM_FLUSH_S_NULL) \
X(CASE_STREAM_FLUSH_BACKEND_NULL) \
X(CASE_STREAM_FLUSH_NOMINAL_OK) \
X(CASE_STREAM_FLUSH_NOMINAL_IO_ERROR)

#define STREAM_MAKE_STREAM_FLUSH_TEST(case_sym) \
LEXLEO_MAKE_TEST(stream_flush, case_sym)

static const struct CMUnitTest stream_flush_tests[] = {
	STREAM_FLUSH_CASES(STREAM_MAKE_STREAM_FLUSH_TEST)
};

#undef STREAM_FLUSH_CASES
#undef STREAM_MAKE_STREAM_FLUSH_TEST

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	static const struct CMUnitTest stream_tests[] = {
		cmocka_unit_test(test_stream_default_ops),
		cmocka_unit_test(test_stream_default_env)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(stream_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stream_lifecycle_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stream_read_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stream_write_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stream_flush_tests, NULL, NULL);
	return failed;
}

/** @endcond */
