// src/io/src/input_provider.c

struct input_provider {
    input_source_kind_t kind;
    FILE *f;
    bool own_file;

    char *buf;            // si copie
    const char *src;      // si pas de copie
    size_t len;
    bool own_buffer;
};

input_provider *input_provider_from_cstring(const char *src, bool copy) {
    if (!src) return NULL;
    input_provider *p = calloc(1, sizeof *p);
    if (!p) return NULL;
    p->kind = INPUT_FROM_CSTRING;

    if (copy) {
        p->len = strlen(src);
        p->buf = malloc(p->len + 1);
        if (!p->buf) { free(p); return NULL; }
        memcpy(p->buf, src, p->len + 1);
        p->own_buffer = true;
        // wrap buf en FILE*
        p->f = osal_fmemopen_ro(p->buf, p->len);
        if (!p->f) { free(p->buf); free(p); return NULL; }
        p->own_file = true; // on ferme ce FILE* à destroy
    } else {
        p->src = src;
        p->len = strlen(src);
        p->f = osal_fmemopen_ro((void*)p->src, p->len);
        if (!p->f) { free(p); return NULL; }
        p->own_file = true;
    }
    return p;
}

bool input_provider_bind_to_scanner(input_provider *p, yyscan_t scanner) {
    if (!p || !p->f || !scanner) return false;
    yyset_in(p->f, scanner);
    return true;
}

void input_provider_destroy(input_provider *p) {
    if (!p) return;
    if (p->own_file && p->f) fclose(p->f);
    if (p->own_buffer && p->buf) free(p->buf);
    free(p);
}
