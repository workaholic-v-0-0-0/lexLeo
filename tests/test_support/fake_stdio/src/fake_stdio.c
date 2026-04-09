/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#include "lexleo/test/fake_stdio.h"

#include "osal/mem/osal_mem.h"

#include "lexleo_cmocka.h"

typedef struct fake_stdio_stream_t {
	/* spy */
	size_t read_call_count;
	size_t write_call_count;
	size_t flush_call_count;

	/* state */
	uint8_t buffered_backing[FAKE_STDIO_BUF_SIZE];
	uint8_t sink_backing[FAKE_STDIO_BUF_SIZE];
	size_t buffered_len;
	size_t sink_len;
	size_t read_pos;
} fake_stdio_stream_t;

static fake_stdio_stream_t *osal_stdio_stream_to_fake_stdio(OSAL_STDIO *stdio)
{
	return (fake_stdio_stream_t *)stdio;
}

static OSAL_STDIO *fake_stdio_to_osal_stdio_stream(fake_stdio_stream_t *fake)
{
	return (OSAL_STDIO *)fake;
}

static fake_stdio_stream_t g_fake_stdio_stdin = {
	.read_call_count = 0,
	.write_call_count = 0,
	.flush_call_count = 0,
	.buffered_backing = { 0 },
	.sink_backing = { 0 },
	.buffered_len = 0,
	.sink_len = 0,
	.read_pos = 0
};

static fake_stdio_stream_t g_fake_stdio_stdout = {
	.read_call_count = 0,
	.write_call_count = 0,
	.flush_call_count = 0,
	.buffered_backing = { 0 },
	.sink_backing = { 0 },
	.buffered_len = 0,
	.sink_len = 0,
	.read_pos = 0
};

static fake_stdio_stream_t g_fake_stdio_stderr = {
	.read_call_count = 0,
	.write_call_count = 0,
	.flush_call_count = 0,
	.buffered_backing = { 0 },
	.sink_backing = { 0 },
	.buffered_len = 0,
	.sink_len = 0,
	.read_pos = 0
};

static void fake_stdio_stream_reset(fake_stdio_stream_t *fake_stdio)
{
	assert_non_null(fake_stdio);

	fake_stdio->read_call_count = 0;
	fake_stdio->write_call_count = 0;
	fake_stdio->flush_call_count = 0;

	osal_memset(fake_stdio->buffered_backing, 0, FAKE_STDIO_BUF_SIZE);
	osal_memset(fake_stdio->sink_backing, 0, FAKE_STDIO_BUF_SIZE);

	fake_stdio->buffered_len = 0;
	fake_stdio->sink_len = 0;
	fake_stdio->read_pos = 0;
}

void fake_stdio_reset(void)
{
	fake_stdio_stream_reset(&g_fake_stdio_stdin);
	fake_stdio_stream_reset(&g_fake_stdio_stdout);
	fake_stdio_stream_reset(&g_fake_stdio_stderr);
}

void fake_stdio_set_buffered_backing(
	OSAL_STDIO *stdio,
	const uint8_t *backing,
	size_t len)
{
	fake_stdio_stream_t *fake = osal_stdio_stream_to_fake_stdio(stdio);

	assert_non_null(fake);
	assert_true(backing || len == 0);
	assert_true(len <= FAKE_STDIO_BUF_SIZE);

	if (len > 0) {
		osal_memcpy(fake->buffered_backing, backing, len);
	}
	if (len < FAKE_STDIO_BUF_SIZE) {
		osal_memset(fake->buffered_backing + len, 0, FAKE_STDIO_BUF_SIZE - len);
	}

	fake->buffered_len = len;
	fake->read_pos = 0;
}

void fake_stdio_set_sink_backing(
	OSAL_STDIO *stdio,
	const uint8_t *backing,
	size_t len)
{
	fake_stdio_stream_t *fake = osal_stdio_stream_to_fake_stdio(stdio);

	assert_non_null(fake);
	assert_true(backing || len == 0);
	assert_true(len <= FAKE_STDIO_BUF_SIZE);

	if (len > 0) {
		osal_memcpy(fake->sink_backing, backing, len);
	}
	if (len < FAKE_STDIO_BUF_SIZE) {
		osal_memset(fake->sink_backing + len, 0, FAKE_STDIO_BUF_SIZE - len);
	}

	fake->sink_len = len;
}

void fake_stdio_set_read_pos(OSAL_STDIO *stdio, size_t n)
{
	fake_stdio_stream_t *fake = osal_stdio_stream_to_fake_stdio(stdio);

	assert_non_null(fake);
	assert_true(n <= fake->buffered_len);

	fake->read_pos = n;
}

size_t fake_stdio_read_call_count(OSAL_STDIO *stdio)
{
	fake_stdio_stream_t *fake = osal_stdio_stream_to_fake_stdio(stdio);
	assert_non_null(fake);
	return fake->read_call_count;
}

size_t fake_stdio_write_call_count(OSAL_STDIO *stdio)
{
	fake_stdio_stream_t *fake = osal_stdio_stream_to_fake_stdio(stdio);
	assert_non_null(fake);
	return fake->write_call_count;
}

size_t fake_stdio_flush_call_count(OSAL_STDIO *stdio)
{
	fake_stdio_stream_t *fake = osal_stdio_stream_to_fake_stdio(stdio);
	assert_non_null(fake);
	return fake->flush_call_count;
}

const uint8_t *fake_stdio_buffered_backing(OSAL_STDIO *stdio)
{
	fake_stdio_stream_t *fake = osal_stdio_stream_to_fake_stdio(stdio);
	assert_non_null(fake);
	return fake->buffered_backing;
}

