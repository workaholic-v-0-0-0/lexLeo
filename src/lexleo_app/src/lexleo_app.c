
// WIP todo

#include "internal/lexleo_app_handle.h"
#include "internal/lexleo_app_env.h"
#include "internal/lexleo_app_log_path.h"

#include "lexleo_app/lexleo_app.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/osal_mem.h"
#include "osal/stdio/osal_stdio_ops.h"
#include "osal/file/osal_file_ops.h"
#include "osal/str/osal_str_ops.h"
#include "osal/str/osal_str.h"
#include "osal/time/osal_time_ops.h"

#include "stream/cr/stream_cr_api.h"

#include "fs_stream/cr/fs_stream_cr_api.h"

#include "stdio_stream/cr/stdio_stream_cr_api.h"

#include "logger/owners/logger_owners_api.h"
#include "logger/adapters/logger_adapters_api.h"

#include "logger_default/cr/logger_default_cr_api.h"

#include "lexleo_vm/cr/lexleo_vm_cr_api.h"

#include "policy/lexleo_assert.h"

static lexleo_app_io_cfg_t lexleo_app_default_stdio_io_cfg(void);
lexleo_app_cfg_t lexleo_app_default_cfg(void)
{
	return (lexleo_app_cfg_t){
		.in = lexleo_app_default_stdio_io_cfg(),
		.out = lexleo_app_default_stdio_io_cfg(),
		.err = lexleo_app_default_stdio_io_cfg()
	};
}

void lexleo_app_cfg_set_input_file(
	lexleo_app_cfg_t *cfg,
	const char *path
) {
	LEXLEO_ASSERT(cfg);
	cfg->in.kind = LEXLEO_APP_IO_FILE;
	cfg->in.path = path;
	cfg->in.mode = "rb";
}

void lexleo_app_cfg_set_output_file(
	lexleo_app_cfg_t *cfg,
	const char *path
) {
	LEXLEO_ASSERT(cfg);
	cfg->out.kind = LEXLEO_APP_IO_FILE;
	cfg->out.path = path;
	cfg->out.mode = "ab";
}

void lexleo_app_cfg_set_error_file(
	lexleo_app_cfg_t *cfg,
	const char *path
) {
	LEXLEO_ASSERT(cfg);
	cfg->err.kind = LEXLEO_APP_IO_FILE;
	cfg->err.path = path;
	cfg->err.mode = "ab";
}

lexleo_app_env_t lexleo_app_default_env(void)
{
	return (lexleo_app_env_t) {
		.mem_ops = osal_mem_default_ops(),
		.stdio_ops = osal_stdio_default_ops(),
		.file_ops = osal_file_default_ops(),
		.str_ops = osal_str_default_ops(),
		.time_ops = osal_time_default_ops()
	};
}

lexleo_app_status_t lexleo_app_create_with_env(
	lexleo_app_t **out,
	const lexleo_app_cfg_t *cfg,
	const lexleo_app_env_t *env
) {
	LEXLEO_ASSERT(
		   out
		&& cfg
		&& env
		&& env->mem_ops
		&& env->mem_ops->calloc
	);

	*out = NULL;

	lexleo_app_t *tmp = env->mem_ops->calloc(1, sizeof(*tmp));
	if (!tmp) {
		return LEXLEO_APP_STATUS_OOM;
	}

	tmp->cfg = *cfg;

	tmp->mem_ops = env->mem_ops;
	tmp->stdio_ops = env->stdio_ops;
	tmp->file_ops = env->file_ops;
	tmp->str_ops = env->str_ops;
	tmp->time_ops = env->time_ops;

	*out = tmp;

	return LEXLEO_APP_STATUS_OK;
}

lexleo_app_status_t lexleo_app_create(
	lexleo_app_t **out,
	const lexleo_app_cfg_t *cfg
) {
	lexleo_app_env_t env = lexleo_app_default_env();
	return
		lexleo_app_create_with_env(
			out,
			cfg,
			&env
		);
}

