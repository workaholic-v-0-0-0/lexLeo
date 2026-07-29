/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_app_cr.c
 * @ingroup lexleo_app_internal_group
 * @brief Composition Root implementation for lexleo_app.
 *
 * @details
 * Implements application configuration, environment construction,
 * handle creation, default component initialization, and destruction.
 *
 * The public construction service composes the application using
 * the supplied configuration and the default runtime environment.
 */

#include "../internal/lexleo_app_handle.h"
#include "../internal/lexleo_app_cr_internal.h"
#include "../internal/lexleo_app_log_path.h"

#include "lexleo_app/cr_opaque/lexleo_app_cr_opaque_api.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/osal_mem.h"
#include "osal/stdio/osal_stdio_ops.h"
#include "osal/file/osal_file_ops.h"
#include "osal/str/osal_str_ops.h"
#include "osal/str/osal_str.h"
#include "osal/time/osal_time_ops.h"

#include "stream/cr/stream_cr_api.h"
#include "stream/adapters/stream_adapters_types.h"
#include "fs_stream/cr/fs_stream_cr_api.h"
#include "stdio_stream/cr/stdio_stream_cr_api.h"
#include "dynamic_buffer_stream/cr/dynamic_buffer_stream_cr_api.h"

#include "logger/cr/logger_cr_api.h"
#include "logger/adapters/logger_adapters_types.h"
#include "logger_default/cr/logger_default_cr_api.h"

#include "lexleo_vm/cr/lexleo_vm_cr_api.h"

#include "policy/lexleo_assert.h"

lexleo_app_cfg_t lexleo_app_default_cfg(void)
{
	return (lexleo_app_cfg_t){
		.action = LEXLEO_APP_ACTION_CONTROL_VM,
		.input_paths = { NULL },
		.input_dynamic_libraries = { NULL },
		.output_path = NULL,
		.in = (lexleo_app_io_cfg_t){
			.kind = LEXLEO_APP_IO_STDIO,
			.path = NULL,
			.mode = NULL
		},
		.out = (lexleo_app_io_cfg_t){
			.kind = LEXLEO_APP_IO_STDIO,
			.path = NULL,
			.mode = NULL
		},
		.err = (lexleo_app_io_cfg_t){
			.kind = LEXLEO_APP_IO_STDIO,
			.path = NULL,
			.mode = NULL
		}
	};
}

lexleo_app_cr_status_t lexleo_app_create(
	lexleo_app_t **out,
	const lexleo_app_env_t *env
) {
	LEXLEO_ASSERT(
		   out
		&& env
		&& env->mem_ops
		&& env->mem_ops->calloc
		&& env->mem_ops->free
	);

	lexleo_app_t *tmp = env->mem_ops->calloc(1, sizeof(*tmp));
	if (!tmp) {
		return LEXLEO_APP_CR_STATUS_OOM;
	}

	tmp->mem_ops = env->mem_ops;
	tmp->stdio_ops = env->stdio_ops;
	tmp->file_ops = env->file_ops;
	tmp->str_ops = env->str_ops;
	tmp->time_ops = env->time_ops;

	*out = tmp;

	return LEXLEO_APP_CR_STATUS_OK;
}

void lexleo_app_destroy(lexleo_app_t **app)
{
	if (!app || !*app) {
		return;
	}

	LEXLEO_ASSERT((*app)->mem_ops && (*app)->mem_ops->free);

	lexleo_vm_destroy(&(*app)->vm);
	stream_destroy(&(*app)->in);
	stream_destroy(&(*app)->out);
	stream_destroy(&(*app)->err);
	logger_destroy(&(*app)->logger);
	stream_destroy(&(*app)->logger_stream);

	(*app)->mem_ops->free(*app);
	*app = NULL;
}

static bool lexleo_app_supported_io_cfg(const lexleo_app_io_cfg_t io_cfg)
{
	return
			  (io_cfg.kind == LEXLEO_APP_IO_STDIO
			&& io_cfg.path == NULL
			&& io_cfg.mode == NULL)
		||
			(io_cfg.kind == LEXLEO_APP_IO_FILE
		  && io_cfg.path != NULL
		  && *io_cfg.path != '\0'
		  && io_cfg.mode != NULL
		  &&
			  (osal_strcmp(io_cfg.mode, "rb") == 0
			|| osal_strcmp(io_cfg.mode, "wb") == 0
			|| osal_strcmp(io_cfg.mode, "ab") == 0))
		||
			   (io_cfg.kind == LEXLEO_APP_IO_BUFFER
			&& io_cfg.path == NULL
			&& io_cfg.mode == NULL);
}

