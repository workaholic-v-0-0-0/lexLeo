#ifndef LEXLEO_STREAM_FAKE_PROVIDER_H
#define LEXLEO_STREAM_FAKE_PROVIDER_H

#include "stream/borrowers/stream_types.h"
#include "policy/lexleo_cstd_types.h"

typedef struct stream_fake_t stream_fake_t;

typedef struct stream_fake_counters_t {
	size_t read_calls;
	size_t write_calls;
	size_t flush_calls;
	size_t close_calls;
} stream_fake_counters_t;

stream_status_t stream_fake_create(
	stream_fake_t **out_fake,
	stream_t **out_stream,
	const osal_mem_ops_t *mem);

void stream_fake_destroy(
	stream_fake_t **fake,
	stream_t **stream);

void stream_fake_reset(stream_fake_t *fake);

void stream_fake_set_write_result(
	stream_fake_t *fake,
	size_t n,
	stream_status_t status);

void stream_fake_set_flush_result(
	stream_fake_t *fake,
	stream_status_t status);

void stream_fake_fail_write_since(
	stream_fake_t *fake,
	size_t call_idx,
	stream_status_t status);

const stream_fake_counters_t *stream_fake_counters(const stream_fake_t *fake);

size_t stream_fake_written_len(const stream_fake_t *fake);
const uint8_t *stream_fake_written_data(const stream_fake_t *fake);

#endif // LEXLEO_STREAM_FAKE_PROVIDER_H
