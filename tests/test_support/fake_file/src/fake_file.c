// tests/test_support/fake_file/src/fake_file.c

#include "lexleo/test/fake_file.h"

#include "policy/lexleo_assert.h"
#include <string.h>

typedef struct fake_file_handle_t {
    uint8_t *buf;
    size_t cap;
    size_t len;
    size_t pos;
    bool is_open;

    osal_file_env_t env; // to free it
} fake_file_handle_t;

static uint8_t *g_backing;
static size_t g_backing_cap;
static size_t g_backing_len;
static size_t g_next_pos;

static struct {
    bool enabled;
    fake_file_op_t op;
    size_t fail_at_call;
    osal_file_status_t status;
} g_fail;

static fake_file_counters_t g_ctrs;

static bool should_fail(fake_file_op_t op, size_t call_idx) {
    return g_fail.enabled && g_fail.op == op && call_idx >= g_fail.fail_at_call;
}

void fake_file_reset(void) {
    g_backing = NULL;
    g_backing_cap = 0;
    g_backing_len = 0;
    g_next_pos = 0;

    memset(&g_fail, 0, sizeof(g_fail));
    memset(&g_ctrs, 0, sizeof(g_ctrs));
}

void fake_file_set_backing(uint8_t *backing_buf, size_t cap, size_t initial_len) {
    g_backing = backing_buf;
    g_backing_cap = cap;
    g_backing_len = (initial_len <= cap) ? initial_len : cap;
    if (g_next_pos > g_backing_len) g_next_pos = g_backing_len;
}

void fake_file_set_pos(size_t pos) {
    g_next_pos = pos;
    if (g_next_pos > g_backing_len) g_next_pos = g_backing_len;
}

void fake_file_fail_disable(void) {
    g_fail.enabled = false;
}

void fake_file_fail_enable(fake_file_op_t op, size_t fail_at_call, osal_file_status_t status) {
    g_fail.enabled = true;
    g_fail.op = op;
    g_fail.fail_at_call = fail_at_call ? fail_at_call : 1;
    g_fail.status = status;
}

const fake_file_counters_t *fake_file_counters(void) {
    return &g_ctrs;
}

size_t fake_file_backing_len(void) {
    return g_backing_len;
}

osal_file_t *fake_file_open(
    const char *path_utf8,
    uint32_t flags,
    osal_file_status_t *status,
    const osal_file_env_t *env)
{
    (void)flags;
    g_ctrs.open_calls++;

    if (status) *status = OSAL_FILE_OK;

    if (!env || !env->mem || !env->mem->calloc || !env->mem->free) {
        if (status) *status = OSAL_FILE_ERR;
        return NULL;
    }

    if (!path_utf8 || *path_utf8 == '\0') {
        if (status) *status = OSAL_FILE_NOENT;
        return NULL;
    }

    if (!g_backing || g_backing_cap == 0) {
        if (status) *status = OSAL_FILE_NOENT;
        return NULL;
    }

    if (should_fail(FAKE_FILE_OP_OPEN, g_ctrs.open_calls)) {
        if (status) *status = g_fail.status;
        return NULL;
    }

    fake_file_handle_t *h = env->mem->calloc(1, sizeof(*h));
    if (!h) {
        if (status) *status = OSAL_FILE_ERR;
        return NULL;
    }

    h->buf = g_backing;
    h->cap = g_backing_cap;
    h->len = g_backing_len;
    h->pos = (g_next_pos <= h->len) ? g_next_pos : h->len;
    h->is_open = true;
    h->env = *env;

    return (osal_file_t *)h;
}

size_t fake_file_read(
    osal_file_t *f,
    void *buf,
    size_t n,
    osal_file_status_t *status)
{
    g_ctrs.read_calls++;
    if (status) *status = OSAL_FILE_OK;

    fake_file_handle_t *h = (fake_file_handle_t *)f;
    if (!h || !h->is_open || (!buf && n)) {
        if (status) *status = OSAL_FILE_ERR;
        return 0;
    }

    if (should_fail(FAKE_FILE_OP_READ, g_ctrs.read_calls)) {
        if (status) *status = g_fail.status;
        return 0;
    }

    if (n == 0) return 0;

    if (h->pos >= h->len) {
        if (status) *status = OSAL_FILE_EOF;
        return 0;
    }

    size_t avail = h->len - h->pos;
    size_t r = (n < avail) ? n : avail;

    memcpy(buf, h->buf + h->pos, r);
    h->pos += r;

    return r;
}

size_t fake_file_write(
    osal_file_t *f,
    const void *buf,
    size_t n,
    osal_file_status_t *status)
{
    g_ctrs.write_calls++;
    if (status) *status = OSAL_FILE_OK;

    fake_file_handle_t *h = (fake_file_handle_t *)f;
    if (!h || !h->is_open || (!buf && n)) {
        if (status) *status = OSAL_FILE_ERR;
        return 0;
    }

    if (should_fail(FAKE_FILE_OP_WRITE, g_ctrs.write_calls)) {
        if (status) *status = g_fail.status;
        return 0;
    }

    if (n == 0) return 0;

    if (h->pos >= h->cap) {
        if (status) *status = OSAL_FILE_IO;
        return 0;
    }

    size_t space = h->cap - h->pos;
    size_t w = (n < space) ? n : space;

    memcpy(h->buf + h->pos, buf, w);
    h->pos += w;
    if (h->pos > h->len) h->len = h->pos;

    g_backing_len = h->len;

    if (w < n) {
        if (status) *status = OSAL_FILE_IO;
    }

    return w;
}

osal_file_status_t fake_file_flush(osal_file_t *f) {
    g_ctrs.flush_calls++;

    fake_file_handle_t *h = (fake_file_handle_t *)f;
    if (!h || !h->is_open) return OSAL_FILE_ERR;

    if (should_fail(FAKE_FILE_OP_FLUSH, g_ctrs.flush_calls)) {
        return g_fail.status;
    }

    return OSAL_FILE_OK;
}

osal_file_status_t fake_file_close(osal_file_t *f) {
    g_ctrs.close_calls++;

    fake_file_handle_t *h = (fake_file_handle_t *)f;
    if (!h) return OSAL_FILE_ERR;

    if (should_fail(FAKE_FILE_OP_CLOSE, g_ctrs.close_calls)) {
        return g_fail.status;
    }

    h->is_open = false;

    LEXLEO_ASSERT(h->env.mem && h->env.mem->free);
    h->env.mem->free(h);

    return OSAL_FILE_OK;
}