static bool lexleo_app_supported_cfg(const lexleo_app_cfg_t cfg)
{
	return
		  (lexleo_app_supported_io_cfg(cfg.in)
		&& lexleo_app_supported_io_cfg(cfg.out)
		&& lexleo_app_supported_io_cfg(cfg.err));
}

static bool lexleo_app_init_stream(
	stream_t **stream,
	lexleo_app_t *app,
	const lexleo_app_io_cfg_t io_cfg
) {
	LEXLEO_ASSERT(
		   stream
		&& app
		&& app->mem_ops
		&& app->stdio_ops
		&& app->file_ops
	);

	const stream_vtbl_t *vtbl = NULL;
	void *backend = NULL;
	switch (io_cfg.kind) {
		case LEXLEO_APP_IO_STDIO: {
			vtbl = stdio_stream_vtbl();
			stdio_stream_env_t stdio_stream_env =
				stdio_stream_default_env(
					app->stdio_ops,
					app->mem_ops
				);
			stdio_stream_cfg_t stdio_stream_cfg = stdio_stream_default_cfg();
			stream_standard_stream_kind_t stream_standard_stream_kind;
			if (stream == &app->in) {
				stream_standard_stream_kind = STREAM_STANDARD_STREAM_KIND_STDIN;
			} else if (stream == &app->out) {
				stream_standard_stream_kind = STREAM_STANDARD_STREAM_KIND_STDOUT;
			} else if (stream == &app->err) {
				stream_standard_stream_kind = STREAM_STANDARD_STREAM_KIND_STDERR;
			} else {
				LEXLEO_ASSERT(false);
				return false;
			}
			stdio_stream_t *stdio_stream = NULL;
			stdio_stream_status_t stdio_stream_status =
				stdio_stream_create_init(
					&stdio_stream,
					&stdio_stream_env,
					&stdio_stream_cfg,
					stream_standard_stream_kind
				);
			if (stdio_stream_status != STDIO_STREAM_STATUS_OK) {
				return false;
			}
			backend = stdio_stream;
			break;
		}
		case LEXLEO_APP_IO_FILE: {
			vtbl = fs_stream_vtbl();
			fs_stream_env_t fs_stream_env =
				fs_stream_default_env(
					app->file_ops,
					app->mem_ops
				);
			fs_stream_cfg_t fs_stream_cfg = fs_stream_default_cfg();
			fs_stream_t *fs_stream = NULL;
			fs_stream_status_t fs_stream_status =
				fs_stream_create_init(
					&fs_stream,
					&fs_stream_env,
					&fs_stream_cfg,
					io_cfg.path,
					io_cfg.mode
				);
			if (fs_stream_status != FS_STREAM_STATUS_OK) {
				return false;
			}
			backend = fs_stream;
			break;
		}
		case LEXLEO_APP_IO_BUFFER: {
			vtbl = dynamic_buffer_stream_vtbl();
			dynamic_buffer_stream_env_t dynamic_buffer_stream_env =
				dynamic_buffer_stream_default_env(
					app->mem_ops
				);
			dynamic_buffer_stream_cfg_t dynamic_buffer_stream_cfg =
				dynamic_buffer_stream_default_cfg();
			dynamic_buffer_stream_t *dynamic_buffer_stream = NULL;
			dynamic_buffer_stream_status_t dbs_status =
				dynamic_buffer_stream_create_init(
					&dynamic_buffer_stream,
					&dynamic_buffer_stream_env,
					&dynamic_buffer_stream_cfg
				);
			if (dbs_status != DYNAMIC_BUFFER_STREAM_STATUS_OK) {
				return false;
			}
			backend = dynamic_buffer_stream;
			break;
		}
		default: {
			LEXLEO_ASSERT(false);
			return false;
		}
	}

	stream_t *tmp = NULL;
	stream_env_t stream_env =
		stream_default_env(
			vtbl,
			app->mem_ops
		);
	stream_status_t stream_status =
		stream_create_init(
			&tmp,
			&stream_env,
			backend
		);
	if (stream_status != STREAM_STATUS_OK) {
		vtbl->close(backend);
		return false;
	}

	*stream = tmp;
	return true;
}

