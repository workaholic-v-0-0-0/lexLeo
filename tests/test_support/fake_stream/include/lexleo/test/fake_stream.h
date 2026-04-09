/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#ifndef LEXLEO_FAKE_STREAM_H
#define LEXLEO_FAKE_STREAM_H

#include "stream/borrowers/stream_types.h"
#include "stream/adapters/stream_env.h"
#include "stream/adapters/stream_adapters_api.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FAKE_STREAM_BUFFER_CAP
#define FAKE_STREAM_BUFFER_CAP 4096u
#endif

typedef struct fake_stream_t fake_stream_t;

typedef struct fake_stream_counters_t {
	size_t read_calls;
	size_t write_calls;
	size_t flush_calls;
	size_t close_calls;
} fake_stream_counters_t;

/**
 * @brief Create one fake backend and its associated public `stream_t`.
 *
 * @param[out] out_fake
 * Receives the fake backend handle used by tests for configuration and
 * observation.
 *
 * @param[out] out_stream
 * Receives the associated public `stream_t` handle to inject into the code
 * under test.
 *
 * @param[in] env
 * Stream environment used to allocate runtime objects.
 *
 * @retval STREAM_STATUS_OK
 * Creation succeeded.
 *
 * @retval STREAM_STATUS_INVALID
 * Invalid arguments.
 *
 * @retval STREAM_STATUS_OOM
 * Allocation failed.
 *
 * @note
 * The returned pair must be destroyed with @ref fake_stream_destroy.
 */
stream_status_t fake_stream_create(
	fake_stream_t **out_fake,
	stream_t **out_stream,
	const stream_env_t *env);

/**
 * @brief Destroy one fake/backend pair.
 *
 * @param[in,out] fake
 * Address of the fake handle. Set to `NULL` on return.
 *
 * @param[in,out] stream
 * Address of the associated public stream handle. Set to `NULL` on return.
 *
 * @details
 * This function delegates destruction of the associated public stream to the
 * real `stream_destroy()` lifecycle entry point. Destroying the public stream
 * also destroys the fake backend.
 */
void fake_stream_destroy(
	fake_stream_t **fake,
	stream_t **stream);

/**
 * @brief Reset observable state and configured runtime behavior of one fake.
 */
void fake_stream_reset(fake_stream_t *fs);

void fake_stream_set_write_result(
	fake_stream_t *fs,
	size_t n,
	stream_status_t status);

void fake_stream_set_flush_result(
	fake_stream_t *fs,
	stream_status_t status);

void fake_stream_fail_write_since(
	fake_stream_t *fs,
	size_t call_idx,
	stream_status_t status);

const fake_stream_counters_t *fake_stream_counters(const fake_stream_t *fs);

size_t fake_stream_written_len(const fake_stream_t *fs);
const uint8_t *fake_stream_written_data(const fake_stream_t *fs);

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_FAKE_STREAM_H