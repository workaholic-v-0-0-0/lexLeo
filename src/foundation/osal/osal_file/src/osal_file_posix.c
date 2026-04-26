/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_file_posix.c
 * @ingroup osal_file_internal_group
 * @brief POSIX / C stdio backend implementation of the `osal_file` module.
 *
 * @details
 * This translation unit provides the active low-level `osal_file` operations
 * for POSIX-like platforms through the C standard I/O runtime.
 *
 * It implements:
 * - platform-to-OSAL status mapping for `errno`,
 * - file acquisition through `fopen()`,
 * - element-oriented reads and writes through `fread()` and `fwrite()`,
 * - buffered output flushing through `fflush()`,
 * - resource release through `fclose()`,
 * - publication of the default `osal_file_ops_t` table for this backend.
 */

#include "internal/osal_file_internal.h"

#include "osal/file/osal_file_ops.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/osal_mem.h"
#include "osal/str/osal_str.h"

#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_io.h"
#include "policy/lexleo_cstd_errno.h"
#include "policy/lexleo_cstd_limits.h"
#include "policy/lexleo_assert.h"
#include "policy/lexleo_posix_sys_stat.h"

/**
 * @brief Map a platform `errno` value to an `osal_file_status_t`.
 *
 * @param errnum
 * Platform error value to translate.
 *
 * @return
 * The corresponding portable OSAL file status.
 *
 * @note
 * Unrecognized platform errors are mapped to `OSAL_FILE_STATUS_IO`.
 */
static osal_file_status_t osal_file_map_errno(int errnum)
{
	switch (errnum) {
		case 0:
			return OSAL_FILE_STATUS_OK;

#ifdef EINVAL
		case EINVAL:
			return OSAL_FILE_STATUS_INVALID;
#endif

#ifdef ENOENT
		case ENOENT:
			return OSAL_FILE_STATUS_NOENT;
#endif

#ifdef EACCES
		case EACCES:
			return OSAL_FILE_STATUS_PERM;
#endif

#ifdef EPERM
		case EPERM:
			return OSAL_FILE_STATUS_PERM;
#endif

#ifdef EEXIST
		case EEXIST:
			return OSAL_FILE_STATUS_EXISTS;
#endif

#ifdef ENOSPC
		case ENOSPC:
			return OSAL_FILE_STATUS_NOSPC;
#endif

#ifdef ENAMETOOLONG
		case ENAMETOOLONG:
			return OSAL_FILE_STATUS_NAMETOOLONG;
#endif

#ifdef ENOTDIR
		case ENOTDIR:
			return OSAL_FILE_STATUS_NOTDIR;
#endif

#ifdef EISDIR
		case EISDIR:
			return OSAL_FILE_STATUS_ISDIR;
#endif

#ifdef EBADF
		case EBADF:
			return OSAL_FILE_STATUS_BADF;
#endif

#ifdef EFBIG
		case EFBIG:
			return OSAL_FILE_STATUS_FBIG;
#endif

#ifdef EINTR
		case EINTR:
			return OSAL_FILE_STATUS_INTR;
#endif

#ifdef EMFILE
		case EMFILE:
			return OSAL_FILE_STATUS_MFILE;
#endif

#ifdef ENFILE
		case ENFILE:
			return OSAL_FILE_STATUS_NFILE;
#endif

#ifdef ELOOP
		case ELOOP:
			return OSAL_FILE_STATUS_LOOP;
#endif

#ifdef EROFS
		case EROFS:
			return OSAL_FILE_STATUS_ROFS;
#endif

#ifdef ESPIPE
		case ESPIPE:
			return OSAL_FILE_STATUS_SPIPE;
#endif

#ifdef EXDEV
		case EXDEV:
			return OSAL_FILE_STATUS_XDEV;
#endif

#ifdef ENODEV
		case ENODEV:
			return OSAL_FILE_STATUS_NODEV;
#endif

#ifdef ENXIO
		case ENXIO:
			return OSAL_FILE_STATUS_NXIO;
#endif

#ifdef ESTALE
		case ESTALE:
			return OSAL_FILE_STATUS_STALE;
#endif

#ifdef ENOMEM
		case ENOMEM:
			return OSAL_FILE_STATUS_OOM;
#endif

		default:
			return OSAL_FILE_STATUS_IO;
	}
}

static osal_file_status_t osal_file_open(
	OSAL_FILE **out,
	const char *pathname,
	const char *mode,
	const osal_mem_ops_t *mem_ops
) {
	if (
		   !out
		|| !pathname
		|| pathname[0] == '\0'
		|| !mode
		|| !mem_ops
	) {
		return OSAL_FILE_STATUS_INVALID;
	}

	if (
		   osal_strcmp(mode, "rb") != 0
		&& osal_strcmp(mode, "wb") != 0
		&& osal_strcmp(mode, "ab") != 0
	) {
		return OSAL_FILE_STATUS_INVALID;
	}

	LEXLEO_ASSERT(
		   mem_ops->malloc
		&& mem_ops->free
	);

	OSAL_FILE *tmp = mem_ops->malloc(sizeof(*tmp));
	if (!tmp)
		return OSAL_FILE_STATUS_OOM;

	tmp->mem_ops = mem_ops;

	FILE *fp = fopen(pathname, mode);
	if (!fp) {
		mem_ops->free(tmp);
		return osal_file_map_errno(errno);
	}

	tmp->fp = fp;

	*out = tmp;

	return OSAL_FILE_STATUS_OK;
}

