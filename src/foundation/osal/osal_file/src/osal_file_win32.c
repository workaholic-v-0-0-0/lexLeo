/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_file_win32.c
 * @ingroup osal_file_internal_group
 * @brief Win32 / CRT backend implementation of the `osal_file` module.
 *
 * @details
 * This translation unit provides the active low-level `osal_file` operations
 * for Windows platforms.
 *
 * It implements:
 * - platform-to-OSAL status mapping for CRT `errno` values,
 * - platform-to-OSAL status mapping for Win32 `GetLastError()` values,
 * - UTF-8 to UTF-16 pathname conversion for wide-character file opening,
 * - file acquisition through `_wfopen()`,
 * - element-oriented reads and writes through `fread()` and `fwrite()`,
 * - buffered output flushing through `fflush()`,
 * - resource release through `fclose()`,
 * - publication of the default `osal_file_ops_t` table for this backend.
 */

#include "internal/osal_file_internal.h"

#include "osal/file/osal_file_ops.h"

#include "osal/mem/osal_mem_ops.h"
#include "osal/mem/osal_mem.h"

#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstd_errno.h"
#include "policy/lexleo_cstd_io.h"
#include "policy/lexleo_cstd_wchar.h"
#include "policy/lexleo_cstring.h"
#include "policy/lexleo_win32_base.h"
#include "policy/lexleo_assert.h"

/**
 * @brief Map a CRT `errno` value to an `osal_file_status_t`.
 *
 * @param errnum
 * CRT error value to translate.
 *
 * @return
 * The corresponding portable OSAL file status.
 *
 * @note
 * Unrecognized CRT errors are mapped to `OSAL_FILE_STATUS_IO`.
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

/**
 * @brief Map a Win32 error code to an `osal_file_status_t`.
 *
 * @param err
 * Win32 error code to translate.
 *
 * @return
 * The corresponding portable OSAL file status.
 *
 * @note
 * Unrecognized Win32 errors are mapped to `OSAL_FILE_STATUS_IO`.
 */
static osal_file_status_t osal_file_win32_error(DWORD err)
{
	switch (err) {
		case ERROR_SUCCESS:
			return OSAL_FILE_STATUS_OK;

		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
			return OSAL_FILE_STATUS_NOENT;

		case ERROR_ACCESS_DENIED:
		case ERROR_SHARING_VIOLATION:
			return OSAL_FILE_STATUS_PERM;

		case ERROR_ALREADY_EXISTS:
		case ERROR_FILE_EXISTS:
			return OSAL_FILE_STATUS_EXISTS;

		case ERROR_DISK_FULL:
		case ERROR_HANDLE_DISK_FULL:
			return OSAL_FILE_STATUS_NOSPC;

		case ERROR_FILENAME_EXCED_RANGE:
			return OSAL_FILE_STATUS_NAMETOOLONG;

		case ERROR_DIRECTORY:
			return OSAL_FILE_STATUS_NOTDIR;

		case ERROR_INVALID_NAME:
		case ERROR_BAD_PATHNAME:
		case ERROR_INVALID_PARAMETER:
		case ERROR_NO_UNICODE_TRANSLATION:
			return OSAL_FILE_STATUS_INVALID;

		case ERROR_TOO_MANY_OPEN_FILES:
			return OSAL_FILE_STATUS_MFILE;

		case ERROR_NOT_READY:
		case ERROR_DEV_NOT_EXIST:
			return OSAL_FILE_STATUS_NODEV;

		case ERROR_OUTOFMEMORY:
		case ERROR_NOT_ENOUGH_MEMORY:
			return OSAL_FILE_STATUS_OOM;

		case ERROR_WRITE_PROTECT:
			return OSAL_FILE_STATUS_ROFS;

		default:
			return OSAL_FILE_STATUS_IO;
	}
}

/**
 * @brief Return the wide-character mode string matching a portable file mode.
 *
 * @param mode
 * Portable file access mode string.
 *
 * @return
 * The corresponding wide-character mode string, or `NULL` if `mode` is not
 * supported by the portable `osal_file` contract.
 */