static bool lexleo_app_try_init_default_stdio_stream(
	lexleo_app_t *app,
	stream_t **stream,
	stream_standard_stream_kind_t kind
);
static bool lexleo_app_try_init_default_fs_stream(
	lexleo_app_t *app,
	stream_t **stream,
	const char *path,
	const char *mode
);

static bool lexleo_app_is_initialized(lexleo_app_t *app);

lexleo_app_status_t lexleo_app_run(
	lexleo_app_t *app
) {
	LEXLEO_ASSERT(lexleo_app_is_initialized(app));

	// todo

	return LEXLEO_APP_STATUS_OK; // placeholder
}

void lexleo_app_destroy(
	lexleo_app_t **app
) {
	if (!app || !*app) {
		return;
	}

	lexleo_app_t *tmp = *app;
	*app = NULL;

	LEXLEO_ASSERT(
		   tmp
		&& tmp->mem_ops
		&& tmp->mem_ops->free
	);

	stream_destroy(&tmp->in);
	stream_destroy(&tmp->out);
	stream_destroy(&tmp->err);
	logger_destroy(&tmp->logger);
	lexleo_vm_destroy(&tmp->vm);

	tmp->mem_ops->free(tmp);
}

static lexleo_app_io_cfg_t lexleo_app_default_stdio_io_cfg(void)
{
	return (lexleo_app_io_cfg_t){
		.kind = LEXLEO_APP_IO_STDIO,
		.path = NULL,
		.mode = NULL
	};
}

static bool lexleo_app_try_init_default_stdio_stream(
	lexleo_app_t *app,
	stream_t **stream,
	stream_standard_stream_kind_t kind
) {
	LEXLEO_ASSERT(app);

	stdio_stream_env_t stdio_stream_env =
		stdio_stream_default_env(
			app->stdio_ops,
			app->mem_ops
		);
	stdio_stream_t *stdio_stream = NULL;
	stdio_stream_status_t stdio_stream_status =
		stdio_stream_create(
			&stdio_stream,
			&stdio_stream_env
		);
	if (stdio_stream_status != STDIO_STREAM_STATUS_OK) {
		return false;
	}

	stdio_stream_cfg_t stdio_stream_cfg = stdio_stream_default_cfg();
	const stream_standard_stream_creator_args_t args = {
		.kind = kind
	};
	stdio_stream_status =
		stdio_stream_complete_default_init(
			stdio_stream,
			&stdio_stream_cfg,
			&args
		);
	if (stdio_stream_status != STDIO_STREAM_STATUS_OK) {
		stdio_stream_vtbl()->close(stdio_stream);
		return false;
	}

	stream_env_t stream_env =
		stream_default_env(
			stdio_stream_vtbl(),
			app->mem_ops
		);
	stream_status_t stream_status = stream_create(stream, &stream_env);
	if (stream_status != STREAM_STATUS_OK) {
		stdio_stream_vtbl()->close(stdio_stream);
		return false;
	}

	stream_status = stream_complete_default_init(*stream, stdio_stream);
	if (stream_status != STREAM_STATUS_OK) {
		stream_destroy(stream);
		return false;
	}

	return true;
}

static bool lexleo_app_try_init_default_fs_stream(
	lexleo_app_t *app,
	stream_t **stream,
	const char *path,
	const char *mode
) {
	LEXLEO_ASSERT(app);

	fs_stream_env_t fs_stream_env =
		fs_stream_default_env(
			app->file_ops,
			app->mem_ops
		);
	fss_stream_t *fs_stream = NULL;
	fs_stream_status_t fs_stream_status =
		fs_stream_create(
			&fs_stream,
			&fs_stream_env
		);
	if (fs_stream_status != FS_STREAM_STATUS_OK) {
		return false;
	}

	fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();
	const stream_regular_file_creator_args_t args = {
		.path = path,
		.mode = mode
	};
	fs_stream_status =
		fs_stream_complete_default_init(
			fs_stream,
			&fs_stream_cfg,
			&args
		);
	if (fs_stream_status != FS_STREAM_STATUS_OK) {
		fs_stream_vtbl()->close(fs_stream);
		return false;
	}

	stream_env_t stream_env =
		stream_default_env(
			fs_stream_vtbl(),
			app->mem_ops
		);
	stream_status_t stream_status = stream_create(stream, &stream_env);
	if (stream_status != STREAM_STATUS_OK) {
		fs_stream_vtbl()->close(fs_stream);
		return false;
	}

	stream_status = stream_complete_default_init(*stream, fs_stream);
	if (stream_status != STREAM_STATUS_OK) {
		stream_destroy(stream);
		return false;
	}

	return true;
}

