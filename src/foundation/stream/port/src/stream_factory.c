// src/foundation/stream/src/stream_factory.c

#include "stream/owners/stream_factory.h"
#include "stream/cr/stream_factory_cr_api.h"
#include "internal/stream_factory_handle.h"

#include <string.h>
#include <stddef.h>

static const stream_branch_t *stream_registry_find(
	const stream_registry_t *reg,
	stream_key_t key)
{
	if (!reg || !reg->entries || !reg->count || !key) return NULL;

	for (size_t i = 0; i < reg->count; i++) {
		const stream_branch_t *e = &reg->entries[i];
		if (e->key && strcmp(e->key, key) == 0) return e;
	}
	return NULL;
}

static stream_status_t stream_registry_create(
	const stream_registry_t *reg,
	stream_key_t key,
	const void *args,
	stream_t **out_stream)
{
	if (!out_stream) return STREAM_STATUS_INVALID;
	*out_stream = NULL;

	const stream_branch_t *e = stream_registry_find(reg, key);
	if (!e || !e->ctor) return STREAM_STATUS_NO_BACKEND;

	return e->ctor(e->ud, args, out_stream);
}

stream_status_t stream_factory_create_stream(
	const stream_factory_t *f,
	stream_key_t key,
	const void *args,
	stream_t **out)
{
	if (!out) return STREAM_STATUS_INVALID;
	*out = NULL;
	if (!f || !key) return STREAM_STATUS_INVALID;
	return stream_registry_create(&f->reg, key, args, out);
}

stream_status_t stream_create_empty_factory(
	stream_factory_t **out,
	size_t capacity,
	const stream_ctx_t *ctx )
{
	if (out) *out = NULL;

    if (!out || !ctx || !ctx->mem || !ctx->mem->calloc || !ctx->mem->free) {
        return STREAM_STATUS_INVALID;
    }

    stream_factory_t *f = (stream_factory_t *)ctx->mem->calloc(1, sizeof(*f));
    if (!f) {
        return STREAM_STATUS_OOM;
    }

    f->mem = ctx->mem;
    f->reg.entries = NULL;
    f->reg.count = 0;
    f->reg.cap = 0;

    if (capacity > 0) {
        f->reg.entries = (stream_branch_t *)ctx->mem->calloc(
            capacity, sizeof(*f->reg.entries));
        if (!f->reg.entries) {
            ctx->mem->free(f);
            return STREAM_STATUS_OOM;
        }
        f->reg.cap = capacity;
    }

    *out = f;
    return STREAM_STATUS_OK;
}

static int stream_key_equals(stream_key_t a, stream_key_t b) {
    if (a == b) return 1;
    if (!a || !b) return 0;

    const char *pa = (const char *)a;
    const char *pb = (const char *)b;

    while (*pa && *pb) {
        if (*pa != *pb) return 0;
        ++pa; ++pb;
    }
    return *pa == *pb;
}

stream_status_t stream_destroy_factory(stream_factory_t **fact)
{
    if (!fact || !*fact) {
        return STREAM_STATUS_INVALID;
    }

    stream_factory_t *f = *fact;
    *fact = NULL;

    const osal_mem_ops_t *mem = f->mem;
    if (!mem || !mem->free) {
        return STREAM_STATUS_INVALID;
    }

    if (f->reg.entries) {
        mem->free(f->reg.entries);
        f->reg.entries = NULL;
    }
    f->reg.count = 0;
    f->reg.cap = 0;

    mem->free(f);
    return STREAM_STATUS_OK;
}

stream_status_t stream_factory_add_adapter(
    stream_factory_t *fact,
    stream_key_t key,
    stream_ctor_fn_t ctor,
    void *ud)
{
    if (!fact || !key || !ctor || !ud) {
        return STREAM_STATUS_INVALID;
    }

    stream_registry_t *reg = &fact->reg;

    if (!reg->entries || reg->cap == 0) {
        return STREAM_STATUS_INVALID;
    }

    if (reg->count >= reg->cap) {
        return STREAM_STATUS_OOM;
    }

    // Enforce uniqueness
    for (size_t i = 0; i < reg->count; ++i) {
        if (stream_key_equals(reg->entries[i].key, key)) {
            return STREAM_STATUS_INVALID; /* duplicate key */
        }
    }

    reg->entries[reg->count++] = (stream_branch_t){
        .key  = key,
        .ctor = ctor,
        .ud   = ud
    };

    return STREAM_STATUS_OK;
}