lexleo_app_cr_status_t lexleo_app_complete_default_init(
	lexleo_app_t *app,
	const lexleo_app_cfg_t *cfg
) {
	LEXLEO_ASSERT(app && cfg && lexleo_app_supported_cfg(*cfg));

	if (!app->in) {
		if (!lexleo_app_init_stream(&app->in, app, cfg->in)) {
			return LEXLEO_APP_CR_STATUS_INPUT_INIT_ERROR;
		}
	}

	if (!app->out) {
		if (!lexleo_app_init_stream(&app->out, app, cfg->out)) {
			return LEXLEO_APP_CR_STATUS_OUTPUT_INIT_ERROR;
		}
	}

	if (!app->err) {
		if (!lexleo_app_init_stream(&app->err, app, cfg->err)) {
			return LEXLEO_APP_CR_STATUS_ERR_INIT_ERROR;
		}
	}

	if (app->log_path[0] == '\0') {
		bool ok =
			lexleo_app_resolve_log_path(
				app->log_path,
				sizeof(app->log_path),
				app->mem_ops,
				app->file_ops
			);
		if (!ok) {
			return LEXLEO_APP_CR_STATUS_LOG_PATH_RESOLUTION_ERROR;
		}
	}

	if (!app->logger_stream) {
		lexleo_app_io_cfg_t logger_stream_cfg = {
			.kind = LEXLEO_APP_IO_FILE,
			.path = app->log_path,
			.mode = "ab"
		};
		bool ok =
			lexleo_app_init_stream(
				&app->logger_stream,
				app,
				logger_stream_cfg
			);
		if (!ok) {
			return LEXLEO_APP_CR_STATUS_LOGGER_STREAM_INIT_ERROR;
		}
	}

	if (!app->logger) {
		logger_default_t *backend = NULL;
		logger_default_env_t backend_env =
			logger_default_default_env(
				app->logger_stream,
				app->time_ops,
				app->mem_ops
			);
		logger_default_cfg_t logger_default_cfg = logger_default_default_cfg();
		logger_default_status_t logger_default_status =
			logger_default_create_init(
				&backend,
				&backend_env,
				&logger_default_cfg
			);
		if (logger_default_status != LOGGER_DEFAULT_STATUS_OK) {
			return LEXLEO_APP_CR_STATUS_LOGGER_INIT_ERROR;
		}
		logger_env_t logger_env =
			logger_default_env(
				logger_default_vtbl(),
				app->mem_ops
			);
		logger_status_t logger_status =
			logger_create_init(
				&app->logger,
				&logger_env,
				backend
			);
		if (logger_status != LOGGER_STATUS_OK) {
			logger_default_vtbl()->destroy(backend);
			return LEXLEO_APP_CR_STATUS_LOGGER_INIT_ERROR;
		}
	}

	if (!app->vm) {
		lexleo_vm_env_t lexleo_vm_env =
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
		lexleo_vm_cfg_t lexleo_vm_cfg = lexleo_vm_default_cfg();
		lexleo_vm_status_t lexleo_vm_status =
			lexleo_vm_create_init(
				&app->vm,
				&lexleo_vm_env,
				&lexleo_vm_cfg
			);
		if (lexleo_vm_status != LEXLEO_VM_STATUS_OK) {
			return LEXLEO_APP_CR_STATUS_VM_INIT_ERROR;
		}
	}

	return LEXLEO_APP_CR_STATUS_OK;
}

lexleo_app_env_t lexleo_app_default_env(
	const osal_mem_ops_t *mem_ops,
	const osal_stdio_ops_t *stdio_ops,
	const osal_file_ops_t *file_ops,
	const osal_str_ops_t *str_ops,
	const osal_time_ops_t *time_ops
) {
	return (lexleo_app_env_t){
		.mem_ops = mem_ops,
		.stdio_ops = stdio_ops,
		.file_ops = file_ops,
		.str_ops = str_ops,
		.time_ops = time_ops
	};
}

bool lexleo_app_create_init(
	lexleo_app_t **out,
	const lexleo_app_cfg_t *cfg
) {
	LEXLEO_ASSERT(out);

	lexleo_app_t *tmp = NULL;
	lexleo_app_env_t lexleo_app_env =
		lexleo_app_default_env(
			osal_mem_default_ops(),
			osal_stdio_default_ops(),
			osal_file_default_ops(),
			osal_str_default_ops(),
			osal_time_default_ops()
		);
	lexleo_app_cr_status_t lexleo_app_cr_status =
		lexleo_app_create(
			&tmp,
			&lexleo_app_env
		);
	if (lexleo_app_cr_status != LEXLEO_APP_CR_STATUS_OK) {
		LEXLEO_ASSERT(lexleo_app_cr_status == LEXLEO_APP_CR_STATUS_OOM);
		return false;
	}

	lexleo_app_cr_status = lexleo_app_complete_default_init(tmp, cfg);
	if (lexleo_app_cr_status != LEXLEO_APP_CR_STATUS_OK) {
		lexleo_app_destroy(&tmp);
		return false;
	}

	*out = tmp;
	return true;
}
