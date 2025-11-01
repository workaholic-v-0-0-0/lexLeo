// src/osal/src/osal_macos.c

#include "osal.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

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

FILE *osal_fmemopen_ro(const char *data, size_t len) {
    if (!data) return NULL;
    if (len == (size_t)-1) len = strlen(data);

#if defined(__APPLE__)
    struct cookie { const char *p; size_t i, n; };
    struct cookie *c = malloc(sizeof(*c));
    if (!c) return NULL;
    c->p = data; c->i = 0; c->n = len;

    int readfn(void *v, char *buf, int sz) {
        struct cookie *ck = v;
        if (ck->i >= ck->n) return 0;
        if ((size_t)sz > ck->n - ck->i) sz = (int)(ck->n - ck->i);
        memcpy(buf, ck->p + ck->i, (size_t)sz);
        ck->i += (size_t)sz;
        return sz;
    }
    int closefn(void *v) { free(v); return 0; }

    FILE *f = funopen(c, readfn, NULL, NULL, closefn);
    if (!f) { free(c); return NULL; }
    return f;
#else
    FILE *f = tmpfile();
    if (!f) return NULL;
    if (len && fwrite(data, 1, len, f) != len) { fclose(f); return NULL; }
    rewind(f);
    return f;
#endif
}
