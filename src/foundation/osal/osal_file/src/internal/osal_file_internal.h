/* SPDX-License-Identifier: GPL-3.0-or-later
* Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_file_internal.h
 * @ingroup osal_file_internal_group
 * @brief Private internal definitions for the `osal_file` module.
 *
 * @details
 * This header exposes the private in-memory representation of `OSAL_FILE`
 * used by the active OSAL file backends.
 *
 * It is intended for internal implementation use only.
 */

#ifndef LEXLEO_OSAL_FILE_INTERNAL_H
#define LEXLEO_OSAL_FILE_INTERNAL_H

#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_cstd_io.h"

/**
 * @brief Private representation of an acquired OSAL file handle.
 *
 * @details
 * This structure stores:
 * - the underlying C standard I/O file handle used by the active backend,
 * - the memory operations table required to release the wrapper itself.
 *
 * This representation is private to the `osal_file` implementation and is not
 * part of the public OSAL file contract.
 */
struct OSAL_FILE {
	/**
	 * @brief Underlying C standard I/O file handle.
	 */
	FILE *fp;

	/**
	 * @brief Memory operations table used to release this wrapper.
	 */
	const osal_mem_ops_t *mem_ops;
};

#endif // LEXLEO_OSAL_FILE_INTERNAL_H
