/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_file_ops.h
 * @ingroup osal_file_api
 * @brief Public low-level file operations API for the `osal_file` module.
 *
 * @details
 * This header exposes the OSAL-facing entry points used to:
 * - describe the portable low-level file operations table for the active
 *   platform,
 * - acquire, access, flush, and release `OSAL_FILE` resources through the
 *   `osal_file_ops_t` contract,
 * - retrieve the default operations table provided by the active OSAL file
 *   backend.
 */

#ifndef LEXLEO_OSAL_FILE_OPS_H
#define LEXLEO_OSAL_FILE_OPS_H

/**
 * @brief Operations table for the low-level OSAL file abstraction.
 *
 * @details
 * This API defines the low-level file operations exposed by the `osal_file`
 * module.
 *
 * It provides a portable file-oriented operations table for the active
 * platform, while keeping higher-level adapter concerns outside of the OSAL.
 */

#include "osal/file/osal_file_types.h"

#include "osal/mem/osal_mem_types.h"
#include "osal/str/osal_str_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Low-level file operations for the active OSAL backend.
 *
 * @details
 * This structure groups the primitive operations used to acquire, access,
 * flush, and release `OSAL_FILE` resources through the OS abstraction layer.
 *
 * These operations are intentionally file-oriented. They do not define a
 * higher-level byte-stream port contract.
 */
struct osal_file_ops_t {

	/**
	 * @brief Open a file resource.
	 *
	 * @param[out] out
	 * Receives the acquired `OSAL_FILE` handle on success.
	 *
	 * @param pathname
	 * Path of the file to open, expressed as a project-level UTF-8 path string.
	 * Platform-specific conversion is performed by the active OSAL backend.
	 *
	 * @param mode
	 * File access mode string. Only mode values supported by the portable
	 * `osal_file` contract are valid.
	 *
	 * @param mem_ops
	 * Memory operations table used to allocate and release the OSAL file
	 * handle.
	 *
	 * @return
	 * `OSAL_FILE_STATUS_OK` on success, or an error status on failure.
	 *
	 * @note
	 * On success, the acquired handle is owned by the caller and must be
	 * released through `osal_file_ops_t::close`.
	 */
	osal_file_status_t (*open)(
		OSAL_FILE **out,
		const char *pathname,
		const char *mode,
		const osal_mem_ops_t *mem_ops);

	/**
	 * @brief Read elements from an open `OSAL_FILE`.
	 *
	 * @param ptr
	 * Destination buffer.
	 *
	 * @param size
	 * Size in bytes of each element.
	 *
	 * @param nmemb
	 * Number of elements to read.
	 *
	 * @param stream
	 * Open `OSAL_FILE` handle.
	 *
	 * @param[out] st
	 * Receives the operation status.
	 *
	 * @return
	 * The number of elements successfully read.
	 *
	 * @note
	 * A return value smaller than `nmemb` does not by itself distinguish
	 * between end-of-file and failure; callers must inspect `st`.
	 */
	size_t (*read)(
		void *ptr,
		size_t size,
		size_t nmemb,
		OSAL_FILE *stream,
		osal_file_status_t *st);

	/**
	 * @brief Write elements to an open `OSAL_FILE`.
	 *
	 * @param ptr
	 * Source buffer.
	 *
	 * @param size
	 * Size in bytes of each element.
	 *
	 * @param nmemb
	 * Number of elements to write.
	 *
	 * @param stream
	 * Open `OSAL_FILE` handle.
	 *
	 * @param[out] st
	 * Receives the operation status.
	 *
	 * @return
	 * The number of elements successfully written.
	 */
	size_t (*write)(
		const void *ptr,
		size_t size,
		size_t nmemb,
		OSAL_FILE *stream,
		osal_file_status_t *st);

	/**
	 * @brief Flush buffered output associated with an open `OSAL_FILE`.
	 *
	 * @param stream
	 * Open `OSAL_FILE` handle.
	 *
	 * @return
	 * `OSAL_FILE_STATUS_OK` on success, or an error status on failure.
	 */
	osal_file_status_t (*flush)(OSAL_FILE *stream);

	/**
	 * @brief Close an open `OSAL_FILE` and release its associated resource.
	 *
	 * @param stream
	 * Open `OSAL_FILE` handle to close.
	 *
	 * @return
	 * `OSAL_FILE_STATUS_OK` on success, or an error status on failure.
	 */
	osal_file_status_t (*close)(OSAL_FILE *stream);

	char *(*gets)(
		char* out,
		size_t out_size,
		OSAL_FILE* stream,
		osal_file_status_t* st);

	osal_file_status_t (*mkdir)(const char *pathname);
};

/**
 * @brief Return the default OSAL file operations for the active platform.
 *
 * @details
 * This function exposes the platform-specific low-level file operations used
 * by the `osal_file` module.
 *
 * @return
 * A pointer to the default operations table for the active platform.
 *
 * @note
 * The returned operations table is not owned by the caller.
 */
const osal_file_ops_t *osal_file_default_ops(void);

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_OSAL_FILE_OPS_H
