/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file lexleo_app_cr_opaque_types.h
 * @ingroup lexleo_app_cr_opaque_api
 * @brief Public Composition Root configuration types for lexleo_app.
 *
 * @details
 * Defines the requested application action, input and output paths,
 * dynamic library paths, and application stream configuration.
 */

#ifndef LEXLEO_LEXLEO_APP_CR_OPAQUE_TYPES_H
#define LEXLEO_LEXLEO_APP_CR_OPAQUE_TYPES_H

#include "lexleo_app/common/lexleo_app_action_type.h"
#include "lexleo_app/borrowers/lexleo_app_borrowers_api.h"

#include "policy/lexleo_cstd_types.h"

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

	/** File path for FILE. Ignored for STDIO. */
	const char *path;

	/** File opening mode for FILE: "rb", "wb", or "ab".
	 * Ignored for STDIO. */
	const char *mode;

} lexleo_app_io_cfg_t;

typedef struct lexleo_app_cfg_t {

	/** Requested application action. */
	lexleo_app_action_t action;

	/**
	 * NULL-terminated array of input file paths.
	 *
	 * Paths designate source files for COMPILE/INTERPRET,
	 * assembly files for ASSEMBLE, object files for LINK,
	 * or an executable for EXECUTE.
	 *
	 * Allowed numbers of input files depend on the action.
	 * At most LEXLEO_APP_MAX_INPUT_PATHS - 1 paths may be stored.
	 *
	 * A NULL first element denotes an empty list.
	 * For INTERPRET, an empty list selects the configured input stream.
	 * Unused for CONTROL_VM, HELP, and VERSION. */
	const char *const input_paths[LEXLEO_APP_MAX_INPUT_PATHS];

	/**
	 * NULL-terminated array of dynamic library paths.
	 *
	 * At most LEXLEO_APP_MAX_INPUT_DYN_LIB - 1 paths may be stored.
	 * A NULL first element denotes an empty list.
	 *
	 * For LINK, identifies libraries against which dynamic linkage
	 * is to be prepared. */
	const char *const input_dynamic_libraries[LEXLEO_APP_MAX_INPUT_DYN_LIB];

	/**
	 * Artifact destination for COMPILE, ASSEMBLE, or LINK.
	 *
	 * NULL selects the action's default destination.
	 * Unused for other actions. */
	const char *output_path;

	/**
	 * Application input, also used for runtime reads.
	 * STDIO selects standard input. */
	lexleo_app_io_cfg_t in;

	/**
	 * Application and runtime output.
	 * STDIO selects standard output. */
	lexleo_app_io_cfg_t out;

	/**
	 * User-facing diagnostics.
	 * STDIO selects standard error. */
	lexleo_app_io_cfg_t err;

} lexleo_app_cfg_t;

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_LEXLEO_APP_CR_OPAQUE_TYPES_H */
