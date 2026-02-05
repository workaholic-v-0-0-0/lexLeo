/* SPDX-License-Identifier: GPL-3.0-or-later
* Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file stream_handle.h
 * @ingroup stream_internal_group
 * @brief Private stream handle definition for the `stream` port.
 *
 * @details
 * This header exposes the private handle structure used by the `stream` port
 * implementation.
 */

#ifndef LEXLEO_STREAM_HANDLE_H
#define LEXLEO_STREAM_HANDLE_H

#include "stream/adapters/stream_adapters_api.h"
#include "osal/mem/osal_mem_ops.h"

/**
 * @brief Private handle structure for a `stream_t`.
 *
 * @details
 * This structure is the private in-memory handle bound to a public `stream_t`
 * object.
 *
 * It stores:
 * - the bound adapter dispatch table,
 * - the opaque backend instance,
 * - the memory operations used for destruction.
 */
struct stream_t {
	/** Bound adapter dispatch table. */
	stream_vtbl_t vtbl;

	/** Opaque adapter-owned backend instance. */
	void *backend;

	/** Memory operations used to destroy the handle. */
	const osal_mem_ops_t *mem;
};

#endif // LEXLEO_STREAM_HANDLE_H
