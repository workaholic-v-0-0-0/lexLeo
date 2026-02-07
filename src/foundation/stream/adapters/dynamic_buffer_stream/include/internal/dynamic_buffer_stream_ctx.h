/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/adapters/stream/dynamic_buffer_stream/include/internal/
 * dynamic_buffer_stream_ctx.h
 */

#ifndef LEXLEO_DYNAMIC_BUFFER_STREAM_CTX_H
#define LEXLEO_DYNAMIC_BUFFER_STREAM_CTX_H

#include "internal/stream_ctx.h" // stream port / vtable contract
#include "dynamic_buffer_stream_factory.h"
#include "osal_mem_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

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
	const stream_vtbl_t *stream_vtbl;
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

/**
 * @brief Create a dynamic_buffer_stream factory (heap handle) for dependency
 *        injection.
 *
 * This function allocates and initializes a factory object, and returns it
 * via @p out.
 *
 * The factory stores the allocator (`osal_mem_ops_t`) needed for its own
 * destruction, so it can be destroyed safely without requiring a wiring
 * context.
 *
 * @note Lifetime rules:
 * - The pointers stored in the factory (e.g. `mem`, `vtbl`) must reference
 *   lifetime-stable storage (VM-owned or static).
 * - They must never point to stack-owned objects.
 *
 * @param[out] out  Receives the newly created factory handle.
 *                  Set to NULL on error.
 * @param[in]  ctx  Wiring context (consumed at creation time; not retained).
 *
 * @return STREAM_STATUS_OK on success, STREAM_STATUS_ERROR otherwise.
 */
stream_status_t dynamic_buffer_stream_create_factory(
    dynamic_buffer_stream_factory_t **out,
    const dynamic_buffer_stream_ctx_t *ctx
);

/**
 * @brief Destroy a dynamic_buffer_stream factory handle.
 *
 * This function releases all resources owned by the factory (including its
 * userdata), then frees the factory itself and sets @p *fact to NULL.
 *
 * This function is NULL-safe: passing NULL or a NULL handle is a no-op.
 *
 * @param[in,out] fact  Pointer to the factory handle to destroy.
 *                      Set to NULL on success.
 *
 * @return STREAM_STATUS_OK on success (or if already NULL),
 *         STREAM_STATUS_ERROR on invariant breakage (e.g. missing allocator).
 */
stream_status_t dynamic_buffer_stream_destroy_factory(
    dynamic_buffer_stream_factory_t **fact
);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //LEXLEO_DYNAMIC_BUFFER_STREAM_CTX_H
