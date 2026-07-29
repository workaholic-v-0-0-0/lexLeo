/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_app_white_box_tests_access_cr_wrapper.h
 * @ingroup lexleo_app_white_box_tests_access_cr_wrapper_group
 * @brief Test-only wrappers for internal LexLeo application CR services.
 *
 * @details
 * This header declares environment preparation and wrappers for application
 * creation and default initialization, with status translation for tests.
 */

#ifndef LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_CR_WRAPPER_H
#define LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_CR_WRAPPER_H

#include "lexleo_app/borrowers/lexleo_app_borrowers_types.h"
#include "lexleo_app/cr_opaque/lexleo_app_cr_opaque_types.h"

#include "osal/mem/osal_mem_types.h"
#include "osal/stdio/osal_stdio_types.h"
#include "osal/file/osal_file_types.h"
#include "osal/str/osal_str_types.h"
#include "osal/time/osal_time_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_CR_WRAPPER_STATUS_MAP(X) \
    X(OK)                            \
    X(OOM)                           \
    X(LOG_PATH_RESOLUTION_ERROR)     \
    X(STREAM_ERROR)                   \
    X(INPUT_INIT_ERROR)               \
    X(OUTPUT_INIT_ERROR)              \
    X(ERR_INIT_ERROR)                 \
    X(LOGGER_STREAM_INIT_ERROR)       \
    X(LOGGER_INIT_ERROR)              \
    X(VM_INIT_ERROR)

#define LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_CR_WRAPPER_MAKE_STATUS(name) LEXLEO_APP_TEST_STATUS_##name,

typedef enum {
	LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_CR_WRAPPER_STATUS_MAP(LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_CR_WRAPPER_MAKE_STATUS)
} lexleo_app_cr_test_status_t;

#undef LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_CR_WRAPPER_MAKE_STATUS

typedef struct lexleo_app_env_t lexleo_app_env_t;

lexleo_app_env_t *lexleo_app_test_default_env_p(
	const osal_mem_ops_t *mem_ops,
	const osal_stdio_ops_t *stdio_ops,
	const osal_file_ops_t *file_ops,
	const osal_str_ops_t *str_ops,
	const osal_time_ops_t *time_ops);

lexleo_app_cr_test_status_t lexleo_app_test_create(
	lexleo_app_t **out,
	const lexleo_app_env_t *env);

lexleo_app_cr_test_status_t lexleo_app_test_complete_default_init(
	lexleo_app_t *app,
	const lexleo_app_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_CR_WRAPPER_H */
