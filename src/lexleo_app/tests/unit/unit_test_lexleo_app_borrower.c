/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file unit_test_lexleo_app_borrower.c
 * @ingroup lexleo_app_unit_tests
 * @brief Unit tests implementation for lexleo_app_borrower.
 *
 * @details
 * This file implements the unit-level validation of lexleo_app_borrower.
 *
 * See also:
 * - @ref testing_foundation_lexleo_app_borrower_unit "lexleo_app_borrower.c unit tests page"
 * - @ref specifications_lexleo_app "lexleo_app specifications"
 */

#include "lexleo_app/borrower/lexleo_app_borrower_api.h"
#include "lexleo_app/cr_opaque/lexleo_app_cr_opaque_api.h"

#include "lexleo_app/tests/lexleo_app_white_box_tests_access_cr_wrapper.h"
#include "lexleo_app/tests/lexleo_app_white_box_tests_access.h"

#include "lexleo/test/fake_stream.h"

#include "lexleo/test/fake_logger.h"

#include "lexleo/test/fake_vm.h"

#include "osal/mem/osal_mem.h"
#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/test/osal_mem_fake_provider.h"
#include "osal/stdio/test/osal_stdio_fake_provider.h"
#include "osal/file/test/osal_file_fake_provider.h"
#include "osal/str/test/osal_str_fake_provider.h"
#include "osal/time/test/osal_time_fake_provider.h"

#include "policy/lexleo_cstd_types.h"
#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_jmp.h"

#include "lexleo_cmocka.h"

/**********************************************************************************************************************
 * @brief Test scenarios for `lexleo_app_read_input_line()`.
 *
 * See contract:
 * - @ref
 *
 * See test description:
 * - @ref testing_foundation_lexleo_app_borrower_unit_lexleo_app_read_input_line "`lexleo_app_read_input_line()` unit tests section"
 */
typedef enum {
	LEXLEO_APP_READ_INPUT_SCENARIO_ = 0,
	//<here>
} lexleo_app_read_input_line_scenario_t;

/** @cond INTERNAL */

typedef struct {
	const char *name;
	lexleo_app_read_input_line_scenario_t scenario;

	/* setup */
	size_t fake_in_next_read_ret;
	stream_status_t fake_in_next_read_st_value;
	uint8_t fake_in_buffered_backing[FAKE_STREAM_ADAPTER_BUF_SIZE];
	size_t fake_in_read_pos;
	bool arg_app_is_null;
	char *arg_message;
	char *arg_capacity;

	/* arrange */
	lexleo_app_status_t expected_ret;
	const char *expected_message;
} test_lexleo_app_read_input_line_case_t;

typedef struct {
	stream_t *fake_in;
	fake_stream_adapter_t fake_in_adapter;
	stream_t *fake_out;
	fake_stream_adapter_t fake_out_adapter;
	stream_t *fake_err;
	fake_stream_adapter_t fake_err_adapter;
	stream_t *fake_logger_stream;
	fake_stream_adapter_t fake_logger_stream_adapter;
	logger_t *fake_logger;
	fake_logger_adapter_t fake_logger_adapter;
	lexleo_vm_t *fake_vm;

	const test_lexleo_app_read_input_line_case_t *tc;
} test_lexleo_app_read_input_line_fixture_t;

static int setup_lexleo_app_read_input_line(void **state)
{
	LEXLEO_CMOCKA_INIT_SETUP(lexleo_app_read_input_line, state, tc, fx);

	fake_stream_reset();

	fake_stream_adapter_reset_instance(&fx->fake_in_adapter);
	fx->fake_in = fake_stream_create_instance(&fx->fake_in_adapter);
	fx->fake_in.next_read_ret = tc->fake_in_next_read_ret;
	fx->fake_in.next_read_st_value = tc->fake_in_next_read_st_value;
	void fake_stream_adapter_set_buffered_backing(
		fake_stream_adapter_t *fake_stream_adapter,
		const uint8_t *backing,
		size_t len);
	void fake_stream_adapter_set_read_pos(
		fake_stream_adapter_t *fake_stream_adapter,
		size_t n);

	fake_stream_adapter_reset_instance(&fx->fake_out_adapter);
	fx->fake_out = fake_stream_create_instance(&fx->fake_out_adapter);

	fake_stream_adapter_reset_instance(&fx->fake_err_adapter);
	fx->fake_err = fake_stream_create_instance(&fx->fake_err_adapter);

	fake_stream_adapter_reset_instance(&fx->fake_logger_stream_adapter);
	fx->fake_logger_stream = fake_stream_create_instance(&fx->fake_logger_stream_adapter);

	fake_logger_reset();

	fake_logger_adapter_reset_instance(&fx->fake_logger_adapter);
	fx->fake_logger = fake_logger_create_instance(&fx->fake_logger_adapter);

	fake_memory_reset();

	fake_str_reset();

	fake_vm_reset();
	fx->fake_vm = fake_vm_fake_to_real(&g_fake_vm);

	*state = fx;
	return 0;
}

static int teardown_lexleo_app_read_input_line(void **state)
{
	test_lexleo_app_read_input_line_fixture_t *fx = (test_lexleo_app_read_input_line_fixture_t *)(*state);
	osal_free(fx);
	return 0;
}

static void test_lexleo_app_read_input_line(void **state) {
	test_lexleo_app_read_input_line_fixture_t *fx = (test_lexleo_app_read_input_line_fixture_t *)(*state);
	const test_lexleo_app_read_input_line_case_t *tc = fx->tc;

	// ARRANGE


	// ACT


	// ASSERT

}

static const test_lexleo_app_read_input_line_case_t CASE_LEXLEO_APP_T_LIFECYCLE_LEXLEO_APP_HANDLE_OOM = {
	.name = "",
	.scenario = ,
};

#define LEXLEO_APP_READ_INPUT_CASES(X) \
X()

#define MAKE_LEXLEO_APP_READ_INPUT_TEST(case_sym) \
LEXLEO_MAKE_TEST(lexleo_app_read_input_line, case_sym)

static const struct CMUnitTest lexleo_app_read_input_line_tests[] = {
	LEXLEO_APP_READ_INPUT_CASES(MAKE_LEXLEO_APP_READ_INPUT_TEST)
};

#undef LEXLEO_APP_READ_INPUT_CASES
#undef MAKE_LEXLEO_APP_READ_INPUT_TEST

/** @endcond */

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	int failed = 0;
	failed += cmocka_run_group_tests(lexleo_app_read_input_line_tests, NULL, NULL);

	return failed;
}
/** @endcond */
