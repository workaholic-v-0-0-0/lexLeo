// src/foundation/osal/src/osal_unix.c

#include "osal.h"
#include "osal_config.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void osal_sleep(int ms) {
    struct timespec req = {
        .tv_sec  = ms / 1000,
        .tv_nsec = (ms % 1000) * 1000000
    };
    nanosleep(&req, NULL);
}

void osal_open_in_web_browser(const char *filepath) {
    int ret = system("command -v firefox > /dev/null 2>&1");
    char cmd[1024];
    if (ret == 0) {
        snprintf(cmd, sizeof(cmd), "firefox --new-window \"%s\" 2>/dev/null &", filepath);
    } else {
        snprintf(cmd, sizeof(cmd), "xdg-open \"%s\" 2>/dev/null &", filepath);
    }
    system(cmd);
}


#if OSAL_HAVE_FOPENCOOKIE
#include <unistd.h>
#include <sys/types.h>

typedef struct {
    const char *p;
    size_t i;
    size_t n;
} osal_ro_cookie_t;

static ssize_t osal_cookie_read(void *c, char *buf, size_t sz) {
    osal_ro_cookie_t *cookie = (osal_ro_cookie_t *)c;
    if (cookie->i >= cookie->n) return 0;
    if (sz > cookie->n - cookie->i) sz = cookie->n - cookie->i;
    memcpy(buf, cookie->p + cookie->i, sz);
    cookie->i += sz;
    return (ssize_t)sz;
}
static int osal_cookie_ro_close(void *c) {
	free(c);
	return 0;
}
#endif // OSAL_HAVE_FOPENCOOKIE

FILE *osal_fmemopen_ro(const char *data, size_t len) {
    if (!data) return NULL;
    if (len == (size_t)-1) len = strlen(data);

#if OSAL_HAVE_FMEMOPEN
    return fmemopen((void*)data, len, "r");

#elif OSAL_HAVE_FOPENCOOKIE
    osal_ro_cookie_t *cookie = malloc(sizeof(*cookie));
    if (!cookie) return NULL;
    cookie->p = data;
	cookie->i = 0;
	cookie->n = len;
    cookie_io_functions_t io = (cookie_io_functions_t){
        .read  = osal_cookie_read,
        .write = NULL,
        .seek  = NULL,
        .close = osal_cookie_ro_close
    };
    return fopencookie(cookie, "r", io);
#else
    // portable fallback : copy in tmpfile
    FILE *f = tmpfile();
    if (!f) return NULL;
    if (len && fwrite(data, 1, len, f) != len) {
		fclose(f);
		return NULL;
	}
    rewind(f);
    return f;
#endif
}

#if OSAL_HAVE_FOPENCOOKIE

typedef struct {
    char **out_buf;
    size_t *out_len;
    size_t cap;
} osal_wo_cookie_t;

static ssize_t osal_cookie_wo_write(void *c, const char *buf, size_t sz) {
    osal_wo_cookie_t *cookie = (osal_wo_cookie_t *)c;
	if (!cookie || !cookie->out_buf || !cookie->out_len) return -1;
    if (sz == 0) return 0;
    size_t needed = *cookie->out_len + sz + 1; // +1 for '\0'
    if (needed > cookie->cap) {
        size_t new_cap = cookie->cap ? cookie->cap : 64;
        while (new_cap < needed) new_cap *= 2;
        char *new_buf = realloc(*cookie->out_buf, new_cap);
        if (!new_buf) return -1;
        *cookie->out_buf = new_buf;
        cookie->cap = new_cap;
	}
    memcpy(*cookie->out_buf + *cookie->out_len, buf, sz);
    *cookie->out_len += sz;
    (*cookie->out_buf)[*cookie->out_len] = '\0';
    return (ssize_t)sz;
}

static int osal_cookie_wo_close(void *c) {
	free(c);
	return 0;
}
#endif // OSAL_HAVE_FOPENCOOKIE

FILE *osal_open_memstream(char **out_buf, size_t *out_len) {
    if (!out_buf || !out_len) return NULL;
    *out_buf = NULL;
    *out_len = 0;

#if OSAL_HAVE_OPEN_MEMSTREAM
    return open_memstream(out_buf, out_len);

#elif OSAL_HAVE_FOPENCOOKIE
    osal_wo_cookie_t *cookie = malloc(sizeof(osal_wo_cookie_t));
    if (!cookie) return NULL;
    cookie->out_buf = out_buf;
    cookie->out_len = out_len;
    cookie->cap     = 0;
    *out_buf = NULL;
    *out_len = 0;

    cookie_io_functions_t io = {
        .read  = NULL,
        .write = osal_cookie_wo_write,
        .seek  = NULL,
        .close = osal_cookie_wo_close
    };

    FILE *f = fopencookie(cookie, "w+", io);
    if (!f) {
        free(cookie);
        return NULL;
    }
    return f;

#else
    (void)out_buf;
    (void)out_len;
    // Platform does not support open_memstream-like behavior.
    // Caller must handle a NULL return value.
    errno = ENOSYS;
    return NULL;
#endif
}
