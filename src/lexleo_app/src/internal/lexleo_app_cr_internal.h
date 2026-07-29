

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
	// ...
} lexleo_app_cr_status_t;

typedef struct lexleo_app_env_t {
	const osal_mem_ops_t *mem_ops;
	const osal_stdio_ops_t *stdio_ops;
	const osal_file_ops_t *file_ops;
	const osal_str_ops_t *str_ops;
	const osal_time_ops_t *time_ops;
} lexleo_app_env_t;

lexleo_app_env_t lexleo_app_default_env(
	const osal_mem_ops_t *mem_ops,
	const osal_stdio_ops_t *stdio_ops,
	const osal_file_ops_t *file_ops,
	const osal_str_ops_t *str_ops,
	const osal_time_ops_t *time_ops);

lexleo_app_cr_status_t lexleo_app_create(
	lexleo_app_t **out,
	const lexleo_app_env_t *env);

lexleo_app_cr_status_t lexleo_app_complete_default_init(
	lexleo_app_t *app,
	const lexleo_app_cfg_t *cfg);

#endif /* LEXLEO_LEXLEO_APP_CR_INTERNAL_H */
