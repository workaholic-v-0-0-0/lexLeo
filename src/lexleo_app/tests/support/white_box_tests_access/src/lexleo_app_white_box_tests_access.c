#include "lexleo_app/tests/lexleo_app_white_box_tests_access.h"

#include "internal/lexleo_app_handle.h"

#include "policy/lexleo_assert.h"

void lexleo_app_env_inject_mem_ops(
	lexleo_app_env_t *env,
	const osal_mem_ops_t *mem_ops
) {
	LEXLEO_ASSERT(env);
	env->mem_ops = mem_ops;
}

void lexleo_app_env_inject_stdio_ops(
	lexleo_app_env_t *env,
	const osal_stdio_ops_t *stdio_ops
) {
	LEXLEO_ASSERT(env);
	env->stdio_ops = stdio_ops;
}

void lexleo_app_env_inject_file_ops(
	lexleo_app_env_t *env,
	const osal_file_ops_t *file_ops
) {
	LEXLEO_ASSERT(env);
	env->file_ops = file_ops;
}

void lexleo_app_env_inject_str_ops(
	lexleo_app_env_t *env,
	const osal_str_ops_t *str_ops
) {
	LEXLEO_ASSERT(env);
	env->str_ops = str_ops;
}

void lexleo_app_env_inject_time_ops(
	lexleo_app_env_t *env,
	const osal_time_ops_t *time_ops
) {
	LEXLEO_ASSERT(env);
	env->time_ops = time_ops;
}

void lexleo_app_inject_in(lexleo_app_t *app, stream_t *in)
{
	LEXLEO_ASSERT(app);
	app->in = in;
}
void lexleo_app_inject_out(lexleo_app_t *app, stream_t *out)
{
	LEXLEO_ASSERT(app);
	app->out = out;
}
void lexleo_app_inject_err(lexleo_app_t *app, stream_t *err)
{
	LEXLEO_ASSERT(app);
	app->err = err;
}
void lexleo_app_set_log_path(lexleo_app_t *app, const char *log_path)
{
	LEXLEO_ASSERT(
		   app
		&& log_path
		&& osal_strlen(log_path) < sizeof(app->log_path)
	);
	osal_memcpy(app->log_path, log_path, osal_strlen(log_path) + 1);
}
void lexleo_app_inject_logger(lexleo_app_t *app, logger_t *logger)
{
	LEXLEO_ASSERT(app);
	app->logger = logger;
}
void lexleo_app_inject_vm(lexleo_app_t *app, lexleo_vm_t *vm)
{
	LEXLEO_ASSERT(app);
	app->vm = vm;
}
