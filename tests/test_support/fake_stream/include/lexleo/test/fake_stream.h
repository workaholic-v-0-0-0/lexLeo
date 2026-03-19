#ifndef LEXLEO_FAKE_STREAM_H
#define LEXLEO_FAKE_STREAM_H

#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_env.h"
#include "stream/adapters/stream_adapters_api.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct fake_stream_counters_t {
	size_t read_calls;
	size_t write_calls;
	size_t flush_calls;
	size_t close_calls;
} fake_stream_counters_t;

void fake_stream_reset(void);

void fake_stream_set_write_result(size_t n, stream_status_t status);
void fake_stream_set_flush_result(stream_status_t status);
void fake_stream_fail_write_since(size_t call_idx, stream_status_t status);

const fake_stream_counters_t *fake_stream_counters(void);

size_t fake_stream_written_len(void);
const uint8_t *fake_stream_written_data(void);

stream_status_t fake_stream_create(
	stream_t **out,
	const stream_env_t *env);

#endif //LEXLEO_FAKE_STREAM_H
