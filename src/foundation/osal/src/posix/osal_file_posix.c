// src/foundation/osal/src/posix/osal_file_posix.c

#include "file/internal/osal_file_ctx.h"
#include "file/osal_file_types.h"
#include "file/osal_file_ops.h"
#include "mem/osal_mem_ops.h"
#include "lexleo_panic.h"

#include <errno.h>
#include <stdio.h>
#include <stdbool.h>

struct osal_file_t {
    FILE *fp;
	const osal_mem_ops_t *mem;
};

static osal_file_status_t map_errno(int e) {
    switch (e) {
        case 0:      return OSAL_FILE_OK;
#ifdef ENOENT
        case ENOENT: return OSAL_FILE_NOENT;
#endif
#ifdef EACCES
        case EACCES: return OSAL_FILE_PERM;
#endif
#ifdef EPERM
        case EPERM:  return OSAL_FILE_PERM;
#endif
        default:     return OSAL_FILE_IO;
    }
}

static void set_status(osal_file_status_t *st, osal_file_status_t v) {
    if (st) *st = v;
}

static const char *mode_from_flags(uint32_t flags, osal_file_status_t *st) {
    const bool r = (flags & OSAL_FILE_READ)   != 0;
    const bool w = (flags & OSAL_FILE_WRITE)  != 0;
    const bool a = (flags & OSAL_FILE_APPEND) != 0;
    const bool t = (flags & OSAL_FILE_TRUNC)  != 0;
    const bool c = (flags & OSAL_FILE_CREATE) != 0;

    // Notes:
    // - stdio doesn't have a pure "create if missing" mode without opening
    //   for write.
    // - We approximate common cases for fs_stream needs.

    if (a) {
        // append implies write
        if (r) return "a+b";
        return "ab";
    }

    if (w) {
        if (t) {
            if (r) return "w+b";
            return "wb";
        }
        if (c) {
            // create if missing; stdio can't do O_CREAT|O_EXCL easily, but "a" creates
            // without truncation. We'll open "ab" and later seek is irrelevant for stream.
            if (r) return "a+b";
            return "ab";
        }
        // "r+b" requires file exists
        if (r) return "r+b";
        // write only without trunc/create is ambiguous -> error
        set_status(st, OSAL_FILE_ERR);
        return NULL;
    }

    if (r) {
        return "rb";
    }

    set_status(st, OSAL_FILE_ERR);
    return NULL;
}

static osal_file_t *posix_open(
	const char *path_utf8,
	uint32_t flags,
	osal_file_status_t *st,
	const osal_file_ctx_t *ctx )
{
    if (st) *st = OSAL_FILE_ERR;
	if (!path_utf8 || !*path_utf8) return NULL;

    osal_file_ctx_t default_ctx;
    const osal_file_ctx_t *use_ctx = ctx;

    if (!use_ctx) {
        default_ctx = osal_file_default_ctx(osal_mem_default_ops());
        use_ctx = &default_ctx;
    }

    const osal_mem_ops_t *mem = use_ctx->deps.mem;
    if (!mem || !mem->calloc || !mem->free) return NULL;

    const char *mode = mode_from_flags(flags, st);
    if (!mode) return NULL;

    errno = 0;
    FILE *fp = fopen(path_utf8, mode);
    if (!fp) {
        set_status(st, map_errno(errno));
        return NULL;
    }

    osal_file_t *f = (osal_file_t *)mem->calloc(1, sizeof(*f));
    if (!f) {
        fclose(fp);
        set_status(st, OSAL_FILE_ERR);
        return NULL;
    }

    f->fp = fp;
	f->mem = mem;

    set_status(st, OSAL_FILE_OK);
    return f;
}

static size_t posix_read(
	osal_file_t *f,
	void *buf,
	size_t n,
	osal_file_status_t *st )
{
    if (st) *st = OSAL_FILE_ERR;
    if (!f || !f->fp || (!buf && n)) return 0;

    errno = 0;
    size_t got = fread(buf, 1, n, f->fp);

    if (got < n) {
        if (ferror(f->fp)) {
            clearerr(f->fp);
            set_status(st, map_errno(errno));
            return got;
        }
        // EOF is not an error
    }

    set_status(st, OSAL_FILE_OK);
    return got;
}

static size_t posix_write(
	osal_file_t *f,
	const void *buf,
	size_t n,
	osal_file_status_t *st )
{
    if (st) *st = OSAL_FILE_ERR;
    if (!f || !f->fp || (!buf && n)) return 0;

    errno = 0;
    size_t put = fwrite(buf, 1, n, f->fp);

    if (put < n) {
        set_status(st, map_errno(errno));
        return put;
    }

    set_status(st, OSAL_FILE_OK);
    return put;
}

static osal_file_status_t posix_flush(osal_file_t *f)
{
    if (!f || !f->fp) return OSAL_FILE_ERR;
    errno = 0;
    if (fflush(f->fp) != 0) return map_errno(errno);
    return OSAL_FILE_OK;
}

static osal_file_status_t posix_close(osal_file_t *f) {
    if (!f) return OSAL_FILE_OK; // idempotent close

    osal_file_status_t st = OSAL_FILE_OK;

    if (f->fp) {
        errno = 0;
        if (fclose(f->fp) != 0) st = map_errno(errno);
        f->fp = NULL;
    }

    if (!f->mem || !f->mem->free) {
		// log
		lexleo_panic("osal_file: invalid allocator in close");
		return OSAL_FILE_ERR;
	}

    f->mem->free(f);
    return st;
}

const osal_file_ops_t *osal_file_posix_ops(void)
{
    static const osal_file_ops_t OPS = {
        .open  = posix_open,
        .read  = posix_read,
        .write = posix_write,
        .flush = posix_flush,
        .close = posix_close,
    };
    return &OPS;
}