static lexleo_app_status_t lexleo_app_init_default_in(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);

	switch (app->cfg.in.kind) {
		case LEXLEO_APP_IO_STDIO:
			return
				lexleo_app_try_init_default_stdio_stream(
					app,
					&app->in,
					STREAM_STANDARD_STREAM_KIND_STDIN
				) ?
					  LEXLEO_APP_STATUS_OK
					: LEXLEO_APP_STATUS_INPUT_INIT_ERROR;
		case LEXLEO_APP_IO_FILE:
			return
				lexleo_app_try_init_default_fs_stream(
					app,
					&app->in,
					app->cfg.in.path,
					app->cfg.in.mode
				) ?
					  LEXLEO_APP_STATUS_OK
					: LEXLEO_APP_STATUS_INPUT_INIT_ERROR;
		default:
			return LEXLEO_APP_STATUS_INPUT_INIT_ERROR;
	}
}

static lexleo_app_status_t lexleo_app_init_default_out(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);

	switch (app->cfg.out.kind) {
		case LEXLEO_APP_IO_STDIO:
			return
				lexleo_app_try_init_default_stdio_stream(
					app,
					&app->out,
					STREAM_STANDARD_STREAM_KIND_STDOUT
				) ?
					  LEXLEO_APP_STATUS_OK
					: LEXLEO_APP_STATUS_OUTPUT_INIT_ERROR;
		case LEXLEO_APP_IO_FILE:
			return
				lexleo_app_try_init_default_fs_stream(
					app,
					&app->out,
					app->cfg.in.path,
					app->cfg.in.mode
				) ?
					  LEXLEO_APP_STATUS_OK
					: LEXLEO_APP_STATUS_OUTPUT_INIT_ERROR;
		default:
			return LEXLEO_APP_STATUS_OUTPUT_INIT_ERROR;
	}
}

static lexleo_app_status_t lexleo_app_init_default_err(lexleo_app_t *app)
{
	LEXLEO_ASSERT(app);

	switch (app->cfg.err.kind) {
		case LEXLEO_APP_IO_STDIO:
			return
				lexleo_app_try_init_default_stdio_stream(
					app,
					&app->err,
					STREAM_STANDARD_STREAM_KIND_STDERR
				) ?
					  LEXLEO_APP_STATUS_OK
					: LEXLEO_APP_STATUS_ERR_INIT_ERROR;
		case LEXLEO_APP_IO_FILE:
			return
				lexleo_app_try_init_default_fs_stream(
					app,
					&app->err,
					app->cfg.in.path,
					app->cfg.in.mode
				) ?
					  LEXLEO_APP_STATUS_OK
					: LEXLEO_APP_STATUS_ERR_INIT_ERROR;
		default:
			return LEXLEO_APP_STATUS_ERR_INIT_ERROR;
	}
}

static lexleo_app_status_t lexleo_app_init_default_log_path(
	lexleo_app_t *app
) {
	LEXLEO_ASSERT(app);
	return (
		lexleo_app_resolve_log_path(
			app->log_path,
			sizeof(app->log_path),
			app->mem_ops,
			app->file_ops
		)
	)
		? LEXLEO_APP_STATUS_OK
		: LEXLEO_APP_STATUS_LOG_PATH_RESOLUTION_ERROR;
}

