/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_borrower.c
 * @ingroup stream_internal_group
 * @brief Borrower-facing stream operation implementation.
 *
 * @details
 * This file implements the public borrower operations used to read, write and
 * flush borrowed `stream_t` handles.
 */

#include "stream/borrowers/stream_borrowers_api.h"

#include "internal/stream_handle.h"

size_t stream_read(
	stream_t *s,
	void *buf,
	size_t n,
	stream_status_t *st
) {
    if (n == 0) {
		if (st) *st = STREAM_STATUS_OK;
		return (size_t)0;
	}

    if (!s || !buf) {
        if (st) *st = STREAM_STATUS_INVALID;
        return (size_t)0;
    }

    if (!s->backend) {
        if (st) *st = STREAM_STATUS_NO_BACKEND;
        return (size_t)0;
    }

    return s->vtbl->read(s->backend, buf, n, st);
}

size_t stream_write(
	stream_t *s,
	const void *buf,
	size_t n,
	stream_status_t *st
) {
    if (n == 0) {
		if (st) *st = STREAM_STATUS_OK;
		return (size_t)0;
	}

    if (!s || !buf) {
        if (st) *st = STREAM_STATUS_INVALID;
        return (size_t)0;
    }

    if (!s->backend) {
        if (st) *st = STREAM_STATUS_NO_BACKEND;
        return (size_t)0;
    }

    return s->vtbl->write(s->backend, buf, n, st);
}

stream_status_t stream_flush(stream_t *s)
{
    if (!s || !s->vtbl->flush) return STREAM_STATUS_INVALID;
    if (!s->backend) return STREAM_STATUS_NO_BACKEND;

    return s->vtbl->flush(s->backend);
}
