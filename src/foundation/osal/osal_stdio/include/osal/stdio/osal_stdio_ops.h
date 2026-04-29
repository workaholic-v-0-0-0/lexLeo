/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_stdio_ops.h
 * @ingroup osal_stdio_api
 * @brief Injectable standard I/O operations table for the `osal_stdio` module.
 *
 * @details
 * This header declares the standard I/O operations interface used to inject
 * standard stream services into modules that must not depend directly on the
 * default standard I/O backend.
 *
 * Current injectable services:
 * - standard stream access
 * - standard stream reads and writes
 * - standard stream flushing
 * - error-state queries
 * - end-of-file queries
 * - error-state clearing
 */

#ifndef LEXLEO_OSAL_STDIO_OPS_H
#define LEXLEO_OSAL_STDIO_OPS_H

#include "osal/stdio/osal_stdio_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct osal_stdio_ops_t {

	OSAL_STDIO *(*get_stdin)(void);

	OSAL_STDIO *(*get_stdout)(void);

	OSAL_STDIO *(*get_stderr)(void);

	size_t (*read)(
		void *ptr,
		size_t size,
		size_t nmemb,
		OSAL_STDIO *stdio);

	size_t (*write)(
		const void *ptr,
		size_t size,
		size_t nmemb,
		OSAL_STDIO *stdio);

	int (*flush)(OSAL_STDIO *stdio);

	int (*error)(OSAL_STDIO *stdio);

	int (*eof)(OSAL_STDIO *stdio);
};

const osal_stdio_ops_t *osal_stdio_default_ops(void);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_OSAL_STDIO_OPS_H */