static const wchar_t *osal_file_mode_w(const char *mode)
{
	if (osal_strcmp(mode, "rb") == 0)
		return L"rb";

	if (osal_strcmp(mode, "wb") == 0)
		return L"wb";

	if (osal_strcmp(mode, "ab") == 0)
		return L"ab";

	return NULL;
}

/**
 * @brief Duplicate a UTF-8 string as a UTF-16 wide-character string.
 *
 * @param[out] out_wstr
 * Receives the allocated UTF-16 duplicate on success.
 *
 * @param utf8
 * Source UTF-8 string.
 *
 * @param mem_ops
 * Memory operations table used to allocate the duplicate.
 *
 * @return
 * `OSAL_FILE_STATUS_OK` on success, or an error status on failure.
 *
 * @note
 * This helper uses `MultiByteToWideChar()` with `CP_UTF8` and
 * `MB_ERR_INVALID_CHARS`.
 */
static osal_file_status_t osal_utf8_to_utf16_dup(
	wchar_t **out_wstr,
	const char *utf8,
	const osal_mem_ops_t *mem_ops)
{
	LEXLEO_ASSERT(out_wstr);
	LEXLEO_ASSERT(mem_ops && mem_ops->malloc && mem_ops->free);

	*out_wstr = NULL;

	if (!utf8)
		return OSAL_FILE_STATUS_INVALID;

	int wlen = MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		utf8,
		-1,
		NULL,
		0);

	if (wlen <= 0)
		return osal_file_win32_error(GetLastError());

	size_t bytes = (size_t)wlen * sizeof(wchar_t);

	wchar_t *tmp = mem_ops->malloc(bytes);
	if (!tmp)
		return OSAL_FILE_STATUS_OOM;

	int converted = MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		utf8,
		-1,
		tmp,
		wlen);

	if (converted <= 0) {
		mem_ops->free(tmp);
		return osal_file_win32_error(GetLastError());
	}

	*out_wstr = tmp;
	return OSAL_FILE_STATUS_OK;
}

/**
 * @brief Open a file resource through the Win32 / CRT backend.
 *
 * @details
 * This private callback implements the public
 * `osal_file_ops_t::open` contract for the active Windows backend.
 *
 * @param[out] out
 * Receives the acquired `OSAL_FILE` handle on success.
 *
 * @param pathname
 * Pathname of the file resource to open, expressed as a project-level UTF-8
 * string.
 *
 * @param mode
 * Portable file access mode string.
 *
 * @param mem_ops
 * Memory operations table used to allocate the OSAL file wrapper and
 * temporary pathname conversion buffers.
 *
 * @return
 * `OSAL_FILE_STATUS_OK` on success, or an error status on failure.
 */
osal_file_status_t osal_file_open(
	OSAL_FILE **out,
	const char *pathname,
	const char *mode,
	const osal_mem_ops_t *mem_ops)
{
	if (
		   !out
		|| !pathname
		|| pathname[0] == '\0'
		|| !mode
		|| (
			   osal_strcmp(mode, "rb") != 0
			&& osal_strcmp(mode, "wb") != 0
			&& osal_strcmp(mode, "ab") != 0)
		|| !mem_ops
	) {
		return OSAL_FILE_STATUS_INVALID;
	}

	LEXLEO_ASSERT(
		   mem_ops->malloc
		&& mem_ops->free
	);

	const wchar_t *wmode = osal_file_mode_w(mode);
	if (!wmode)
		return OSAL_FILE_STATUS_INVALID;

	wchar_t *wpath = NULL;
	osal_file_status_t st = osal_utf8_to_utf16_dup(&wpath, pathname, mem_ops);
	if (st != OSAL_FILE_STATUS_OK)
		return st;

	FILE *fp = _wfopen(wpath, wmode);

	mem_ops->free(wpath);
	wpath = NULL;

	if (!fp) {
		/*
		 * _wfopen() is a CRT call; errno is the primary error channel here.
		 */
		return osal_file_map_errno(errno);
	}

	OSAL_FILE *tmp = mem_ops->malloc(sizeof(*tmp));
	if (!tmp) {
		fclose(fp);
		return OSAL_FILE_STATUS_OOM;
	}

	tmp->fp = fp;
	tmp->mem_ops = mem_ops;

	*out = tmp;

	return OSAL_FILE_STATUS_OK;
}

