/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_osal_file.c
 * @ingroup osal_file_unit_tests
 * @brief Unit tests for the public `osal_file` operations table.
 *
 * @details
 * This file contains unit tests for:
 * - `osal_file_default_ops()`,
 * - `osal_file_ops_t::open()`,
 * - `osal_file_ops_t::read()`,
 * - `osal_file_ops_t::write()`,
 * - `osal_file_ops_t::flush()`,
 * - `osal_file_ops_t::close()`.
 *
 * The `osal_file_ops_t` operations are exercised through the default
 * operations table returned by `osal_file_default_ops()`.
 */

#include "osal/file/osal_file_ops.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/test/osal_mem_fake_provider.h"

#include "lexleo_cmocka.h"

/**
 * @brief Test `osal_file_default_ops()`.
 *
 * No double.
 *
 * See contract:
 * - @ref testing_foundation_osal_file_unit_default_ops "osal_file_default_ops() unit tests section"
 * - @ref specifications_osal_file_default_ops "osal_file_default_ops() specifications".
 */
static void test_osal_file_default_ops(void **state) {
	(void)state;

	const osal_file_ops_t *ret = osal_file_default_ops();

	assert_non_null(ret);
	assert_non_null(ret->open);
	assert_non_null(ret->read);
	assert_non_null(ret->write);
	assert_non_null(ret->flush);
	assert_non_null(ret->close);
}

/**
 * @brief Scenarios for `osal_file_ops_t::open()`.
 *
 * Doubles:
 * - fake_memory
 *
 * See contracts:
 * - @ref specifications_osal_file_open
 */
typedef enum {
	/**
	 * WHEN `osal_file_ops_t::open()` is called with `out == NULL`
	 * EXPECT:
	 * - returns `OSAL_FILE_STATUS_INVALID`
	 * - no `OSAL_FILE` handle is produced
	 */
	OSAL_FILE_OPEN_SCENARIO_OUT_NULL = 0,

	/**
	 * WHEN `osal_file_ops_t::open()` is called with `pathname == NULL`
	 * EXPECT:
	 * - returns `OSAL_FILE_STATUS_INVALID`
	 * - leaves `*out` unchanged if `out != NULL`
	 * - no `OSAL_FILE` handle is produced
	 */
	OSAL_FILE_OPEN_SCENARIO_PATHNAME_NULL,

	/**
	 * WHEN `osal_file_ops_t::open()` is called with `pathname[0] == '\0'`
	 * EXPECT:
	 * - returns `OSAL_FILE_STATUS_INVALID`
	 * - leaves `*out` unchanged if `out != NULL`
	 * - no `OSAL_FILE` handle is produced
	 */
	OSAL_FILE_OPEN_SCENARIO_PATHNAME_EMPTY,

	/**
	 * WHEN `osal_file_ops_t::open()` is called with `mode == NULL`
	 * EXPECT:
	 * - returns `OSAL_FILE_STATUS_INVALID`
	 * - leaves `*out` unchanged if `out != NULL`
	 * - no `OSAL_FILE` handle is produced
	 */
	OSAL_FILE_OPEN_SCENARIO_MODE_NULL,

	/**
	 * WHEN `osal_file_ops_t::open()` is called with an unsupported mode
	 * EXPECT:
	 * - returns `OSAL_FILE_STATUS_INVALID`
	 * - leaves `*out` unchanged if `out != NULL`
	 * - no `OSAL_FILE` handle is produced
	 */
	OSAL_FILE_OPEN_SCENARIO_MODE_UNSUPPORTED,

	/**
	 * WHEN `osal_file_ops_t::open()` is called with `mem_ops == NULL`
	 * EXPECT:
	 * - returns `OSAL_FILE_STATUS_INVALID`
	 * - leaves `*out` unchanged if `out != NULL`
	 * - no `OSAL_FILE` handle is produced
	 */
	OSAL_FILE_OPEN_SCENARIO_MEM_OPS_NULL,

	/**
	 * WHEN allocation of the OSAL wrapper fails during
	 * `osal_file_ops_t::open()`
	 * EXPECT:
	 * - returns `OSAL_FILE_STATUS_OOM`
	 * - leaves `*out` unchanged if `out != NULL`
	 * - no `OSAL_FILE` handle is produced
	 */
	OSAL_FILE_OPEN_SCENARIO_OOM,

	/**
	 * WHEN `osal_file_ops_t::open()` is called with valid arguments
	 * EXPECT:
	 * - returns `OSAL_FILE_STATUS_OK`
	 * - stores a non-NULL `OSAL_FILE` handle in `*out`
	 */
	OSAL_FILE_OPEN_SCENARIO_OK

} osal_file_open_scenario_t;

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
 * - `OUT_EXPECT_NON_NULL` means the output handle is expected to be
 *   non-`NULL` after the call.
 * - `OUT_EXPECT_UNCHANGED` means the output handle is expected to preserve
 *   its pre-call value.
 */
typedef enum {
	OUT_CHECK_NONE,
	OUT_EXPECT_NON_NULL,
	OUT_EXPECT_UNCHANGED
} out_expect_t;

/**
 * @brief One parametric test case for `osal_file_ops_t::open()`.
 *
 * Notes:
 * - `fail_call_idx` is used by `fake_memory` to inject an allocation failure
 *   on a specific call number.
 * - `expected_ret` is the expected returned value.
 * - `out_expect` expresses the expected postcondition on the fixture-managed
 *   output handle.
 */
typedef struct {
	const char *name;

	/* arrange */
	osal_file_open_scenario_t scenario;
	size_t fail_call_idx; /* 0 = no OOM, otherwise 1-based */

	/* assert */
	osal_file_status_t expected_ret;
	out_expect_t out_expect;
} test_osal_file_open_case_t;

/**
 * @brief Runtime fixture for `osal_file_ops_t::open()`.
 *
 * Holds:
 * - the output `OSAL_FILE` handle managed by the test,
 * - a snapshot of the pre-call output handle value,
 * - the default OSAL file operations table,
 * - the injected memory operations table,
 * - the input pathname and mode,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	/* output state */
	OSAL_FILE *out;
	OSAL_FILE *snapshot;

	/* DI */
	const osal_mem_ops_t *mem_ops;

	const char *pathname;
	const char *mode;

	const osal_file_ops_t *file_ops;

	/* active parametric test case */
	const test_osal_file_open_case_t *tc;
} test_osal_file_open_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `osal_file_ops_t::open()` tests.
 */
