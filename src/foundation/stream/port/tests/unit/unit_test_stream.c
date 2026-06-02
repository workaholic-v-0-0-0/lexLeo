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

#include "stream/owners/stream_owners_api.h"
#include "stream/cr/stream_cr_api.h"

#include "stream/tests/stream_white_box_tests_access.h"

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
 * @brief Test `stream_default_env()`.
 *
 * Doubles:
 * - dummy `stream_vtbl_t`
 * - dummy `osal_mem_ops_t`
 *
* See contract:
 * - @ref specifications_stream_default_env "stream_default_env() specifications".
 *
 * See test description:
 * - @ref testing_foundation_stream_unit_stream_default_env "stream_default_env() unit tests section"
 */
static void test_stream_default_env(void **state) {
	(void)state;

	const stream_vtbl_t dummy_vtbl = {0};
	const stream_vtbl_t *dummy_vtbl_p = &dummy_vtbl;

	const osal_mem_ops_t dummy_mem_ops = {0};
	const osal_mem_ops_t *dummy_mem_ops_p = &dummy_mem_ops;

	stream_env_t ret =
		stream_default_env(
			dummy_vtbl_p,
			dummy_mem_ops_p
		);

	assert_ptr_equal(ret.mem, dummy_mem_ops_p);
	assert_ptr_equal(ret.vtbl, dummy_vtbl_p);
}

/**
 * @brief Scenarios for `stream_create()` / `stream_complete_default_init` /
 * `stream_destroy()`.
 *
 * Doubles:
 * - stream backend defined in the beginning of this file
 * - fake_memory
 *
 * See contract:
 * - @ref specifications_stream_create "stream_create() specifications"
 * - @ref specifications_stream_complete_default_init "stream_complete_default_init() specifications"
 * - @ref specifications_stream_destroy "stream_destroy() specifications"
 *
 * See test description:
 * - @ref testing_foundation_stream_unit_stream_lifecycle "stream lifecycle unit tests section"
 *
 * The scenarios below define the test oracle for `stream` lifecycle.
 */
typedef enum {
	/**
	 * WHEN the nominal stream lifecycle is executed:
	 * - `stream_create(out, env)`
	 * - `stream_complete_default_init(*out, backend)`
	 * - `stream_destroy(out)`
	 *
	 * EXPECT:
	 * - `stream_create()` returns `STREAM_STATUS_OK`
	 * - `*out != NULL`
	 * - `(*out)->backend == NULL`
	 * - `stream_complete_default_init()` returns `STREAM_STATUS_OK`
	 * - `(*out)->backend == backend`
	 * - `stream_destroy()` releases the handle and sets `*out` to `NULL`
	 * - no memory leak, invalid free or double free
	 */
	STREAM_LIFECYCLE_SCENARIO_OK = 0,

	/**
	 * WHEN allocation of the stream handle fails during
	 * `stream_create(out, env)`
	 *
	 * EXPECT:
	 * - `stream_create()` returns `STREAM_STATUS_OOM`
	 * - `*out == NULL`
	 * - no memory leak, invalid free or double free
	 */
	STREAM_LIFECYCLE_SCENARIO_OOM,

	/**
	 * WHEN `stream_destroy(out)` is called twice after a successfully completed
	 * stream lifecycle
	 *
	 * EXPECT:
	 * - the first call releases the backend and the stream handle
	 * - the first call sets `*out` to `NULL`
	 * - the second call leaves `*out == NULL`
	 * - the backend close operation is invoked exactly once
	 * - no memory leak, invalid free or double free
	 */
	STREAM_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT,

} stream_lifecycle_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for the stream lifecycle contract.
 */
typedef struct {
	const char *name;
	stream_lifecycle_scenario_t scenario;
	size_t fail_call_idx;
} test_stream_lifecycle_case_t;

/**
 * @brief Runtime fixture for the stream lifecycle contract.
 */
typedef struct {
	// runtime resources
	stream_t *out;

	// DI
	stream_env_t env;
	fake_stream_backend_t backend;

	const test_stream_lifecycle_case_t *tc;
} test_stream_lifecycle_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for stream lifecycle tests.
 */
