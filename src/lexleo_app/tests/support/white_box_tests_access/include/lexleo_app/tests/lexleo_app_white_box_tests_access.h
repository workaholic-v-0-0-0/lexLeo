

#ifndef LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_H
#define LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_H

#include "internal/lexleo_app_env.h"

#include "lexleo_app/lexleo_app_types.h"

#include "osal/mem/osal_mem_types.h"
#include "osal/stdio/osal_stdio_types.h"
#include "osal/file/osal_file_types.h"
#include "osal/str/osal_str_types.h"
#include "osal/time/osal_time_types.h"

#include "stream/borrowers/stream_types.h"

lexleo_app_env_t lexleo_app_default_env(void);

void lexleo_app_env_inject_mem_ops(
	lexleo_app_env_t *env,
	const osal_mem_ops_t *mem_ops);

void lexleo_app_env_inject_stdio_ops(
	lexleo_app_env_t *env,
	const osal_stdio_ops_t *stdio_ops);

void lexleo_app_env_inject_file_ops(
	lexleo_app_env_t *env,
	const osal_file_ops_t *file_ops);

void lexleo_app_env_inject_str_ops(
	lexleo_app_env_t *env,
	const osal_str_ops_t *str_ops);

void lexleo_app_env_inject_time_ops(
	lexleo_app_env_t *env,
	const osal_time_ops_t *time_ops);

lexleo_app_status_t lexleo_app_create_with_env(
	lexleo_app_t **out,
	const lexleo_app_cfg_t *cfg,
	const lexleo_app_env_t *env
);

void lexleo_app_inject_in(lexleo_app_t *app, stream_t *in);
void lexleo_app_inject_out(lexleo_app_t *app, stream_t *out);
void lexleo_app_inject_err(lexleo_app_t *app, stream_t *err);
void lexleo_app_set_log_path(lexleo_app_t *app, const char *log_path);
void lexleo_app_inject_logger(lexleo_app_t *app, logger_t *logger);
void lexleo_app_inject_vm(lexleo_app_t *app, lexleo_vm_t *vm);

#endif /* LEXLEO_APP_WHITE_BOX_TESTS_ACCESS_H */
