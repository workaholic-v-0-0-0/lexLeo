/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_app_cr_internal.h
 * @ingroup lexleo_app_internal_group
 * @brief Internal Composition Root services for lexleo_app.
 *
 * @details
 * Declares internal status codes, the borrowed runtime environment,
 * and services for application creation and default initialization.
 */

#ifndef LEXLEO_LEXLEO_APP_CR_INTERNAL_H
#define LEXLEO_LEXLEO_APP_CR_INTERNAL_H

#include "lexleo_app/borrowers/lexleo_app_borrowers_types.h"
#include "lexleo_app/cr_opaque/lexleo_app_cr_opaque_types.h"

#include "osal/mem/osal_mem_types.h"
#include "osal/stdio/osal_stdio_types.h"
#include "osal/file/osal_file_types.h"
#include "osal/str/osal_str_types.h"
#include "osal/time/osal_time_types.h"

typedef enum lexleo_app_cr_status_t {
	LEXLEO_APP_CR_STATUS_OK,
	LEXLEO_APP_CR_STATUS_OOM,
	LEXLEO_APP_CR_STATUS_LOG_PATH_RESOLUTION_ERROR,
	LEXLEO_APP_CR_STATUS_STREAM_ERROR,
	LEXLEO_APP_CR_STATUS_INPUT_INIT_ERROR,
	LEXLEO_APP_CR_STATUS_OUTPUT_INIT_ERROR,
	LEXLEO_APP_CR_STATUS_ERR_INIT_ERROR,
	LEXLEO_APP_CR_STATUS_LOGGER_STREAM_INIT_ERROR,
	LEXLEO_APP_CR_STATUS_LOGGER_INIT_ERROR,
	LEXLEO_APP_CR_STATUS_VM_INIT_ERROR
} lexleo_app_cr_status_t;

typedef struct lexleo_app_env_t {
	const osal_mem_ops_t *mem_ops;
	const osal_stdio_ops_t *stdio_ops;
	const osal_file_ops_t *file_ops;
	const osal_str_ops_t *str_ops;
	const osal_time_ops_t *time_ops;
} lexleo_app_env_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Build a LexLeo application environment from borrowed dependencies.
 *
 * @param[in] mem_ops Borrowed memory operations.
 * @param[in] stdio_ops Borrowed standard I/O operations.
 * @param[in] file_ops Borrowed file operations.
 * @param[in] str_ops Borrowed string operations.
 * @param[in] time_ops Borrowed time operations.
 *
 * @return Environment value initialized from the provided dependencies.
 *
 * See contract:
 * - @ref specifications_lexleo_app_default_env
 */
lexleo_app_env_t lexleo_app_default_env(
	const osal_mem_ops_t *mem_ops,
	const osal_stdio_ops_t *stdio_ops,
	const osal_file_ops_t *file_ops,
	const osal_str_ops_t *str_ops,
	const osal_time_ops_t *time_ops);

/**
 * @brief Create a LexLeo application handle.
 *
 * @param[out] out Receives the created application handle on success.
 * @param[in] env Application borrowed environment.
 *
 * @return Creation status.
 *
 * @details
 * Allocates the handle and stores the borrowed dependencies provided by
 * `env`. Remaining initialization is completed separately through
 * `lexleo_app_complete_default_init()`.
 *
 * See contract:
 * - @ref specifications_lexleo_app_create
 */
lexleo_app_cr_status_t lexleo_app_create(
	lexleo_app_t **out,
	const lexleo_app_env_t *env);

/**
 * @brief Complete the default initialization of a LexLeo application handle.
 *
 * @param[in,out] app Application handle to complete.
 * @param[in] cfg Application configuration.
 *
 * @return Initialization status.
 *
 * @details
 * Initializes internal fields that were not injected during the preceding
 * construction phase, using `cfg` and the handle's borrowed dependencies.
 * Preserves previously initialized or injected fields.
 *
 * See contract:
 * - @ref specifications_lexleo_app_complete_default_init
 */
lexleo_app_cr_status_t lexleo_app_complete_default_init(
	lexleo_app_t *app,
	const lexleo_app_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LEXLEO_APP_CR_INTERNAL_H */