/**
 * @brief Read elements from an open `OSAL_FILE`.
 *
 * @details
 * This private callback implements the public `osal_file_ops_t::read`
 * contract for the active Windows backend.
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
size_t osal_file_read(
	void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_FILE *stream,
	osal_file_status_t *st)
{
	if (
		   !ptr
		|| !st
		|| !stream
		|| !stream->fp
	) {
		if (st)
			*st = OSAL_FILE_STATUS_INVALID;
		return 0;
	}

	*st = OSAL_FILE_STATUS_OK;

	size_t ret = fread(ptr, size, nmemb, stream->fp);

	if (ret < nmemb && ferror(stream->fp))
		*st = osal_file_map_errno(errno);

	return ret;
}

/**
 * @brief Write elements to an open `OSAL_FILE`.
 *
 * @details
 * This private callback implements the public `osal_file_ops_t::write`
 * contract for the active Windows backend.
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
size_t osal_file_write(
	const void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_FILE *stream,
	osal_file_status_t *st)
{
	if (
		   !ptr
		|| !st
		|| !stream
		|| !stream->fp
	) {
		if (st)
			*st = OSAL_FILE_STATUS_INVALID;
		return 0;
	}

	*st = OSAL_FILE_STATUS_OK;

	size_t ret = fwrite(ptr, size, nmemb, stream->fp);

	if (ret < nmemb && ferror(stream->fp))
		*st = osal_file_map_errno(errno);

	return ret;
}

/**
 * @brief Flush buffered output associated with an open `OSAL_FILE`.
 *
 * @details
 * This private callback implements the public `osal_file_ops_t::flush`
 * contract for the active Windows backend.
 *
 * @param stream
 * Open `OSAL_FILE` handle.
 *
 * @return
 * `OSAL_FILE_STATUS_OK` on success, or an error status on failure.
 */
osal_file_status_t osal_file_flush(OSAL_FILE *stream)
{
	if (!stream || !stream->fp)
		return OSAL_FILE_STATUS_INVALID;

	if (fflush(stream->fp) != 0)
		return osal_file_map_errno(errno);

	return OSAL_FILE_STATUS_OK;
}

/**
 * @brief Close an open `OSAL_FILE` and release its associated wrapper.
 *
 * @details
 * This private callback implements the public `osal_file_ops_t::close`
 * contract for the active Windows backend.
 *
 * @param stream
 * Open `OSAL_FILE` handle to close.
 *
 * @return
 * `OSAL_FILE_STATUS_OK` on success, or an error status on failure.
 */
osal_file_status_t osal_file_close(OSAL_FILE *stream)
{
	if (!stream || !stream->fp)
		return OSAL_FILE_STATUS_INVALID;

	if (fclose(stream->fp) != 0)
		return osal_file_map_errno(errno);

	LEXLEO_ASSERT(stream->mem_ops && stream->mem_ops->free);
	stream->mem_ops->free(stream);

	return OSAL_FILE_STATUS_OK;
}

char* osal_file_gets(
	char* out,
	size_t out_size,
	OSAL_FILE* stream,
	osal_file_status_t* st)
{
	char* ret;

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

/**
 * @brief Return the default Win32 / CRT OSAL file operations table.
 *
 * @return
 * A pointer to the default `osal_file_ops_t` table for this backend.
 */
const osal_file_ops_t *osal_file_default_ops(void)
{
	static const osal_file_ops_t osal_file_ops = {
		.open  = osal_file_open,
		.read  = osal_file_read,
		.write = osal_file_write,
		.close = osal_file_close,
		.flush = osal_file_flush
	};

	return &osal_file_ops;
}
