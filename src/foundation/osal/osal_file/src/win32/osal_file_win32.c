// src/foundation/osal/osal_file/src/win32/osal_file_win32.c

#include "osal/file/internal/osal_file_env.h"
#include "osal/file/osal_file_ops.h"
#include "osal/file/osal_file_types.h"
#include "osal/mem/osal_mem_ops.h"
#include "policy/lexleo_panic.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <wchar.h>

struct osal_file_t {
    FILE *fp;
	const osal_mem_ops_t *mem;
};

static void set_status(osal_file_status_t *st, osal_file_status_t v)
{
    if (st) *st = v;
}

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

static const wchar_t *wmode_from_flags(uint32_t flags, osal_file_status_t *st)
{
    const bool r = (flags & OSAL_FILE_READ)   != 0;
    const bool w = (flags & OSAL_FILE_WRITE)  != 0;
    const bool a = (flags & OSAL_FILE_APPEND) != 0;
    const bool t = (flags & OSAL_FILE_TRUNC)  != 0;
    const bool c = (flags & OSAL_FILE_CREATE) != 0;

    if (a) {
        if (r) return L"a+b";
        return L"ab";
    }

    if (w) {
        if (t) {
            if (r) return L"w+b";
            return L"wb";
        }
        if (c) {
            if (r) return L"a+b";
            return L"ab";
        }
        if (r) return L"r+b";
        set_status(st, OSAL_FILE_ERR);
        return NULL;
    }

    if (r) return L"rb";

    set_status(st, OSAL_FILE_ERR);
    return NULL;
}

static wchar_t *utf8_to_wide(const char *s, const osal_mem_ops_t *mem) {
	if (!s || !mem || !mem->calloc || !mem->free) return NULL;
    int len = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
    if (len <= 0) return NULL;

    wchar_t *w = (wchar_t *)mem->calloc((size_t)len, sizeof(*w));
    if (!w) return NULL;

    int ok = MultiByteToWideChar(CP_UTF8, 0, s, -1, w, len);
    if (ok <= 0) {
        mem->free(w);
        return NULL;
    }
    return w;
}

static osal_file_t *win_open(
	const char *path_utf8,
	uint32_t flags,
	osal_file_status_t *st,
	const osal_file_env_t *env )
{
    if (st) *st = OSAL_FILE_ERR;
    if (!path_utf8 || !*path_utf8) return NULL;

    osal_file_env_t default_env;
    const osal_file_env_t *use_env = env;

    if (!use_env) {
        default_env = osal_file_default_env(osal_mem_default_ops());
        use_env = &default_env;
    }

    const osal_mem_ops_t *mem = use_env->mem;
    if (!mem || !mem->calloc || !mem->free) return NULL;

    const wchar_t *mode = wmode_from_flags(flags, st);
    if (!mode) return NULL;

    wchar_t *wpath = utf8_to_wide(path_utf8, mem);
    if (!wpath) {
        set_status(st, OSAL_FILE_ERR);
        return NULL;
    }

    errno = 0;
    FILE *fp = _wfopen(wpath, mode);
    mem->free(wpath);

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

static size_t win_read(osal_file_t *f, void *buf, size_t n, osal_file_status_t *st)
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
    }

    set_status(st, OSAL_FILE_OK);
    return got;
}

static size_t win_write(osal_file_t *f, const void *buf, size_t n, osal_file_status_t *st)
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

static osal_file_status_t win_flush(osal_file_t *f)
{
    if (!f || !f->fp) return OSAL_FILE_ERR;
    errno = 0;
    if (fflush(f->fp) != 0) return map_errno(errno);
    return OSAL_FILE_OK;
}

static osal_file_status_t win_close(osal_file_t *f)
{
    if (!f) return OSAL_FILE_OK;

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

const osal_file_ops_t *osal_file_win32_ops(void)
{
    static const osal_file_ops_t OPS = {
        .open  = win_open,
        .read  = win_read,
        .write = win_write,
        .flush = win_flush,
        .close = win_close,
    };
    return &OPS;
}
