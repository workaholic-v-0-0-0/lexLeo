/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_CTX_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_CTX_H

#include "internal/stream_ctx.h" // stream port / vtable contract
#include "osal_mem_ops.h"

/**
 * @brief Dependencies required by the dynamic buffer stream adapter.
 *
 * This structure groups all external dependencies injected
 * into the the dynamic buffer stream backend at creation time.
 */
typedef struct dynamic_buffer_stream_deps_t {
	/** Memory operations used by the backend */
	const osal_mem_ops_t *mem;
} dynamic_buffer_stream_deps_t;

/**
 * @brief dynamic buffer stream adapter context.
 *
 * This context describes how the dynamic buffer stream adapter is wired:
 * - memory operations (OSAL)
 * - stream virtual table
 *
 * The context is consumed at creation time and is not retained
 * by the stream instance afterwards.
 */
typedef struct dynamic_buffer_stream_ctx_t {
	/** Injected dependencies. */
	dynamic_buffer_stream_deps_t deps;

	/** stream virtual table used by the port stream module. */
	stream_vtbl_t stream_vtbl;
} dynamic_buffer_stream_ctx_t;

/**
 * @brief Create a stream instance backed by a dynamic_buffer_stream.
 *
 * @param[out] out  Created stream instance
 * @param[in]  ctx  dynamic_buffer_stream adapter context (consumed at creation time)
 *
 * @return STREAM_STATUS_OK on success, STREAM_STATUS_ERROR otherwise
 */
stream_status_t dynamic_buffer_stream_create_stream(
	stream_t **out,
	const dynamic_buffer_stream_ctx_t *ctx );

/**
 * @brief Create a default dynamic_buffer_stream adapter context.
 *
 * This helper initializes a context using default OSAL memory
 * operations when NULL arguments are provided.
 *
 * @param[in] mem_ops  Memory operations (or NULL for default)
 *
 * @return Initialized dynamic_buffer_stream adapter context
 */
dynamic_buffer_stream_ctx_t dynamic_buffer_stream_default_ctx(
	const osal_mem_ops_t *mem_ops );

#endif //LEXLEO_DYNAMIC_BUFFER_STREAM_CTX_H
