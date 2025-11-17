// src/osal/src/osal_macos.c

#include "osal.h"
#include "osal_config.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

void osal_sleep(int ms) {
    struct timespec req = {
        .tv_sec  = ms / 1000,
        .tv_nsec = (ms % 1000) * 1000000
    };
    nanosleep(&req, NULL);
}

void osal_open_in_web_browser(const char *filepath) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "open \"%s\" &", filepath);
    system(cmd);
}

typedef struct {
    const char *p;
    size_t i;
    size_t n;
} osal_ro_cookie_t;

static int osal_ro_readfn(void *v, char *buf, int sz) {
    osal_ro_cookie_t *ck = v;
    if (!ck || sz <= 0) return 0;
    if (ck->i >= ck->n) return 0;
    size_t avail = ck->n - ck->i;
    size_t to_copy = (size_t)sz;
    if (to_copy > avail) to_copy = avail;
    memcpy(buf, ck->p + ck->i, to_copy);
    ck->i += to_copy;
    return (int)to_copy;
}

static int osal_ro_closefn(void *v) {
    free(v);
    return 0;
}

FILE *osal_fmemopen_ro(const char *data, size_t len) {
    if (!data) return NULL;
    if (len == (size_t)-1) len = strlen(data);
#if OSAL_HAVE_FMEMOPEN
    return fmemopen((void*)data, len, "r");
#else
    osal_ro_cookie_t *c = malloc(sizeof(*c));
    if (!c) return NULL;
    c->p = data;
    c->i = 0;
    c->n = len;
    FILE *f = funopen(
        c,
        osal_ro_readfn,
        NULL,
        NULL,
        osal_ro_closefn
    );
    if (!f) {
        free(c);
        return NULL;
    }
    return f;
#endif
}

typedef struct {
    char **out_buf;
    size_t *out_len;
    size_t cap;
} osal_wo_cookie_t;

static int osal_wo_writefn(void *v, const char *buf, int sz) {
    osal_wo_cookie_t *cookie = v;
    if (!cookie || !cookie->out_buf || !cookie->out_len) return -1;
    if (sz <= 0) return 0;
    size_t usz = (size_t)sz;
    size_t needed = *cookie->out_len + usz + 1; // +1 for '\0'
    if (needed > cookie->cap) {
        size_t new_cap = cookie->cap ? cookie->cap : 64;
        while (new_cap < needed) new_cap *= 2;
        char *new_buf = realloc(*cookie->out_buf, new_cap);
        if (!new_buf) {
            errno = ENOMEM;
            return -1;
        }
        *cookie->out_buf = new_buf;
        cookie->cap = new_cap;
    }
    memcpy(*cookie->out_buf + *cookie->out_len, buf, usz);
    *cookie->out_len += usz;
    (*cookie->out_buf)[*cookie->out_len] = '\0';
    return sz;
}

static int osal_wo_closefn(void *v) {
    free(v);
    return 0;
}

FILE *osal_open_memstream(char **out_buf, size_t *out_len) {
    if (!out_buf || !out_len) return NULL;
    *out_buf = NULL;
    *out_len = 0;
#if OSAL_HAVE_OPEN_MEMSTREAM
    return open_memstream(out_buf, out_len);
#else
    osal_wo_cookie_t *cookie = malloc(sizeof(*cookie));
    if (!cookie) return NULL;
    cookie->out_buf = out_buf;
    cookie->out_len = out_len;
    cookie->cap     = 0;
    *out_buf = NULL;
    *out_len = 0;
    FILE *f = funopen(
        cookie,
        NULL,
        osal_wo_writefn,
        NULL,
        osal_wo_closefn
    );
    if (!f) {
        free(cookie);
        return NULL;
    }
    return f;
#endif
}
