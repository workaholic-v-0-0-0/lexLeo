/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file integration_test_lexleo_app_cr_no_double.c
 * @ingroup lexleo_app_integration_tests
 * @brief Integration tests for the LexLeo application Composition Root without test doubles.
 *
 * See contracts:
 * - @ref specifications_lexleo_app "lexleo_app specifications"
 *
 * See test description:
 * - @ref testing_lexleo_app_cr_integration_no_double "lexleo_app_cr.c integration tests without test doubles page"
 */

#include "lexleo_app/cr_opaque/lexleo_app_cr_opaque_api.h"

#include "lexleo_app/tests/lexleo_app_white_box_tests_access.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/stdio/osal_stdio_ops.h"
#include "osal/file/osal_file_ops.h"
#include "osal/str/osal_str_ops.h"
#include "osal/time/osal_time_ops.h"

#include "lexleo_cmocka.h"

/******************************************************************************************************************************************
 * @brief Test `lexleo_app_t` lifecycle.
 *
 * See contracts:
 * - @ref specifications_lexleo_app_create_init "lexleo_app_create_init() specification".
 * - @ref specifications_lexleo_app_destroy "lexleo_app_destroy() specification".
 *
 * See test description:
 * - @ref testing_lexleo_app_cr_integration_no_double "lexleo_app_cr.c integration tests without test doubles page"
 */
static void test_lexleo_app_cr_handle_lifecycle_no_double(void **state) {
	(void)state;

	// ARRANGE
	lexleo_app_t *app = NULL;
	lexleo_app_cfg_t cfg = lexleo_app_default_cfg();

	// ACT
	bool ret = lexleo_app_create_init(&app, &cfg);

	// ASSERT
	assert_true(ret);
	assert_non_null(app);
	assert_ptr_equal(lexleo_app_get_mem_ops(app), osal_mem_default_ops());
	assert_ptr_equal(lexleo_app_get_stdio_ops(app), osal_stdio_default_ops());
	assert_ptr_equal(lexleo_app_get_file_ops(app), osal_file_default_ops());
	assert_ptr_equal(lexleo_app_get_str_ops(app), osal_str_default_ops());
	assert_ptr_equal(lexleo_app_get_time_ops(app), osal_time_default_ops());
	for (size_t i = 0; i < LEXLEO_APP_MAX_INPUT_PATHS; ++i) {
		assert_null(lexleo_app_get_input_paths(app)[i]);
	}
	for (size_t i = 0; i < LEXLEO_APP_MAX_INPUT_DYN_LIB; ++i) {
		assert_null(lexleo_app_get_input_dynamic_libraries(app)[i]);
	}
	assert_null(lexleo_app_get_output_path(app));
	assert_int_equal(lexleo_app_get_action(app), LEXLEO_APP_ACTION_CONTROL_VM);
	assert_non_null(lexleo_app_get_in(app));
	assert_non_null(lexleo_app_get_out(app));
	assert_non_null(lexleo_app_get_err(app));
	assert_true(*lexleo_app_get_log_path(app) != '\0');
	assert_non_null(lexleo_app_get_logger_stream(app));
	assert_non_null(lexleo_app_get_logger(app));
	assert_non_null(lexleo_app_get_stream_owner_opaque(app));
	assert_non_null(lexleo_app_get_stream_factory(app));
	assert_non_null(lexleo_app_get_stream_standard_stream_creator(app));
	assert_non_null(lexleo_app_get_stream_regular_file_creator(app));
	assert_non_null(lexleo_app_get_stream_dynamic_buffer_creator(app));
	assert_ptr_equal(lexleo_app_get_vm_in(app), lexleo_app_get_in(app));
	assert_ptr_equal(lexleo_app_get_vm_out(app), lexleo_app_get_out(app));
	assert_non_null(lexleo_app_get_vm(app));

	// ACT
	lexleo_app_destroy(&app);

	// ASSERT
	assert_null(app);
}

//-----------------------------------------------------------------------------____________________________________________________________
// MAIN
//-----------------------------------------------------------------------------____________________________________________________________

/** @cond INTERNAL */
int main(void) {
	static const struct CMUnitTest lexleo_app_cr_tests_non_parametric[] = {
		cmocka_unit_test(test_lexleo_app_cr_handle_lifecycle_no_double),
	};

	int failed = 0;
	failed += cmocka_run_group_tests(lexleo_app_cr_tests_non_parametric, NULL, NULL);

	return failed;
}
/** @endcond */
