/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_app_white_box_tests_access_cr_wrapper.c
 * @ingroup lexleo_app_white_box_tests_access_cr_wrapper_group
 * @brief Test-only CR wrapper implementation for lexleo_app.
 *
 * @details
 * This file implements environment preparation and wrappers for application
 * creation and default initialization, with status translation for tests.
 */

#include "lexleo_app/tests/lexleo_app_white_box_tests_access_cr_wrapper.h"

#include "internal/lexleo_app_cr_internal.h"

#include "policy/lexleo_cstd_lib.h"

static lexleo_app_env_t g_lexleo_app_default_env_impl = {0};

lexleo_app_env_t *lexleo_app_test_default_env_p(
	const osal_mem_ops_t *mem_ops,
	const osal_stdio_ops_t *stdio_ops,
	const osal_file_ops_t *file_ops,
	const osal_str_ops_t *str_ops,
	const osal_time_ops_t *time_ops
) {
	g_lexleo_app_default_env_impl.mem_ops = mem_ops;
	g_lexleo_app_default_env_impl.stdio_ops = stdio_ops;
	g_lexleo_app_default_env_impl.file_ops = file_ops;
	g_lexleo_app_default_env_impl.str_ops = str_ops;
	g_lexleo_app_default_env_impl.time_ops = time_ops;
	return &g_lexleo_app_default_env_impl;
}

static lexleo_app_cr_test_status_t translate_status(
	lexleo_app_cr_status_t status
) {
	switch (status) {
#define MAKE_STATUS_CASE(name)             \
    case LEXLEO_APP_CR_STATUS_##name:       \
        return LEXLEO_APP_TEST_STATUS_##name;

		LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_CR_WRAPPER_STATUS_MAP(
			MAKE_STATUS_CASE)

#undef MAKE_STATUS_CASE
	}

	/* Unexpected internal status. */
	abort();
}

lexleo_app_cr_test_status_t lexleo_app_test_create(
	lexleo_app_t **out,
	const lexleo_app_env_t *env
) {
	return translate_status(lexleo_app_create(out, env));
}

lexleo_app_cr_test_status_t lexleo_app_test_complete_default_init(
	lexleo_app_t *app,
	const lexleo_app_cfg_t *cfg
) {
	return translate_status(lexleo_app_complete_default_init(app, cfg));
}