static lexleo_app_status_t lexleo_app_create_logger_stream_from_app(
	stream_t **out,
	lexleo_app_t *app
) {
	LEXLEO_ASSERT(app);

	fs_stream_env_t fs_stream_env =
		fs_stream_default_env(
			app->file_ops,
			app->mem_ops,
			app->mem_ops
		);
	fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();

	return (
		fs_stream_create_stream(
			out,
			&(stream_file_creator_args_t)
			{
				.path = app->log_path,
				.mode = "ab"
			},
			&fs_stream_cfg,
			&fs_stream_env
		) == STREAM_STATUS_OK
	)
		? LEXLEO_APP_STATUS_OK
		: LEXLEO_APP_STATUS_LOGGER_STREAM_INIT_ERROR;
}

static lexleo_app_status_t lexleo_app_init_default_logger(
	lexleo_app_t *app,
	stream_t *logger_stream
) {
	LEXLEO_ASSERT(app);

	logger_default_cfg_t logger_default_cfg = logger_default_default_cfg();
	logger_default_env_t logger_default_env =
		logger_default_default_env(
			logger_stream,
			app->time_ops,
			app->mem_ops,
			app->mem_ops
		);

	return (
		logger_default_create_logger(
			&app->logger,
			&logger_default_cfg,
			&logger_default_env
		) == LOGGER_STATUS_OK
	)
		? LEXLEO_APP_STATUS_OK
		: LEXLEO_APP_STATUS_LOGGER_INIT_ERROR;
}

static lexleo_app_status_t lexleo_app_init_default_vm(
	lexleo_app_t *app
) {
	LEXLEO_ASSERT(app);
	lexleo_vm_cfg_t vm_cfg = lexleo_vm_default_cfg();
	lexleo_vm_env_t vm_env =
		lexleo_vm_default_env(
			app->mem_ops,
			app->stdio_ops,
			app->file_ops,
			app->str_ops,
			app->time_ops,
			app->in,
			app->out,
			app->err,
			app->logger
		);
	return (
		lexleo_vm_create(
			&app->vm,
			&vm_cfg,
			&vm_env
		) == LEXLEO_VM_STATUS_OK
	)
		? LEXLEO_APP_STATUS_OK
		: LEXLEO_APP_STATUS_VM_INIT_ERROR;
}

lexleo_app_status_t lexleo_app_complete_default_init(
	lexleo_app_t *app
) {
	LEXLEO_ASSERT(
		   app
		&& app->mem_ops
		&& app->mem_ops->calloc
		&& app->stdio_ops
		&& app->file_ops
		&& app->str_ops
		&& app->time_ops
	);

	lexleo_app_status_t st = LEXLEO_APP_STATUS_OK;

	if (!app->in) {
		st = lexleo_app_init_default_in(app);
		if (st != LEXLEO_APP_STATUS_OK) {
			return st;
		}
	}

	if (!app->out) {
		st = lexleo_app_init_default_out(app);
		if (st != LEXLEO_APP_STATUS_OK) {
			return st;
		}
	}

	if (!app->err) {
		st = lexleo_app_init_default_err(app);
		if (st != LEXLEO_APP_STATUS_OK) {
			return st;
		}
	}

	if (*app->log_path == (char)0) {
		st = lexleo_app_init_default_log_path(app);
		if (st != LEXLEO_APP_STATUS_OK) {
			return st;
		}
	}

	if (!app->logger) {
		stream_t *logger_stream = NULL;
		st = lexleo_app_create_logger_stream_from_app(&logger_stream, app);
		if (st != LEXLEO_APP_STATUS_OK) {
			return st;
		}
		st = lexleo_app_init_default_logger(app, logger_stream);
		if (st != LEXLEO_APP_STATUS_OK) {
			return st;
		}
	}

	if (!app->vm) {
		if (lexleo_app_init_default_vm(app) != LEXLEO_APP_STATUS_OK) {
			return LEXLEO_APP_STATUS_VM_INIT_ERROR;
		}
	}

	return LEXLEO_APP_STATUS_OK;
}

static bool lexleo_app_is_initialized(lexleo_app_t *app)
{
	return
		   app
		&& app->mem_ops
		&& app->stdio_ops
		&& app->file_ops
		&& app->str_ops
		&& app->time_ops
		&& app->in
		&& app->out
		&& app->err
		&& app->logger
		&& app->vm;
}
