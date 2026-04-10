/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_file_types.h
 * @ingroup osal_file_api
 * @brief Public types for the `osal_file` module.
 *
 * @details
 * This header exposes:
 * - the opaque `OSAL_FILE` handle type used by the low-level OSAL file API,
 * - the portable `osal_file_status_t` status codes returned by `osal_file`
 *   operations.
 */

#ifndef LEXLEO_OSAL_FILE_TYPES_H
#define LEXLEO_OSAL_FILE_TYPES_H

/**
 * @brief Opaque OSAL file handle.
 *
 * @details
 * An `OSAL_FILE` designates a file resource acquired through
 * @ref osal_file_ops_t::open and released through
 * @ref osal_file_ops_t::close.
 *
 * On successful open, ownership of the acquired handle belongs to the caller
 * until the handle is released through the matching close operation.
 */
typedef struct OSAL_FILE OSAL_FILE;

/**
 * @brief Portable status codes for low-level OSAL file operations.
 */
typedef enum osal_file_status {
	OSAL_FILE_STATUS_OK = 0,
	OSAL_FILE_STATUS_INVALID,
	OSAL_FILE_STATUS_NOENT,
	OSAL_FILE_STATUS_PERM,
	OSAL_FILE_STATUS_EXISTS,
	OSAL_FILE_STATUS_NOSPC,
	OSAL_FILE_STATUS_NAMETOOLONG,
	OSAL_FILE_STATUS_NOTDIR,
	OSAL_FILE_STATUS_ISDIR,
	OSAL_FILE_STATUS_BADF,
	OSAL_FILE_STATUS_FBIG,
	OSAL_FILE_STATUS_INTR,
	OSAL_FILE_STATUS_MFILE,
	OSAL_FILE_STATUS_NFILE,
	OSAL_FILE_STATUS_LOOP,
	OSAL_FILE_STATUS_ROFS,
	OSAL_FILE_STATUS_SPIPE,
	OSAL_FILE_STATUS_XDEV,
	OSAL_FILE_STATUS_NODEV,
	OSAL_FILE_STATUS_NXIO,
	OSAL_FILE_STATUS_STALE,
	OSAL_FILE_STATUS_OOM,
	OSAL_FILE_STATUS_IO
} osal_file_status_t;

#endif // LEXLEO_OSAL_FILE_TYPES_H
