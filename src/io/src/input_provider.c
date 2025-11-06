// src/io/src/input_provider.c

#define INITIAL_SIZE_OF_BUFFER 256

struct dynamic_buffer {
    char *buf;
	size_t cap;
	size_t len;
};

struct input_provider {
    input_provider_mode mode;
    FILE *file;
    struct dynamic_buffer dbuf;
	yyscan_t lexer_scanner; // borrowed from lexer ;
			// if mode == INPUT_PROVIDER_MODE_CHUNKS, owns its YY_BUFFER_STATE field
			// if mode == INPUT_PROVIDER_MODE_FILE, doesn't use its YY_BUFFER_STATE field at all and hence doesn't own its YY_BUFFER_STATE field
	YY_BUFFER_STATE lexer_buffer_state;
};




/*

input_provider *input_provider_from_cstring(const char *src, bool copy) {
    if (!src) return NULL;
    input_provider *p = calloc(1, sizeof *p);
    if (!p) return NULL;
    p->kind = INPUT_FROM_CSTRING;

    p->src = src;
    p->len = strlen(src);
    p->f = osal_fmemopen_ro((void*)p->src, p->len);
    if (!p->f) { free(p); return NULL; }
    p->own_file = true;

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

*/