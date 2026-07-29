/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_handle.h
 * @ingroup stream_internal_group
 * @brief Private stream handle definition for the `stream` port.
 *
 * @details
 * This header defines the private handle structure used by the `stream` port
 * implementation.
 */

#ifndef LEXLEO_STREAM_HANDLE_H
#define LEXLEO_STREAM_HANDLE_H

#include "stream/common/stream_vtbl_type.h"

#include "osal/mem/osal_mem_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Private handle structure for a `stream_t`.
 *
 * @details
 * This header defines the private in-memory representation of a public
 * `stream_t` handle.
 */
struct stream_t {

	/** Borrowed adapter dispatch table. */
	const stream_vtbl_t *vtbl;

	/** Borrowed memory operations used to destroy the handle. */
	const osal_mem_ops_t *mem;

	/** Owned opaque adapter backend instance. */
	void *backend;

};

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_STREAM_HANDLE_H */