const uint8_t *fake_stdio_sink_backing(OSAL_STDIO *stdio)
{
	fake_stdio_stream_t *fake = osal_stdio_stream_to_fake_stdio(stdio);
	assert_non_null(fake);
	return fake->sink_backing;
}

size_t fake_stdio_buffered_len(OSAL_STDIO *stdio)
{
	fake_stdio_stream_t *fake = osal_stdio_stream_to_fake_stdio(stdio);
	assert_non_null(fake);
	return fake->buffered_len;
}

size_t fake_stdio_sink_len(OSAL_STDIO *stdio)
{
	fake_stdio_stream_t *fake = osal_stdio_stream_to_fake_stdio(stdio);
	assert_non_null(fake);
	return fake->sink_len;
}

size_t fake_stdio_read_pos(OSAL_STDIO *stdio)
{
	fake_stdio_stream_t *fake = osal_stdio_stream_to_fake_stdio(stdio);
	assert_non_null(fake);
	return fake->read_pos;
}

OSAL_STDIO *fake_stdio_stdin(void)
{
	return fake_stdio_to_osal_stdio_stream(&g_fake_stdio_stdin);
}

OSAL_STDIO *fake_stdio_stdout(void)
{
	return fake_stdio_to_osal_stdio_stream(&g_fake_stdio_stdout);
}

OSAL_STDIO *fake_stdio_stderr(void)
{
	return fake_stdio_to_osal_stdio_stream(&g_fake_stdio_stderr);
}

size_t fake_stdio_read(
	void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_STDIO *stdio)
{
	fake_stdio_stream_t *fake;
	size_t requested_bytes;
	size_t available_bytes;
	size_t readable_bytes;
	size_t readable_nmemb;

	if (!ptr || !stdio || size == 0 || nmemb == 0) {
		return 0;
	}

	fake = osal_stdio_stream_to_fake_stdio(stdio);
	fake->read_call_count++;

	if (fake->read_pos >= fake->buffered_len) {
		return 0;
	}

	requested_bytes = size * nmemb;
	available_bytes = fake->buffered_len - fake->read_pos;
	readable_bytes = (requested_bytes < available_bytes)
		? requested_bytes
		: available_bytes;

	readable_nmemb = readable_bytes / size;
	readable_bytes = readable_nmemb * size;

	if (readable_bytes > 0) {
		osal_memcpy(ptr, fake->buffered_backing + fake->read_pos, readable_bytes);
		fake->read_pos += readable_bytes;
	}

	return readable_nmemb;
}

size_t fake_stdio_write(
	const void *ptr,
	size_t size,
	size_t nmemb,
	OSAL_STDIO *stdio)
{
	fake_stdio_stream_t *fake;
	size_t requested_bytes;
	size_t available_bytes;
	size_t writable_bytes;
	size_t writable_nmemb;

	if (!ptr || !stdio || size == 0 || nmemb == 0) {
		return 0;
	}

	fake = osal_stdio_stream_to_fake_stdio(stdio);
	fake->write_call_count++;

	if (fake->buffered_len >= FAKE_STDIO_BUF_SIZE) {
		return 0;
	}

	requested_bytes = size * nmemb;
	available_bytes = FAKE_STDIO_BUF_SIZE - fake->buffered_len;
	writable_bytes = (requested_bytes < available_bytes)
		? requested_bytes
		: available_bytes;

	writable_nmemb = writable_bytes / size;
	writable_bytes = writable_nmemb * size;

	if (writable_bytes > 0) {
		osal_memcpy(fake->buffered_backing + fake->buffered_len, ptr, writable_bytes);
		fake->buffered_len += writable_bytes;
	}

	return writable_nmemb;
}

int fake_stdio_flush(OSAL_STDIO *stdio)
{
	fake_stdio_stream_t *fake;
	size_t flushable_bytes;
	size_t available_sink_bytes;

	if (!stdio) {
		return -1;
	}

	fake = osal_stdio_stream_to_fake_stdio(stdio);
	fake->flush_call_count++;

	if (fake->buffered_len == 0) {
		return 0;
	}

	if (fake->sink_len >= FAKE_STDIO_BUF_SIZE) {
		return 0;
	}

	available_sink_bytes = FAKE_STDIO_BUF_SIZE - fake->sink_len;
	flushable_bytes = (fake->buffered_len < available_sink_bytes)
		? fake->buffered_len
		: available_sink_bytes;

	if (flushable_bytes > 0) {
		osal_memcpy(
			fake->sink_backing + fake->sink_len,
			fake->buffered_backing,
			flushable_bytes);
		fake->sink_len += flushable_bytes;
	}

	if (flushable_bytes < fake->buffered_len) {
		osal_memmove(
			fake->buffered_backing,
			fake->buffered_backing + flushable_bytes,
			fake->buffered_len - flushable_bytes);
	}

	if (fake->buffered_len > flushable_bytes) {
		osal_memset(
			fake->buffered_backing + (fake->buffered_len - flushable_bytes),
			0,
			flushable_bytes);
	} else {
		osal_memset(fake->buffered_backing, 0, FAKE_STDIO_BUF_SIZE);
	}

	fake->buffered_len -= flushable_bytes;

	if (fake->read_pos > fake->buffered_len) {
		fake->read_pos = fake->buffered_len;
	}

	return 0;
}

int fake_stdio_error(OSAL_STDIO *stdio)
{
	(void)stdio;
	return 0;
}

int fake_stdio_eof(OSAL_STDIO *stdio)
{
	fake_stdio_stream_t *fake;

	if (!stdio) {
		return 0;
	}

	fake = osal_stdio_stream_to_fake_stdio(stdio);
	return fake->read_pos >= fake->buffered_len;
}

void fake_stdio_clearerr(OSAL_STDIO *stdio)
{
	(void)stdio;
}