static int setup_stream_lifecycle(void **state)
{
	const test_stream_lifecycle_case_t *tc = (const test_stream_lifecycle_case_t *)(*state);
	test_stream_lifecycle_fixture_t *fx = (test_stream_lifecycle_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;
	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();
	if (tc->scenario == STREAM_LIFECYCLE_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	// borrowed DI
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.vtbl = &fake_stream_vtbl;

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

	osal_free(fx);
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one parametric test scenario for `stream_create()` / `stream_destroy()`.
 */
static void test_stream_lifecycle(void **state) {
	test_stream_lifecycle_fixture_t *fx =
		(test_stream_lifecycle_fixture_t *)(*state);
	const test_stream_lifecycle_case_t *tc = fx->tc;

	// ARRANGE stream_create()
	stream_status_t ret = STREAM_STATUS_INVALID;

	// ACT stream_create()
	ret = stream_create(&fx->out, &fx->env);

	// ASSERT stream_create()
	if (tc->scenario == STREAM_LIFECYCLE_SCENARIO_OOM) {
		assert_int_equal(ret, STREAM_STATUS_OOM);
		assert_null(fx->out);
		return;
	}
	assert_int_equal(ret, STREAM_STATUS_OK);
	assert_non_null(fx->out);
	assert_null(stream_get_backend(fx->out));

	// ACT stream_complete_default_init()
	ret = stream_complete_default_init(fx->out, &fx->backend);

	// ASSERT stream_complete_default_init()
	assert_int_equal(ret, STREAM_STATUS_OK);
	assert_non_null(fx->out);
	assert_ptr_equal(stream_get_backend(fx->out), &fx->backend);

	// ACT stream_destroy()
	ret = stream_destroy(&fx->out);

	// ASSERT stream_destroy()
	assert_int_equal(ret, STREAM_STATUS_OK);
	assert_null(fx->out);
	if (tc->scenario == STREAM_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT) {
		assert_int_equal(fx->backend.close_called, 1);

		// ACT second stream_destroy()
		ret = stream_destroy(&fx->out);

		// ASSERT second stream_destroy()
		assert_int_equal(ret, STREAM_STATUS_OK);
		assert_null(fx->out);
		assert_int_equal(fx->backend.close_called, 1);
	}
	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_stream_lifecycle_case_t CASE_STREAM_LIFECYCLE_OOM = {
	.name = "stream_lifecycle_oom",
	.scenario = STREAM_LIFECYCLE_SCENARIO_OOM,
	.fail_call_idx = 1,
};

static const test_stream_lifecycle_case_t CASE_STREAM_LIFECYCLE_DESTROY_IDEMPOTENT = {
	.name = "stream_lifecycle_destroy_idempotent",
	.scenario = STREAM_LIFECYCLE_SCENARIO_DESTROY_IDEMPOTENT,
	.fail_call_idx = 0,
};

static const test_stream_lifecycle_case_t CASE_STREAM_LIFECYCLE_OK = {
	.name = "stream_lifecycle_ok",
	.scenario = STREAM_LIFECYCLE_SCENARIO_OK,
	.fail_call_idx = 0,
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define STREAM_LIFECYCLE_CASES(X) \
X(CASE_STREAM_LIFECYCLE_OOM) \
X(CASE_STREAM_LIFECYCLE_DESTROY_IDEMPOTENT) \
X(CASE_STREAM_LIFECYCLE_OK)

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
 * Doubles:
 * - stream backend defined in the beginning of this file
 * - fake_memory
 *
 * See contract:
 * - @ref specifications_stream_read "stream_read() specifications"
 *
 * See test description:
 * - @ref testing_foundation_stream_unit_stream_read "stream_read() unit tests section"
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
	test_stream_read_fixture_t *fx = (test_stream_read_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;
	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();

	// borrowed DI
	fx->stream_env.mem = osal_mem_test_fake_ops();
	fx->stream_env.vtbl = &fake_stream_vtbl;

	fx->fake_backend.close_ret = STREAM_STATUS_OK;

	assert_int_equal(
	    stream_create(&fx->stream, &fx->stream_env),
    	STREAM_STATUS_OK
	);

	// owned DI
	stream_inject_backend(fx->stream, &fx->fake_backend);

	// Explicitly complete the normal initialization lifecycle.
	assert_int_equal(
		stream_complete_default_init(fx->stream, NULL),
		STREAM_STATUS_OK
	);

	assert_int_equal(
		stream_create(&fx->stream_no_backend, &fx->stream_env),
		STREAM_STATUS_OK
	);

	/*
	 * Deliberately do not call `stream_complete_default_init()` in order to
	 * keep a partially initialized stream handle with no backend.
	 */

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

	osal_free(fx);

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
 * Doubles:
 * - stream backend defined in the beginning of this file
 *
 * See contract:
 * - @ref specifications_stream_write "stream_write() specifications"
 *
 * See test description:
 * - @ref testing_foundation_stream_unit_stream_write "stream_write() unit tests section"
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
	test_stream_write_fixture_t *fx = (test_stream_write_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;
	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();

	// borrowed DI
	fx->stream_env.mem = osal_mem_test_fake_ops();
	fx->stream_env.vtbl = &fake_stream_vtbl;

	fx->fake_backend.close_ret = STREAM_STATUS_OK;

	assert_int_equal(
	    stream_create(&fx->stream, &fx->stream_env),
    	STREAM_STATUS_OK
	);

	// owned DI
	stream_inject_backend(fx->stream, &fx->fake_backend);

	// Explicitly complete the normal initialization lifecycle.
	assert_int_equal(
		stream_complete_default_init(fx->stream, NULL),
		STREAM_STATUS_OK
	);

	assert_int_equal(
		stream_create(&fx->stream_no_backend, &fx->stream_env),
		STREAM_STATUS_OK
	);

	/*
	 * Deliberately do not call `stream_complete_default_init()` in order to
	 * keep a partially initialized stream handle with no backend.
	 */

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

	osal_free(fx);

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
 * Doubles:
 * - stream backend defined in the beginning of this file
 * - fake_memory
 *
 * See contract:
 * - @ref specifications_stream_flush "stream_flush() specifications"
 *
 * See test description:
 * - @ref testing_foundation_stream_unit_stream_flush "stream_flush() unit tests section"
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

    test_stream_flush_fixture_t *fx = (test_stream_flush_fixture_t *)osal_malloc(sizeof(*fx));
    if (!fx) return -1;

    osal_memset(fx, 0, sizeof(*fx));
    fx->tc = tc;

    fake_memory_reset();

	// borrowed DI
	fx->env.mem = osal_mem_test_fake_ops();
	fx->env.vtbl = &fake_stream_vtbl;

    fake_stream_backend_reset(&fx->backend);
    fx->backend.close_ret = STREAM_STATUS_OK;

	assert_int_equal(
		stream_create(&fx->stream, &fx->env),
		STREAM_STATUS_OK
	);

	// owned DI
	stream_inject_backend(fx->stream, &fx->backend);

	// Explicitly complete the normal initialization lifecycle.
	assert_int_equal(
		stream_complete_default_init(fx->stream, NULL),
		STREAM_STATUS_OK
	);

	assert_int_equal(
		stream_create(&fx->stream_no_backend, &fx->env),
		STREAM_STATUS_OK
	);

	/*
	 * Deliberately do not call `stream_complete_default_init()` in order to
	 * keep a partially initialized stream handle with no backend.
	 */

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

    osal_free(fx);
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
	static const struct CMUnitTest stream_tests_non_parametric[] = {
		cmocka_unit_test(test_stream_default_env)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(stream_tests_non_parametric, NULL, NULL);
	failed += cmocka_run_group_tests(stream_lifecycle_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stream_read_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stream_write_tests, NULL, NULL);
	failed += cmocka_run_group_tests(stream_flush_tests, NULL, NULL);

	return failed;
}

/** @endcond */
