// src/osal/src/osal_unix.c

#include "osal.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

#if defined(__GLIBC__) || defined(__linux__)
#define OSAL_HAVE_FMEMOPEN 1
#endif

#if !defined(OSAL_HAVE_FMEMOPEN)
// try fopencookie (GNU/BSD like)
#if defined(__USE_GNU) || defined(_GNU_SOURCE)
#define OSAL_HAVE_FOPENCOOKIE 1
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
static ssize_t osal_cookie_read(void *c, char *buf, size_t sz) {
    struct { const char *p; size_t i, n; } *cookie = c;
    if (cookie->i >= cookie->n) return 0;
    if (sz > cookie->n - cookie->i) sz = cookie->n - cookie->i;
    memcpy(buf, cookie->p + cookie->i, sz);
    cookie->i += sz;
    return (ssize_t)sz;
}
static int osal_cookie_close(void *c) { free(c); return 0; }
#endif
#endif

FILE *osal_fmemopen_ro(const char *data, size_t len) {
    if (!data) return NULL;
    if (len == (size_t)-1) len = strlen(data);

#if OSAL_HAVE_FMEMOPEN
    return fmemopen((void*)data, len, "r");
#elif defined(OSAL_HAVE_FOPENCOOKIE)
    struct { const char *p; size_t i, n; } *cookie = malloc(sizeof(*cookie));
    if (!cookie) return NULL;
    cookie->p = data; cookie->i = 0; cookie->n = len;
    cookie_io_functions_t io = {
        .read  = osal_cookie_read,
        .write = NULL,
        .seek  = NULL,
        .close = osal_cookie_close
    };
    return fopencookie(cookie, "r", io);
#else
    // portable fallback : copy in tmpfile
    FILE *f = tmpfile();
    if (!f) return NULL;
    if (len && fwrite(data, 1, len, f) != len) { fclose(f); return NULL; }
    rewind(f);
    return f;
#endif
}