/**
 * @brief Read elements from an open `OSAL_FILE`.
 *
 * @details
 * This private callback implements the public `osal_file_ops_t::read`
 * contract for the active POSIX-like backend.
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
 */
static size_t osal_file_read(
	void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_FILE *stream,
	osal_file_status_t *st
) {
	if (
		   !ptr
		|| !st
		|| !stream
		|| !stream->fp
	) {
		if (st) *st = OSAL_FILE_STATUS_INVALID;
		return 0;
	}

	*st = OSAL_FILE_STATUS_OK;

	size_t ret = fread(ptr, size, nmemb, stream->fp);

	if (ret < nmemb && ferror(stream->fp)) {
		*st = osal_file_map_errno(errno);
	}

	return ret;
}

/**
 * @brief Write elements to an open `OSAL_FILE`.
 *
 * @details
 * This private callback implements the public `osal_file_ops_t::write`
 * contract for the active POSIX-like backend.
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
static size_t osal_file_write(
	const void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_FILE *stream,
	osal_file_status_t *st
) {
	if (
		   !ptr
		|| !st
		|| !stream
		|| !stream->fp
	) {
		if (st) *st = OSAL_FILE_STATUS_INVALID;
		return 0;
	}

	*st = OSAL_FILE_STATUS_OK;

	size_t ret = fwrite(ptr, size, nmemb, stream->fp);

	if (ret < nmemb && ferror(stream->fp)) {
		*st = osal_file_map_errno(errno);
	}

	return ret;
}

/**
 * @brief Flush buffered output associated with an open `OSAL_FILE`.
 *
 * @details
 * This private callback implements the public `osal_file_ops_t::flush`
 * contract for the active POSIX-like backend.
 *
 * @param stream
 * Open `OSAL_FILE` handle.
 *
 * @return
 * `OSAL_FILE_STATUS_OK` on success, or an error status on failure.
 */
static osal_file_status_t osal_file_flush(OSAL_FILE *stream)
{
	if (!stream || !stream->fp) {
		return OSAL_FILE_STATUS_INVALID;
	}

	if (fflush(stream->fp) != 0) {
		return osal_file_map_errno(errno);
	}

	return OSAL_FILE_STATUS_OK;
}

/**
 * @brief Close an open `OSAL_FILE` and release its associated wrapper.
 *
 * @details
 * This private callback implements the public `osal_file_ops_t::close`
 * contract for the active POSIX-like backend.
 *
 * @param stream
 * Open `OSAL_FILE` handle to close.
 *
 * @return
 * `OSAL_FILE_STATUS_OK` on success, or an error status on failure.
 */
static osal_file_status_t osal_file_close(OSAL_FILE *stream)
{
	if (!stream || !stream->fp) {
		return OSAL_FILE_STATUS_INVALID;
	}

	if (fclose(stream->fp) != 0) {
		return osal_file_map_errno(errno);
	}

	LEXLEO_ASSERT(stream->mem_ops && stream->mem_ops->free);
	stream->mem_ops->free(stream);

	return OSAL_FILE_STATUS_OK;
}

static char *osal_file_gets(
	char *out,
	size_t out_size,
	OSAL_FILE *stream,
	osal_file_status_t *st
)
{
	char *ret;

	if (
		!out
		|| out_size == 0
		|| out_size > (size_t)INT_MAX
		|| !stream
		|| !stream->fp
		|| !st
	) {
		if (st)
			*st = OSAL_FILE_STATUS_INVALID;
		return NULL;
	}

	*st = OSAL_FILE_STATUS_OK;

	ret = fgets(out, (int)out_size, stream->fp);
	if (ret)
		return out;

	if (ferror(stream->fp))
		*st = osal_file_map_errno(errno);

	return NULL;
}

static osal_file_status_t osal_file_mkdir(const char *pathname)
{
	if (!pathname || pathname[0] == '\0')
		return OSAL_FILE_STATUS_INVALID;

	if (mkdir(pathname, 0755) == 0)
		return OSAL_FILE_STATUS_OK;

	return osal_file_map_errno(errno);
}


/**
 * @brief Return the default POSIX / C stdio OSAL file operations table.
 *
 * @return
 * A pointer to the default `osal_file_ops_t` table for this backend.
 */
const osal_file_ops_t *osal_file_default_ops(void)
{
	static const osal_file_ops_t osal_file_ops = {
		.open = osal_file_open,
		.read = osal_file_read,
		.write = osal_file_write,
		.close = osal_file_close,
		.flush = osal_file_flush,
		.gets = osal_file_gets,
		.mkdir = osal_file_mkdir
	};
	return &osal_file_ops;
}
