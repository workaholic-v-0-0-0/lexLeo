/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_app_cr_opaque_types.h
 * @ingroup lexleo_app_cr_opaque_api
 * @brief Public Composition Root configuration types for lexleo_app.
 *
 * @details
 * Defines the requested application action, program input and compilation
 * output paths, and configuration of input, output, and diagnostic streams.
 */

#ifndef LEXLEO_LEXLEO_APP_CR_OPAQUE_TYPES_H
#define LEXLEO_LEXLEO_APP_CR_OPAQUE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum lexleo_app_io_kind_t {
	LEXLEO_APP_IO_STDIO,
	LEXLEO_APP_IO_FILE,
	LEXLEO_APP_IO_BUFFER
} lexleo_app_io_kind_t;

typedef struct lexleo_app_io_cfg_t {
	lexleo_app_io_kind_t kind;
	const char *path; /* only for FILE */
	const char *mode; /* "rb", "wb" or "ab" */
} lexleo_app_io_cfg_t;

typedef enum lexleo_app_action_t {
	LEXLEO_APP_ACTION_INTERPRET,
	LEXLEO_APP_ACTION_COMPILE,
	LEXLEO_APP_ACTION_EXECUTE,
	LEXLEO_APP_ACTION_HELP,
	LEXLEO_APP_ACTION_VERSION
} lexleo_app_action_t;

typedef struct lexleo_app_cfg_t {

	lexleo_app_action_t action;

	/** Source path for COMPILE/INTERPRET, bytecode path for EXECUTE.
	 * NULL selects the configured input stream for INTERPRET.
	 * Unused for HELP and VERSION. */
	const char *input_path;

	/** Bytecode destination for COMPILE.
	 * NULL selects the default destination. Unused for other actions. */
	const char *output_path;

	/** Application input, also used for runtime reads. */
	lexleo_app_io_cfg_t in;

	/** Application and runtime output. */
	lexleo_app_io_cfg_t out;

	/** User-facing diagnostics. */
	lexleo_app_io_cfg_t err;

} lexleo_app_cfg_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LEXLEO_APP_CR_OPAQUE_TYPES_H */