static int setup_osal_file_open(void **state)
{
	const test_osal_file_open_case_t *tc =
		(const test_osal_file_open_case_t *)(*state);

	test_osal_file_open_fixture_t *fx =
		(test_osal_file_open_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	fake_memory_reset();
	if (tc->scenario == OSAL_FILE_OPEN_SCENARIO_OOM && tc->fail_call_idx > 0) {
		fake_memory_fail_only_on_call(tc->fail_call_idx);
	}

	// DI
	fx->mem_ops = osal_mem_test_fake_ops();

	fx->file_ops = osal_file_default_ops();

	fx->pathname = "unit_test_osal_file.tmp";
	fx->mode = "wb";

	*state = fx;
	return 0;
}

/**
 * @brief Release the `osal_file_ops_t::open()` test fixture and verify memory invariants.
 */
static int teardown_osal_file_open(void **state)
{
	test_osal_file_open_fixture_t *fx =
		(test_osal_file_open_fixture_t *)(*state);

	if (fx->out && fx->out != fx->snapshot) {
		assert_true(
			fx->file_ops->close(fx->out) == OSAL_FILE_STATUS_OK
		);
		fx->out = NULL;
	}

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	osal_free(fx);
	*state = NULL;
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one parametric test scenario for `osal_file_ops_t::open()`.
 */
static void test_osal_file_open(void **state)
{
	test_osal_file_open_fixture_t *fx =
		(test_osal_file_open_fixture_t *)(*state);
	const test_osal_file_open_case_t *tc =
		(const test_osal_file_open_case_t *)fx->tc;

	// ARRANGE
	osal_file_status_t ret = (osal_file_status_t)-1; // poison
	fx->out = (OSAL_FILE *)(uintptr_t)0xDEADC0DEu;
	OSAL_FILE **out_arg = &fx->out;
	const char *pathname_arg = fx->pathname;
	const char *mode_arg = fx->mode;
	const osal_mem_ops_t *mem_ops_arg = fx->mem_ops;

	// invalid args
	if (tc->scenario == OSAL_FILE_OPEN_SCENARIO_OUT_NULL) {
		out_arg = NULL;
	}
	if (tc->scenario == OSAL_FILE_OPEN_SCENARIO_PATHNAME_NULL) {
		pathname_arg = NULL;
	}
	if (tc->scenario == OSAL_FILE_OPEN_SCENARIO_PATHNAME_EMPTY) {
		pathname_arg = "";
	}
	if (tc->scenario == OSAL_FILE_OPEN_SCENARIO_MODE_NULL) {
		mode_arg = NULL;
	}
	if (tc->scenario == OSAL_FILE_OPEN_SCENARIO_MODE_UNSUPPORTED) {
		mode_arg = "w+";
	}
	if (tc->scenario == OSAL_FILE_OPEN_SCENARIO_MEM_OPS_NULL) {
		mem_ops_arg = NULL;
	}

	fx->snapshot = fx->out;

	// ACT
	ret = fx->file_ops->open(out_arg, pathname_arg, mode_arg, mem_ops_arg);

	// ASSERT
	assert_int_equal(ret, tc->expected_ret);

	switch (tc->out_expect) {
		case OUT_CHECK_NONE:
			break;
		case OUT_EXPECT_NON_NULL:
			assert_non_null(fx->out);
			assert_ptr_not_equal(fx->out, fx->snapshot);
			break;
		case OUT_EXPECT_UNCHANGED:
			assert_ptr_equal(fx->out, fx->snapshot);
			break;
		default:
			fail();
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_osal_file_open_case_t CASE_OSAL_FILE_OPEN_OUT_NULL = {
	.name = "osal_file_open_out_null",
	.scenario = OSAL_FILE_OPEN_SCENARIO_OUT_NULL,
	.fail_call_idx = 0,

	.expected_ret = OSAL_FILE_STATUS_INVALID,
	.out_expect = OUT_CHECK_NONE
};

static const test_osal_file_open_case_t CASE_OSAL_FILE_OPEN_PATHNAME_NULL = {
	.name = "osal_file_open_pathname_null",
	.scenario = OSAL_FILE_OPEN_SCENARIO_PATHNAME_NULL,
	.fail_call_idx = 0,

	.expected_ret = OSAL_FILE_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_osal_file_open_case_t CASE_OSAL_FILE_OPEN_PATHNAME_EMPTY = {
	.name = "osal_file_open_pathname_empty",
	.scenario = OSAL_FILE_OPEN_SCENARIO_PATHNAME_EMPTY,
	.fail_call_idx = 0,

	.expected_ret = OSAL_FILE_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_osal_file_open_case_t CASE_OSAL_FILE_OPEN_MODE_NULL = {
	.name = "osal_file_open_mode_null",
	.scenario = OSAL_FILE_OPEN_SCENARIO_MODE_NULL,
	.fail_call_idx = 0,

	.expected_ret = OSAL_FILE_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_osal_file_open_case_t CASE_OSAL_FILE_OPEN_MODE_UNSUPPORTED = {
	.name = "osal_file_open_mode_unsupported",
	.scenario = OSAL_FILE_OPEN_SCENARIO_MODE_UNSUPPORTED,
	.fail_call_idx = 0,

	.expected_ret = OSAL_FILE_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_osal_file_open_case_t CASE_OSAL_FILE_OPEN_MEM_OPS_NULL = {
	.name = "osal_file_open_mem_ops_null",
	.scenario = OSAL_FILE_OPEN_SCENARIO_MEM_OPS_NULL,
	.fail_call_idx = 0,

	.expected_ret = OSAL_FILE_STATUS_INVALID,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_osal_file_open_case_t CASE_OSAL_FILE_OPEN_OOM_1 = {
	.name = "osal_file_open_oom_1",
	.scenario = OSAL_FILE_OPEN_SCENARIO_OOM,
	.fail_call_idx = 1,

	.expected_ret = OSAL_FILE_STATUS_OOM,
	.out_expect = OUT_EXPECT_UNCHANGED
};

static const test_osal_file_open_case_t CASE_OSAL_FILE_OPEN_OK = {
	.name = "osal_file_open_ok",
	.scenario = OSAL_FILE_OPEN_SCENARIO_OK,
	.fail_call_idx = 0,

	.expected_ret = OSAL_FILE_STATUS_OK,
	.out_expect = OUT_EXPECT_NON_NULL
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define OSAL_FILE_OPEN_CASES(X) \
X(CASE_OSAL_FILE_OPEN_OUT_NULL) \
X(CASE_OSAL_FILE_OPEN_PATHNAME_NULL) \
X(CASE_OSAL_FILE_OPEN_PATHNAME_EMPTY) \
X(CASE_OSAL_FILE_OPEN_MODE_NULL) \
X(CASE_OSAL_FILE_OPEN_MODE_UNSUPPORTED) \
X(CASE_OSAL_FILE_OPEN_MEM_OPS_NULL) \
X(CASE_OSAL_FILE_OPEN_OOM_1) \
X(CASE_OSAL_FILE_OPEN_OK)

#define OSAL_FILE_MAKE_OPEN_TEST(case_sym) \
LEXLEO_MAKE_TEST(osal_file_open, case_sym)

static const struct CMUnitTest osal_file_open_tests[] = {
	OSAL_FILE_OPEN_CASES(OSAL_FILE_MAKE_OPEN_TEST)
};

#undef OSAL_FILE_OPEN_CASES
#undef OSAL_FILE_MAKE_OPEN_TEST

/** @endcond */

/**
 * @brief Scenarios for `osal_file_ops_t::read()`.
 *
 * No doubles.
 *
 * See contract:
 * - @ref specifications_osal_file_read
 */
typedef enum {
	/**
	 * WHEN `osal_file_ops_t::read()` is called with `ptr == NULL`
	 * EXPECT:
	 * - returns `0`
	 * - sets `*st` to `OSAL_FILE_STATUS_INVALID` if `st != NULL`
	 */
	OSAL_FILE_READ_SCENARIO_PTR_NULL = 0,

	/**
	 * WHEN `osal_file_ops_t::read()` is called with `st == NULL`
	 * EXPECT:
	 * - returns `0`
	 * - no status is written
	 */
	OSAL_FILE_READ_SCENARIO_ST_NULL,

	/**
	 * WHEN `osal_file_ops_t::read()` is called with `stream == NULL`
	 * EXPECT:
	 * - returns `0`
	 * - sets `*st` to `OSAL_FILE_STATUS_INVALID` if `st != NULL`
	 */
	OSAL_FILE_READ_SCENARIO_STREAM_NULL,

	/**
	 * WHEN `osal_file_ops_t::read()` is called with valid arguments and the
	 * requested element count is smaller than the available file content
	 * EXPECT:
	 * - returns `nmemb`
	 * - copies the requested elements into `ptr`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 */
	OSAL_FILE_READ_SCENARIO_READ_LESS_THAN_AVAILABLE,

	/**
	 * WHEN `osal_file_ops_t::read()` is called with valid arguments and the
	 * requested element count exactly matches the available file content
	 * EXPECT:
	 * - returns `nmemb`
	 * - copies the requested elements into `ptr`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 */
	OSAL_FILE_READ_SCENARIO_READ_EXACT_AVAILABLE,

	/**
	 * WHEN `osal_file_ops_t::read()` is called with valid arguments and the
	 * requested element count is greater than the available file content
	 * EXPECT:
	 * - returns a value smaller than `nmemb`
	 * - copies the available elements into `ptr`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 */
	OSAL_FILE_READ_SCENARIO_READ_MORE_THAN_AVAILABLE,

	/**
	 * WHEN `osal_file_ops_t::read()` is called with valid arguments and the
	 * file is empty
	 * EXPECT:
	 * - returns `0`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 */
	OSAL_FILE_READ_SCENARIO_EMPTY_FILE,

	/**
	 * WHEN `osal_file_ops_t::read()` is called multiple times on the same open
	 * `OSAL_FILE`, first with a request smaller than the available file
	 * content, then again with another request smaller than the remaining file
	 * content
	 * EXPECT:
	 * - the first call returns the requested element count
	 * - the second call returns the requested element count
	 * - each call copies the expected data from the current file position into
	 *   `ptr`
	 * - each call sets `*st` to `OSAL_FILE_STATUS_OK`
	 */
	OSAL_FILE_READ_SCENARIO_SEQUENTIAL_READS_PARTIAL_THEN_PARTIAL,

	/**
	 * WHEN `osal_file_ops_t::read()` is called after end-of-file has already
	 * been reached on the same open `OSAL_FILE`
	 * EXPECT:
	 * - returns `0`
	 * - reads no additional data into `ptr`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 */
	OSAL_FILE_READ_SCENARIO_READ_AFTER_EOF,

	/**
	 * WHEN `osal_file_ops_t::read()` is called with valid arguments and
	 * `size == 0`
	 * EXPECT:
	 * - returns `0`
	 * - reads no data into `ptr`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 */
	OSAL_FILE_READ_SCENARIO_ZERO_SIZE,

	/**
	 * WHEN `osal_file_ops_t::read()` is called with valid arguments and
	 * `nmemb == 0`
	 * EXPECT:
	 * - returns `0`
	 * - reads no data into `ptr`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 */
	OSAL_FILE_READ_SCENARIO_ZERO_NMEMB,

	/**
	 * WHEN `osal_file_ops_t::read()` is called with valid arguments,
	 * `size > 1`, and the requested element count is smaller than the number of
	 * complete elements available in the file
	 * EXPECT:
	 * - returns `nmemb`
	 * - copies the requested complete elements into `ptr`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 */
	OSAL_FILE_READ_SCENARIO_READ_LESS_THAN_AVAILABLE_SIZE_GT_1,

	/**
	 * WHEN `osal_file_ops_t::read()` is called with valid arguments,
	 * `size > 1`, and the requested element count exactly matches the number of
	 * complete elements available in the file
	 * EXPECT:
	 * - returns `nmemb`
	 * - copies the requested complete elements into `ptr`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 */
	OSAL_FILE_READ_SCENARIO_READ_EXACT_AVAILABLE_SIZE_GT_1,

	/**
	 * WHEN `osal_file_ops_t::read()` is called with valid arguments,
	 * `size > 1`, and the requested element count is greater than the number of
	 * complete elements available in the file
	 * EXPECT:
	 * - returns a value smaller than `nmemb`
	 * - copies the available complete elements into `ptr`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 */
	OSAL_FILE_READ_SCENARIO_READ_MORE_THAN_AVAILABLE_SIZE_GT_1,

} osal_file_read_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `osal_file_ops_t::read()`.
 *
 * Notes:
 * - `file_content` is written before the tested read sequence starts.
 * - `do_second_read` enables an additional `read()` call on the same stream.
 */
typedef struct {
	const char *name;

	// arrange
	osal_file_read_scenario_t scenario;
	uint8_t file_content[16];
	size_t file_content_len;
	size_t first_size;
	size_t first_nmemb;
	bool do_second_read;
	size_t second_size;
	size_t second_nmemb;

	// assert
	size_t first_expected_ret;
	osal_file_status_t first_expected_st;
	size_t second_expected_ret;
	osal_file_status_t second_expected_st;
} test_osal_file_read_case_t;

/**
 * @brief Runtime fixture for `osal_file_ops_t::read()` tests.
 *
 * Holds:
 * - the readable stream used by the tested `read()` calls,
 * - the writable stream used to prepare the initial file content,
 * - the status storage used by the first and second tested calls,
 * - the read buffers used to capture data returned by the tested calls,
 * - the injected memory operations,
 * - the default OSAL file operations table used by the test,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	OSAL_FILE *readable_osal_file;
	OSAL_FILE *writable_osal_file;

	osal_file_status_t first_st_storage;
	osal_file_status_t second_st_storage;
	uint8_t first_read_content[16];
	uint8_t second_read_content[16];

	// injection
	const osal_mem_ops_t *mem_ops;

	// exercised public API
	const osal_file_ops_t *file_ops;

	const test_osal_file_read_case_t *tc;
} test_osal_file_read_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `osal_file_ops_t::read()` tests.
 */
static int setup_osal_file_read(void **state)
{
	const test_osal_file_read_case_t *tc =
		(const test_osal_file_read_case_t *)(*state);

	test_osal_file_read_fixture_t *fx =
		(test_osal_file_read_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	// DI
	fake_memory_reset();
	fx->mem_ops = osal_mem_test_fake_ops();

	// prepare tmp file
	fx->file_ops = osal_file_default_ops();
	assert_int_equal(
		fx->file_ops->open(
			&fx->writable_osal_file,
			"osal_file_read_tmp_file.txt",
			"wb",
			fx->mem_ops
		),
		OSAL_FILE_STATUS_OK
	);
	osal_file_status_t st;
	assert_int_equal(
		fx->file_ops->write(
			tc->file_content,
			1,
			tc->file_content_len,
			fx->writable_osal_file,
			&st
		),
		(size_t)tc->file_content_len
	);
	assert_int_equal(st, OSAL_FILE_STATUS_OK);
	assert_int_equal(
		fx->file_ops->close(
			fx->writable_osal_file
		),
		OSAL_FILE_STATUS_OK
	);
	fx->writable_osal_file = NULL;
	assert_int_equal(
		fx->file_ops->open(
			&fx->readable_osal_file,
			"osal_file_read_tmp_file.txt",
			"rb",
			fx->mem_ops
		),
		OSAL_FILE_STATUS_OK
	);

	*state = fx;
	return 0;
}

/**
 * @brief Release the `osal_file_ops_t::read()` test fixture and verify memory invariants.
 */
static int teardown_osal_file_read(void **state)
{
	test_osal_file_read_fixture_t *fx =
		(test_osal_file_read_fixture_t *)(*state);

	if (fx->readable_osal_file) {
		assert_true(
			fx->file_ops->close(fx->readable_osal_file) == OSAL_FILE_STATUS_OK
		);
		fx->readable_osal_file = NULL;
	}

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	osal_free(fx);
	*state = NULL;
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one parametric test scenario for `osal_file_ops_t::read()`.
 */
static void test_osal_file_read(void **state)
{
	test_osal_file_read_fixture_t *fx =
		(test_osal_file_read_fixture_t *)(*state);
	const test_osal_file_read_case_t *tc =
		(const test_osal_file_read_case_t *)fx->tc;

	// ARRANGE (first read call)
	size_t ret = (size_t)-1; // poison
	void *ptr_arg = fx->first_read_content;
	size_t size_arg = tc->first_size;
	size_t nmemb_arg = tc->first_nmemb;
	OSAL_FILE *stream_arg = fx->readable_osal_file;
	osal_file_status_t *st_arg = &fx->first_st_storage;

	// invalid args
	if (tc->scenario == OSAL_FILE_READ_SCENARIO_PTR_NULL) {
		ptr_arg = NULL;
	}
	if (tc->scenario == OSAL_FILE_READ_SCENARIO_ST_NULL) {
		st_arg = NULL;
	}
	if (tc->scenario == OSAL_FILE_READ_SCENARIO_STREAM_NULL) {
		stream_arg = NULL;
	}

	// ACT (first read call)
	ret = fx->file_ops->read(ptr_arg, size_arg, nmemb_arg, stream_arg, st_arg);

	// ASSERT (first read call)
	assert_int_equal(ret, tc->first_expected_ret);
	if (st_arg) {
		assert_int_equal(*st_arg, tc->first_expected_st);
	}
	if (tc->first_expected_ret > 0) {
		assert_memory_equal(
			fx->first_read_content,
			tc->file_content,
			(tc->first_expected_ret * tc->first_size <= tc->file_content_len) ?
				tc->first_expected_ret * tc->first_size
				:
				tc->file_content_len
		);
	}

	if (!tc->do_second_read) {
		return;
	}

	// ARRANGE (second read call)
	ret = (size_t)-1; // poison
	ptr_arg = fx->second_read_content;
	size_arg = tc->second_size;
	nmemb_arg = tc->second_nmemb;
	stream_arg = fx->readable_osal_file;
	st_arg = &fx->second_st_storage;

	// ACT (second read call)
	ret = fx->file_ops->read(ptr_arg, size_arg, nmemb_arg, stream_arg, st_arg);

	// ASSERT (second read call)
	assert_int_equal(ret, tc->second_expected_ret);
	if (st_arg) {
		assert_int_equal(*st_arg, tc->second_expected_st);
	}
	if (tc->second_expected_ret > 0) {
		assert_true(
			tc->file_content_len >= tc->first_expected_ret * tc->first_size
		);
		assert_memory_equal(
			fx->second_read_content,
			tc->file_content + tc->first_expected_ret * tc->first_size,
			(tc->second_expected_ret * tc->second_size + tc->first_expected_ret * tc->first_size <= tc->file_content_len) ?
				tc->second_expected_ret * tc->second_size : tc->file_content_len - tc->first_expected_ret * tc->first_size
		);
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_osal_file_read_case_t CASE_OSAL_FILE_READ_PTR_NULL = {
	.name = "osal_file_read_ptr_null",
	.scenario = OSAL_FILE_READ_SCENARIO_PTR_NULL,

	/* arrange */
	.file_content = "dummy content", /* '\0' is not considered */
	.file_content_len = 14,
	.first_size = 1,
	.first_nmemb = 3,
	.do_second_read = false,

	/* assert */
	.first_expected_ret = 0,
	.first_expected_st = OSAL_FILE_STATUS_INVALID,
};

static const test_osal_file_read_case_t CASE_OSAL_FILE_READ_ST_NULL = {
	.name = "osal_file_read_st_null",
	.scenario = OSAL_FILE_READ_SCENARIO_ST_NULL,

	/* arrange */
	.file_content = "dummy content",
	.file_content_len = 14,
	.first_size = 1,
	.first_nmemb = 3,
	.do_second_read = false,

	/* assert */
	.first_expected_ret = 0,
};

static const test_osal_file_read_case_t CASE_OSAL_FILE_READ_STREAM_NULL = {
	.name = "osal_file_read_stream_null",
	.scenario = OSAL_FILE_READ_SCENARIO_STREAM_NULL,

	/* arrange */
	.file_content = "dummy content",
	.file_content_len = 14,
	.first_size = 1,
	.first_nmemb = 3,
	.do_second_read = false,

	/* assert */
	.first_expected_ret = 0,
	.first_expected_st = OSAL_FILE_STATUS_INVALID,
};

static const test_osal_file_read_case_t CASE_OSAL_FILE_READ_LESS_THAN_AVAILABLE = {
	.name = "osal_file_read_less_than_available",
	.scenario = OSAL_FILE_READ_SCENARIO_READ_LESS_THAN_AVAILABLE,

	/* arrange */
	.file_content = "dummy content",
	.file_content_len = 14,
	.first_size = 1,
	.first_nmemb = 10,
	.do_second_read = false,

	/* assert */
	.first_expected_ret = 10,
	.first_expected_st = OSAL_FILE_STATUS_OK,
};

static const test_osal_file_read_case_t CASE_OSAL_FILE_READ_READ_EXACT_AVAILABLE = {
	.name = "osal_file_read_exact_available",
	.scenario = OSAL_FILE_READ_SCENARIO_READ_EXACT_AVAILABLE,

	/* arrange */
	.file_content = "dummy content",
	.file_content_len = 14,
	.first_size = 1,
	.first_nmemb = 14,
	.do_second_read = false,

	/* assert */
	.first_expected_ret = 14,
	.first_expected_st = OSAL_FILE_STATUS_OK,
};

static const test_osal_file_read_case_t CASE_OSAL_FILE_READ_READ_MORE_THAN_AVAILABLE = {
	.name = "osal_file_read_more_than_available",
	.scenario = OSAL_FILE_READ_SCENARIO_READ_MORE_THAN_AVAILABLE,

	/* arrange */
	.file_content = "dummy content",
	.file_content_len = 14,
	.first_size = 1,
	.first_nmemb = 17,
	.do_second_read = false,

	/* assert */
	.first_expected_ret = 14,
	.first_expected_st = OSAL_FILE_STATUS_OK,
};

static const test_osal_file_read_case_t CASE_OSAL_FILE_READ_EMPTY_FILE = {
	.name = "osal_file_read_empty_file",
	.scenario = OSAL_FILE_READ_SCENARIO_EMPTY_FILE,

	/* arrange */
	.file_content = "",
	.file_content_len = 0,
	.first_size = 1,
	.first_nmemb = 3,
	.do_second_read = false,

	/* assert */
	.first_expected_ret = 0,
	.first_expected_st = OSAL_FILE_STATUS_OK,
};

static const test_osal_file_read_case_t CASE_OSAL_FILE_READ_SEQUENTIAL_READS_PARTIAL_THEN_PARTIAL = {
	.name = "osal_file_read_sequential_reads_partial_then_partial",
	.scenario = OSAL_FILE_READ_SCENARIO_SEQUENTIAL_READS_PARTIAL_THEN_PARTIAL,

	/* arrange */
	.file_content = "dummy content",
	.file_content_len = 14,
	.first_size = 1,
	.first_nmemb = 6,
	.do_second_read = true,
	.second_size = 1,
	.second_nmemb = 4,

	/* assert */
	.first_expected_ret = 6,
	.first_expected_st = OSAL_FILE_STATUS_OK,
	.second_expected_ret = 4,
	.second_expected_st = OSAL_FILE_STATUS_OK,
};

static const test_osal_file_read_case_t CASE_OSAL_FILE_READ_AFTER_EOF = {
	.name = "osal_file_read_after_eof",
	.scenario = OSAL_FILE_READ_SCENARIO_READ_AFTER_EOF,

	/* arrange */
	.file_content = "dummy content",
	.file_content_len = 14,
	.first_size = 1,
	.first_nmemb = 14,
	.do_second_read = true,
	.second_size = 1,
	.second_nmemb = 1,

	/* assert */
	.first_expected_ret = 14,
	.first_expected_st = OSAL_FILE_STATUS_OK,
	.second_expected_ret = 0,
	.second_expected_st = OSAL_FILE_STATUS_OK,
};

static const test_osal_file_read_case_t CASE_OSAL_FILE_READ_READ_ZERO_SIZE = {
	.name = "osal_file_read_zero_size",
	.scenario = OSAL_FILE_READ_SCENARIO_ZERO_SIZE,

	/* arrange */
	.file_content = "dummy content",
	.file_content_len = 14,
	.first_size = 0,
	.first_nmemb = 14,
	.do_second_read = false,

	/* assert */
	.first_expected_ret = 0,
	.first_expected_st = OSAL_FILE_STATUS_OK,
};

static const test_osal_file_read_case_t CASE_OSAL_FILE_READ_READ_ZERO_NMEMB = {
	.name = "osal_file_read_zero_nmemb",
	.scenario = OSAL_FILE_READ_SCENARIO_ZERO_NMEMB,

	/* arrange */
	.file_content = "dummy content",
	.file_content_len = 14,
	.first_size = 1,
	.first_nmemb = 0,
	.do_second_read = false,

	/* assert */
	.first_expected_ret = 0,
	.first_expected_st = OSAL_FILE_STATUS_OK,
};

static const test_osal_file_read_case_t CASE_OSAL_FILE_READ_READ_LESS_THAN_AVAILABLE_SIZE_GT_1 = {
	.name = "osal_file_read_read_less_than_available_size_gt_1",
	.scenario = OSAL_FILE_READ_SCENARIO_READ_LESS_THAN_AVAILABLE_SIZE_GT_1,

	/* arrange */
	.file_content = "dummy content",
	.file_content_len = 14,
	.first_size = 3,
	.first_nmemb = 4,
	.do_second_read = false,

	/* assert */
	.first_expected_ret = 4,
	.first_expected_st = OSAL_FILE_STATUS_OK,
};

static const test_osal_file_read_case_t CASE_OSAL_FILE_READ_READ_EXACT_AVAILABLE_SIZE_GT_1 = {
	.name = "osal_file_read_read_exact_available_size_gt_1",
	.scenario = OSAL_FILE_READ_SCENARIO_READ_EXACT_AVAILABLE_SIZE_GT_1,

	/* arrange */
	.file_content = "dummy content",
	.file_content_len = 14,
	.first_size = 2,
	.first_nmemb = 7,
	.do_second_read = false,

	/* assert */
	.first_expected_ret = 7,
	.first_expected_st = OSAL_FILE_STATUS_OK,
};

static const test_osal_file_read_case_t CASE_OSAL_FILE_READ_READ_MORE_THAN_AVAILABLE_SIZE_GT_1 = {
	.name = "osal_file_read_more_than_available_size_gt_1",
	.scenario = OSAL_FILE_READ_SCENARIO_READ_MORE_THAN_AVAILABLE_SIZE_GT_1,

	/* arrange */
	.file_content = "dummy content",
	.file_content_len = 14,
	.first_size = 3,
	.first_nmemb = 5,
	.do_second_read = false,

	/* assert */
	.first_expected_ret = 4,
	.first_expected_st = OSAL_FILE_STATUS_OK,
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define OSAL_FILE_READ_CASES(X) \
X(CASE_OSAL_FILE_READ_PTR_NULL) \
X(CASE_OSAL_FILE_READ_ST_NULL) \
X(CASE_OSAL_FILE_READ_STREAM_NULL) \
X(CASE_OSAL_FILE_READ_LESS_THAN_AVAILABLE) \
X(CASE_OSAL_FILE_READ_READ_EXACT_AVAILABLE) \
X(CASE_OSAL_FILE_READ_READ_MORE_THAN_AVAILABLE) \
X(CASE_OSAL_FILE_READ_EMPTY_FILE) \
X(CASE_OSAL_FILE_READ_SEQUENTIAL_READS_PARTIAL_THEN_PARTIAL) \
X(CASE_OSAL_FILE_READ_AFTER_EOF) \
X(CASE_OSAL_FILE_READ_READ_ZERO_SIZE) \
X(CASE_OSAL_FILE_READ_READ_ZERO_NMEMB) \
X(CASE_OSAL_FILE_READ_READ_LESS_THAN_AVAILABLE_SIZE_GT_1) \
X(CASE_OSAL_FILE_READ_READ_EXACT_AVAILABLE_SIZE_GT_1) \
X(CASE_OSAL_FILE_READ_READ_MORE_THAN_AVAILABLE_SIZE_GT_1)

#define OSAL_FILE_MAKE_READ_TEST(case_sym) \
LEXLEO_MAKE_TEST(osal_file_read, case_sym)

static const struct CMUnitTest osal_file_read_tests[] = {
	OSAL_FILE_READ_CASES(OSAL_FILE_MAKE_READ_TEST)
};

#undef OSAL_FILE_READ_CASES
#undef OSAL_FILE_MAKE_READ_TEST

/** @endcond */

/**
 * @brief Scenarios for `osal_file_ops_t::write()`.
 *
 * No doubles.
 *
 * See contract:
 * - @ref specifications_osal_file_write
 */
typedef enum {
	/**
	 * WHEN `osal_file_ops_t::write()` is called with `ptr == NULL`
	 * EXPECT:
	 * - returns `0`
	 * - sets `*st` to `OSAL_FILE_STATUS_INVALID` if `st != NULL`
	 */
	OSAL_FILE_WRITE_SCENARIO_PTR_NULL = 0,

	/**
	 * WHEN `osal_file_ops_t::write()` is called with `stream == NULL`
	 * EXPECT:
	 * - returns `0`
	 * - sets `*st` to `OSAL_FILE_STATUS_INVALID` if `st != NULL`
	 */
	OSAL_FILE_WRITE_SCENARIO_STREAM_NULL,

	/**
	 * WHEN `osal_file_ops_t::write()` is called with `st == NULL`
	 * EXPECT:
	 * - returns `0`
	 * - no status can be stored
	 */
	OSAL_FILE_WRITE_SCENARIO_ST_NULL,

	/**
	 * WHEN `osal_file_ops_t::write()` is called with `size == 0`
	 * EXPECT:
	 * - returns `0`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 * - writes no data
	 */
	OSAL_FILE_WRITE_SCENARIO_ZERO_SIZE,

	/**
	 * WHEN `osal_file_ops_t::write()` is called with `nmemb == 0`
	 * EXPECT:
	 * - returns `0`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 * - writes no data
	 */
	OSAL_FILE_WRITE_SCENARIO_ZERO_NMEMB,

	/**
	 * WHEN `osal_file_ops_t::write()` is called with `nmemb == 1`
	 * on a valid writable stream
	 * EXPECT:
	 * - returns `1`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 * - writes the requested data
	 */
	OSAL_FILE_WRITE_SCENARIO_WRITE_ONE_ELEMENT_OK,

	/**
	 * WHEN `osal_file_ops_t::write()` is called with multiple elements of
	 * size `1` on a valid writable stream
	 * EXPECT:
	 * - returns `nmemb`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 * - writes the requested data
	 */
	OSAL_FILE_WRITE_SCENARIO_WRITE_MULTIPLE_ELEMENTS_SIZE_1_OK,

	/**
	 * WHEN `osal_file_ops_t::write()` is called with multiple elements of
	 * size greater than `1` on a valid writable stream
	 * EXPECT:
	 * - returns `nmemb`
	 * - sets `*st` to `OSAL_FILE_STATUS_OK`
	 * - writes the requested data
	 */
	OSAL_FILE_WRITE_SCENARIO_WRITE_MULTIPLE_ELEMENTS_SIZE_GT_1_OK,

	/**
	 * WHEN `osal_file_ops_t::write()` is called twice successively on the
	 * same valid writable stream
	 * EXPECT:
	 * - each call returns its requested element count
	 * - each call sets `*st` to `OSAL_FILE_STATUS_OK`
	 * - the final file content reflects both writes in order
	 */
	OSAL_FILE_WRITE_SCENARIO_SEQUENTIAL_WRITES_OK,
} osal_file_write_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `osal_file_ops_t::write()`.
 *
 * Notes:
 * - `initial_file_content` is written before the tested call sequence starts.
 * - `do_second_write` enables an additional `write()` call on the same stream.
 * - `expected_file_content` is the final file content expected after all
 *   tested writes complete.
 */
typedef struct {
	const char *name;

	// arrange
	osal_file_write_scenario_t scenario;
	uint8_t initial_file_content[16];
	size_t initial_file_content_len;
	const uint8_t first_write_content[16];
	size_t first_size;
	size_t first_nmemb;
	bool do_second_write;
	const uint8_t second_write_content[16];
	size_t second_size;
	size_t second_nmemb;

	// assert
	size_t first_expected_ret;
	osal_file_status_t first_expected_st;
	size_t second_expected_ret;
	osal_file_status_t second_expected_st;
	uint8_t expected_file_content[16];
	size_t expected_file_content_len;
} test_osal_file_write_case_t;

/**
 * @brief Runtime fixture for `osal_file_ops_t::write()` tests.
 *
 * Holds:
 * - the writable stream used by the tested `write()` calls,
 * - the verification stream used to read back the final file content,
 * - the status storage used by the first and second tested calls,
 * - the injected memory operations,
 * - the default OSAL file operations table used by the test,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	OSAL_FILE *writable_osal_file;
	OSAL_FILE *verification_osal_file;

	osal_file_status_t first_st_storage;
	osal_file_status_t second_st_storage;

	// injection
	const osal_mem_ops_t *mem_ops;

	// exercised public API
	const osal_file_ops_t *file_ops;

	const test_osal_file_write_case_t *tc;
} test_osal_file_write_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `osal_file_ops_t::write()` tests.
 */
static int setup_osal_file_write(void **state)
{
	const test_osal_file_write_case_t *tc =
		(const test_osal_file_write_case_t *)(*state);

	test_osal_file_write_fixture_t *fx =
		(test_osal_file_write_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	// DI
	fake_memory_reset();
	fx->mem_ops = osal_mem_test_fake_ops();

	// prepare tmp file
	fx->file_ops = osal_file_default_ops();
	assert_int_equal(
		fx->file_ops->open(
			&fx->writable_osal_file,
			"osal_file_write_tmp_file.txt",
			"wb",
			fx->mem_ops
		),
		OSAL_FILE_STATUS_OK
	);
	osal_file_status_t st;
	assert_int_equal(
		fx->file_ops->write(
			tc->initial_file_content,
			1,
			tc->initial_file_content_len,
			fx->writable_osal_file,
			&st
		),
		(size_t)tc->initial_file_content_len
	);
	assert_int_equal(st, OSAL_FILE_STATUS_OK);

	*state = fx;
	return 0;
}

/**
 * @brief Release the `osal_file_ops_t::write()` test fixture and verify memory invariants.
 */
static int teardown_osal_file_write(void **state)
{
	test_osal_file_write_fixture_t *fx =
		(test_osal_file_write_fixture_t *)(*state);

	if (fx->verification_osal_file) {
			assert_true(
				fx->file_ops->close(fx->verification_osal_file) == OSAL_FILE_STATUS_OK
			);
			fx->verification_osal_file = NULL;
	}

	if (fx->writable_osal_file) {
			assert_true(
				fx->file_ops->close(fx->writable_osal_file) == OSAL_FILE_STATUS_OK
			);
			fx->writable_osal_file = NULL;
	}

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	osal_free(fx);
	*state = NULL;
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one parametric test scenario for `osal_file_ops_t::write()`.
 */
static void test_osal_file_write(void **state)
{
	test_osal_file_write_fixture_t *fx =
		(test_osal_file_write_fixture_t *)(*state);
	const test_osal_file_write_case_t *tc =
		(const test_osal_file_write_case_t *)fx->tc;

	// ARRANGE (first write call)
	size_t first_write_ret = (size_t)-1; // poison
	fx->first_st_storage = (osal_file_status_t)-1;
	const void *ptr_arg = tc->first_write_content;
	size_t size_arg = tc->first_size;
	size_t nmemb_arg = tc->first_nmemb;
	OSAL_FILE *stream_arg = fx->writable_osal_file;
	osal_file_status_t *first_write_st_arg = &fx->first_st_storage;

	// invalid args
	if (tc->scenario == OSAL_FILE_WRITE_SCENARIO_PTR_NULL) {
		ptr_arg = NULL;
	}
	if (tc->scenario == OSAL_FILE_WRITE_SCENARIO_STREAM_NULL) {
		stream_arg = NULL;
	}
	if (tc->scenario == OSAL_FILE_WRITE_SCENARIO_ST_NULL) {
		first_write_st_arg = NULL;
	}

	// ACT (first write call)
	first_write_ret = fx->file_ops->write(ptr_arg, size_arg, nmemb_arg, stream_arg, first_write_st_arg);

	// ARRANGE (second write call)
	osal_file_status_t *second_write_st_arg = NULL;
	size_t second_write_ret = (size_t)-1; // poison
	if (tc->do_second_write) {
		fx->second_st_storage = (osal_file_status_t)-1;
		ptr_arg = tc->second_write_content;
		size_arg = tc->second_size;
		nmemb_arg = tc->second_nmemb;
		stream_arg = fx->writable_osal_file;
		second_write_st_arg = &fx->second_st_storage;

		// ACT (second write call)
		second_write_ret = fx->file_ops->write(ptr_arg, size_arg, nmemb_arg, stream_arg, second_write_st_arg);
	}

	// ASSERT
	assert_true(first_write_ret == tc->first_expected_ret);
	if (first_write_st_arg) {
		assert_int_equal(*first_write_st_arg, tc->first_expected_st);
	}
	if (tc->do_second_write) {
		assert_true(second_write_ret == tc->second_expected_ret);
		if (second_write_st_arg) {
			assert_int_equal(*second_write_st_arg, tc->second_expected_st);
		}
	}

	size_t total_len =
		tc->initial_file_content_len
		+
		tc->first_expected_ret * tc->first_size
		+
		((tc->do_second_write) ? tc->second_expected_ret * tc->second_size : 0);
	assert_true(
		tc->expected_file_content_len
		==
		total_len
	);
	assert_true(
		fx->file_ops->close(fx->writable_osal_file)
		==
		OSAL_FILE_STATUS_OK
	);

	fx->writable_osal_file = NULL;

	if (tc->expected_file_content_len > 0) {

		assert_int_equal(
			fx->file_ops->open(
				&fx->verification_osal_file,
				"osal_file_write_tmp_file.txt",
				"rb",
				fx->mem_ops
			),
			OSAL_FILE_STATUS_OK
		);
		uint8_t read_buf[16] = { 0 };
		osal_file_status_t st;
		assert_true(
			fx->file_ops->read(read_buf, 1, tc->expected_file_content_len, fx->verification_osal_file, &st)
			==
			tc->expected_file_content_len
		);
		assert_int_equal(st, OSAL_FILE_STATUS_OK);
		assert_memory_equal(
			read_buf,
			tc->expected_file_content,
			tc->expected_file_content_len
		);
		assert_int_equal(
			fx->file_ops->close(
				fx->verification_osal_file
			),
			OSAL_FILE_STATUS_OK
		);
		fx->verification_osal_file = NULL;
	}
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_osal_file_write_case_t CASE_OSAL_FILE_WRITE_PTR_NULL = {
	.name = "osal_file_write_ptr_null",
	.scenario = OSAL_FILE_WRITE_SCENARIO_PTR_NULL,

	.initial_file_content = "abc",
	.initial_file_content_len = 3,
	.first_size = 1,
	.first_nmemb = 5,
	.do_second_write = false,

	.first_expected_ret = 0,
	.first_expected_st = OSAL_FILE_STATUS_INVALID,
	.expected_file_content = "abc",
	.expected_file_content_len = 3
};

static const test_osal_file_write_case_t CASE_OSAL_FILE_WRITE_STREAM_NULL = {
	.name = "osal_file_write_stream_null",
	.scenario = OSAL_FILE_WRITE_SCENARIO_STREAM_NULL,

	.initial_file_content = "abc",
	.initial_file_content_len = 3,
	.first_write_content = "first",
	.first_size = 1,
	.first_nmemb = 5,
	.do_second_write = false,

	.first_expected_ret = 0,
	.first_expected_st = OSAL_FILE_STATUS_INVALID,
	.expected_file_content = "abc",
	.expected_file_content_len = 3
};

static const test_osal_file_write_case_t CASE_OSAL_FILE_WRITE_ST_NULL = {
	.name = "osal_file_write_st_null",
	.scenario = OSAL_FILE_WRITE_SCENARIO_ST_NULL,

	.initial_file_content = "abc",
	.initial_file_content_len = 3,
	.first_write_content = "first",
	.first_size = 1,
	.first_nmemb = 5,
	.do_second_write = false,

	.first_expected_ret = 0,
	.expected_file_content = "abc",
	.expected_file_content_len = 3
};

static const test_osal_file_write_case_t CASE_OSAL_FILE_WRITE_ZERO_SIZE = {
	.name = "osal_file_write_zero_size",
	.scenario = OSAL_FILE_WRITE_SCENARIO_ZERO_SIZE,

	.initial_file_content = "abc",
	.initial_file_content_len = 3,
	.first_write_content = "first",
	.first_size = 0,
	.first_nmemb = 5,
	.do_second_write = false,

	.first_expected_ret = 0,
	.first_expected_st = OSAL_FILE_STATUS_OK,
	.expected_file_content = "abc",
	.expected_file_content_len = 3
};

static const test_osal_file_write_case_t CASE_OSAL_FILE_WRITE_ZERO_NMEMB = {
	.name = "osal_file_write_zero_nmemb",
	.scenario = OSAL_FILE_WRITE_SCENARIO_ZERO_NMEMB,

	.initial_file_content = "abc",
	.initial_file_content_len = 3,
	.first_write_content = "first",
	.first_size = 1,
	.first_nmemb = 0,
	.do_second_write = false,

	.first_expected_ret = 0,
	.first_expected_st = OSAL_FILE_STATUS_OK,
	.expected_file_content = "abc",
	.expected_file_content_len = 3
};

static const test_osal_file_write_case_t CASE_OSAL_FILE_WRITE_ONE_ELEMENT_OK = {
	.name = "osal_file_write_one_element_ok",
	.scenario = OSAL_FILE_WRITE_SCENARIO_WRITE_ONE_ELEMENT_OK,

	.initial_file_content = "abc",
	.initial_file_content_len = 3,
	.first_write_content = "first",
	.first_size = 2,
	.first_nmemb = 1,
	.do_second_write = false,

	.first_expected_ret = 1,
	.first_expected_st = OSAL_FILE_STATUS_OK,
	.expected_file_content = "abcfi",
	.expected_file_content_len = 5
};

static const test_osal_file_write_case_t CASE_OSAL_FILE_WRITE_MULTIPLE_ELEMENTS_SIZE_1_OK = {
	.name = "osal_file_write_multiple_elements_size_1_ok",
	.scenario = OSAL_FILE_WRITE_SCENARIO_WRITE_MULTIPLE_ELEMENTS_SIZE_1_OK,

	.initial_file_content = "abc",
	.initial_file_content_len = 3,
	.first_write_content = "first",
	.first_size = 1,
	.first_nmemb = 4,
	.do_second_write = false,

	.first_expected_ret = 4,
	.first_expected_st = OSAL_FILE_STATUS_OK,
	.expected_file_content = "abcfirs",
	.expected_file_content_len = 7
};

static const test_osal_file_write_case_t CASE_OSAL_FILE_WRITE_MULTIPLE_ELEMENTS_SIZE_GT_1_OK = {
	.name = "osal_file_write_multiple_elements_size_gt_1_ok",
	.scenario = OSAL_FILE_WRITE_SCENARIO_WRITE_MULTIPLE_ELEMENTS_SIZE_GT_1_OK,

	.initial_file_content = "abc",
	.initial_file_content_len = 3,
	.first_write_content = "first",
	.first_size = 2,
	.first_nmemb = 2,
	.do_second_write = false,

	.first_expected_ret = 2,
	.first_expected_st = OSAL_FILE_STATUS_OK,
	.expected_file_content = "abcfirs",
	.expected_file_content_len = 7
};

static const test_osal_file_write_case_t CASE_OSAL_FILE_WRITE_SEQUENTIAL_WRITES_OK = {
	.name = "osal_file_write_sequential_writes_ok",
	.scenario = OSAL_FILE_WRITE_SCENARIO_SEQUENTIAL_WRITES_OK,

	.initial_file_content = "abc",
	.initial_file_content_len = 3,
	.first_write_content = "first",
	.first_size = 1,
	.first_nmemb = 5,
	.do_second_write = true,
	.second_write_content = "second",
	.second_size = 2,
	.second_nmemb = 3,

	.first_expected_ret = 5,
	.first_expected_st = OSAL_FILE_STATUS_OK,
	.second_expected_ret = 3,
	.second_expected_st = OSAL_FILE_STATUS_OK,
	.expected_file_content = "abcfirstsecond",
	.expected_file_content_len = 14
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define OSAL_FILE_WRITE_CASES(X) \
X(CASE_OSAL_FILE_WRITE_PTR_NULL) \
X(CASE_OSAL_FILE_WRITE_STREAM_NULL) \
X(CASE_OSAL_FILE_WRITE_ST_NULL) \
X(CASE_OSAL_FILE_WRITE_ZERO_SIZE) \
X(CASE_OSAL_FILE_WRITE_ZERO_NMEMB) \
X(CASE_OSAL_FILE_WRITE_ONE_ELEMENT_OK) \
X(CASE_OSAL_FILE_WRITE_MULTIPLE_ELEMENTS_SIZE_1_OK) \
X(CASE_OSAL_FILE_WRITE_MULTIPLE_ELEMENTS_SIZE_GT_1_OK) \
X(CASE_OSAL_FILE_WRITE_SEQUENTIAL_WRITES_OK)

#define OSAL_FILE_MAKE_WRITE_TEST(case_sym) \
LEXLEO_MAKE_TEST(osal_file_write, case_sym)

static const struct CMUnitTest osal_file_write_tests[] = {
	OSAL_FILE_WRITE_CASES(OSAL_FILE_MAKE_WRITE_TEST)
};

#undef OSAL_FILE_WRITE_CASES
#undef OSAL_FILE_MAKE_WRITE_TEST

/** @endcond */

/**
 * @brief Scenarios for `osal_file_ops_t::flush()`.
 *
 * No doubles.
 *
 * See contract:
 * - @ref specifications_osal_file_flush
 */
typedef enum {
	/**
	 * WHEN `osal_file_ops_t::flush()` is called with `stream == NULL`
	 * EXPECT:
	 * - returns `OSAL_FILE_STATUS_INVALID`
	 */
	OSAL_FILE_FLUSH_SCENARIO_STREAM_NULL = 0,

	/**
	 * WHEN `osal_file_ops_t::flush()` is called on a valid writable stream
	 * EXPECT:
	 * - returns `OSAL_FILE_STATUS_OK`
	 */
	OSAL_FILE_FLUSH_SCENARIO_OK,
} osal_file_flush_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `osal_file_ops_t::flush()`.
 *
 * Notes:
 * - `file_content` is written before the tested `flush()` call when the
 *   scenario uses a valid writable stream.
 */
typedef struct {
	const char *name;

	// arrange
	osal_file_flush_scenario_t scenario;
	uint8_t file_content[16];
	size_t file_content_len;

	// assert
	osal_file_status_t expected_ret;
} test_osal_file_flush_case_t;

/**
 * @brief Runtime fixture for `osal_file_ops_t::flush()` tests.
 *
 * Holds:
 * - the writable stream used by the tested `flush()` call,
 * - the injected memory operations,
 * - the default OSAL file operations table used by the test,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	OSAL_FILE *writable_osal_file;

	// injection
	const osal_mem_ops_t *mem_ops;

	// exercised public operations table
	const osal_file_ops_t *file_ops;

	const test_osal_file_flush_case_t *tc;
} test_osal_file_flush_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `osal_file_ops_t::flush()` tests.
 */
static int setup_osal_file_flush(void **state)
{
	const test_osal_file_flush_case_t *tc =
		(const test_osal_file_flush_case_t *)(*state);

	test_osal_file_flush_fixture_t *fx =
		(test_osal_file_flush_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	// DI
	fake_memory_reset();
	fx->mem_ops = osal_mem_test_fake_ops();

	// prepare tmp file
	fx->file_ops = osal_file_default_ops();
	assert_int_equal(
		fx->file_ops->open(
			&fx->writable_osal_file,
			"osal_file_flush_tmp_file.txt",
			"wb",
			fx->mem_ops
		),
		OSAL_FILE_STATUS_OK
	);
	osal_file_status_t st;
	assert_true(
		fx->file_ops->write(
			tc->file_content,
			1,
			tc->file_content_len,
			fx->writable_osal_file,
			&st)
		==
		tc->file_content_len
	);
	assert_int_equal(st, OSAL_FILE_STATUS_OK);

	*state = fx;
	return 0;
}

/**
 * @brief Release the `osal_file_ops_t::flush()` test fixture and verify memory invariants.
 */
static int teardown_osal_file_flush(void **state)
{
	test_osal_file_flush_fixture_t *fx =
		(test_osal_file_flush_fixture_t *)(*state);

	if (fx->writable_osal_file) {
			assert_true(
				fx->file_ops->close(fx->writable_osal_file) == OSAL_FILE_STATUS_OK
			);
			fx->writable_osal_file = NULL;
	}

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	osal_free(fx);
	*state = NULL;
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one parametric test scenario for `osal_file_ops_t::flush()`.
 */
static void test_osal_file_flush(void **state)
{
	test_osal_file_flush_fixture_t *fx =
		(test_osal_file_flush_fixture_t *)(*state);
	const test_osal_file_flush_case_t *tc =
		(const test_osal_file_flush_case_t *)fx->tc;

	// ARRANGE
	OSAL_FILE *stream_arg = fx->writable_osal_file;

	// invalid args
	if (tc->scenario == OSAL_FILE_FLUSH_SCENARIO_STREAM_NULL) {
		stream_arg = NULL;
	}

	// ACT
	osal_file_status_t ret = fx->file_ops->flush(stream_arg);


	// ASSERT
	assert_int_equal(ret, tc->expected_ret);
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_osal_file_flush_case_t CASE_OSAL_FILE_FLUSH_STREAM_NULL = {
	.name = "osal_file_flush_stream_null",
	.scenario = OSAL_FILE_FLUSH_SCENARIO_STREAM_NULL,

	.file_content = "abc",
	.file_content_len = 3,

	.expected_ret = OSAL_FILE_STATUS_INVALID
};

static const test_osal_file_flush_case_t CASE_OSAL_FILE_FLUSH_OK = {
	.name = "osal_file_flush_ok",
	.scenario = OSAL_FILE_FLUSH_SCENARIO_OK,

	.file_content = "abc",
	.file_content_len = 3,

	.expected_ret = OSAL_FILE_STATUS_OK
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define OSAL_FILE_FLUSH_CASES(X) \
X(CASE_OSAL_FILE_FLUSH_STREAM_NULL) \
X(CASE_OSAL_FILE_FLUSH_OK)

#define OSAL_FILE_MAKE_FLUSH_TEST(case_sym) \
LEXLEO_MAKE_TEST(osal_file_flush, case_sym)

static const struct CMUnitTest osal_file_flush_tests[] = {
	OSAL_FILE_FLUSH_CASES(OSAL_FILE_MAKE_FLUSH_TEST)
};

#undef OSAL_FILE_FLUSH_CASES
#undef OSAL_FILE_MAKE_FLUSH_TEST

/** @endcond */

/**
 * @brief Scenarios for `osal_file_ops_t::close()`.
 *
 * No doubles.
 *
 * See contract:
 * - @ref specifications_osal_file_close
 */
typedef enum {
	/**
	 * WHEN `osal_file_ops_t::close()` is called with `stream == NULL`
	 * EXPECT:
	 * - returns `OSAL_FILE_STATUS_INVALID`
	 */
	OSAL_FILE_CLOSE_SCENARIO_STREAM_NULL = 0,

	/**
	 * WHEN `osal_file_ops_t::close()` is called on a valid open stream
	 * EXPECT:
	 * - returns `OSAL_FILE_STATUS_OK`
	 */
	OSAL_FILE_CLOSE_SCENARIO_OK,
} osal_file_close_scenario_t;

/** @cond INTERNAL */

/**
 * @brief One parametric test case for `osal_file_ops_t::close()`.
 */
typedef struct {
	const char *name;

	// arrange
	osal_file_close_scenario_t scenario;

	// assert
	osal_file_status_t expected_ret;
} test_osal_file_close_case_t;

/**
 * @brief Runtime fixture for `osal_file_ops_t::close()` tests.
 *
 * Holds:
 * - the stream used by the tested `close()` call,
 * - the injected memory operations,
 * - the default OSAL file operations table used by the test,
 * - a pointer to the active parametric test case.
 */
typedef struct {
	// runtime resources
	OSAL_FILE *osal_file;

	// injection
	const osal_mem_ops_t *mem_ops;

	// exercised public operations table
	const osal_file_ops_t *file_ops;

	const test_osal_file_close_case_t *tc;
} test_osal_file_close_fixture_t;

//-----------------------------------------------------------------------------
// FIXTURES
//-----------------------------------------------------------------------------

/**
 * @brief Allocate and initialize the runtime fixture for `osal_file_ops_t::close()` tests.
 */
static int setup_osal_file_close(void **state)
{
	const test_osal_file_close_case_t *tc =
		(const test_osal_file_close_case_t *)(*state);

	test_osal_file_close_fixture_t *fx =
		(test_osal_file_close_fixture_t *)osal_malloc(sizeof(*fx));
	if (!fx) return -1;

	osal_memset(fx, 0, sizeof(*fx));
	fx->tc = tc;

	// DI
	fake_memory_reset();
	fx->mem_ops = osal_mem_test_fake_ops();

	// prepare tmp file
	fx->file_ops = osal_file_default_ops();
	assert_int_equal(
		fx->file_ops->open(
			&fx->osal_file,
			"osal_file_close_tmp_file.txt",
			"wb",
			fx->mem_ops
		),
		OSAL_FILE_STATUS_OK
	);

	*state = fx;
	return 0;
}

/**
 * @brief Release the `osal_file_ops_t::close()` test fixture and verify memory invariants.
 */
static int teardown_osal_file_close(void **state)
{
	test_osal_file_close_fixture_t *fx =
		(test_osal_file_close_fixture_t *)(*state);

	if (fx->osal_file) {
		assert_int_equal(
			fx->file_ops->close(fx->osal_file),
			OSAL_FILE_STATUS_OK
		);
		fx->osal_file = NULL;
	}

	assert_true(fake_memory_no_leak());
	assert_true(fake_memory_no_invalid_free());
	assert_true(fake_memory_no_double_free());

	osal_free(fx);
	*state = NULL;
	return 0;
}

//-----------------------------------------------------------------------------
// TEST
//-----------------------------------------------------------------------------

/**
 * @brief Execute one parametric test scenario for `osal_file_ops_t::close()`.
 */
static void test_osal_file_close(void **state)
{
	test_osal_file_close_fixture_t *fx =
		(test_osal_file_close_fixture_t *)(*state);
	const test_osal_file_close_case_t *tc =
		(const test_osal_file_close_case_t *)fx->tc;

	// ARRANGE
	OSAL_FILE *stream_arg = fx->osal_file;

	// invalid args
	if (tc->scenario == OSAL_FILE_CLOSE_SCENARIO_STREAM_NULL) {
		stream_arg = NULL;
	}

	// ACT
	osal_file_status_t ret = fx->file_ops->close(stream_arg);
	if (ret == OSAL_FILE_STATUS_OK && stream_arg == fx->osal_file) {
		fx->osal_file = NULL;
	}

	// ASSERT
	assert_int_equal(ret, tc->expected_ret);
}

//-----------------------------------------------------------------------------
// CASES
//-----------------------------------------------------------------------------

static const test_osal_file_close_case_t CASE_OSAL_FILE_CLOSE_STREAM_NULL = {
	.name = "osal_file_close_stream_null",
	.scenario = OSAL_FILE_CLOSE_SCENARIO_STREAM_NULL,

	.expected_ret = OSAL_FILE_STATUS_INVALID
};

static const test_osal_file_close_case_t CASE_OSAL_FILE_CLOSE_OK = {
	.name = "osal_file_close_ok",
	.scenario = OSAL_FILE_CLOSE_SCENARIO_OK,

	.expected_ret = OSAL_FILE_STATUS_OK
};

//-----------------------------------------------------------------------------
// CASES REGISTRY
//-----------------------------------------------------------------------------

#define OSAL_FILE_CLOSE_CASES(X) \
X(CASE_OSAL_FILE_CLOSE_STREAM_NULL) \
X(CASE_OSAL_FILE_CLOSE_OK)

#define OSAL_FILE_MAKE_CLOSE_TEST(case_sym) \
LEXLEO_MAKE_TEST(osal_file_close, case_sym)

static const struct CMUnitTest osal_file_close_tests[] = {
	OSAL_FILE_CLOSE_CASES(OSAL_FILE_MAKE_CLOSE_TEST)
};

#undef OSAL_FILE_CLOSE_CASES
#undef OSAL_FILE_MAKE_CLOSE_TEST

/** @endcond */

/** @cond INTERNAL */

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
	static const struct CMUnitTest osal_file_non_parametric_tests[] = {
		cmocka_unit_test(test_osal_file_default_ops)
	};

	int failed = 0;
	failed += cmocka_run_group_tests(osal_file_non_parametric_tests, NULL, NULL);
	failed += cmocka_run_group_tests(osal_file_open_tests, NULL, NULL);
	failed += cmocka_run_group_tests(osal_file_read_tests, NULL, NULL);
	failed += cmocka_run_group_tests(osal_file_write_tests, NULL, NULL);
	failed += cmocka_run_group_tests(osal_file_flush_tests, NULL, NULL);
	failed += cmocka_run_group_tests(osal_file_close_tests, NULL, NULL);

	return failed;
}

/** @endcond */
