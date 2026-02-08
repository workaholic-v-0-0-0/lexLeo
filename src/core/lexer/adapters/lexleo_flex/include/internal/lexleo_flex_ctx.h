/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/core/lexer/adapters/lexleo_flex/include/internal/
 * lexleo_flex_ctx.h
 */

#ifndef LEXLEO_FLEX_CTX_H
#define LEXLEO_FLEX_CTX_H

#include "internal/lexer_ctx.h" // lexer port / vtable contract
#include "mem/osal_mem_ops.h"
#include "input_provider_legacy_ops.h"

/**
 * @brief Dependencies required by the Flex lexer adapter.
 *
 * This structure groups all external dependencies injected
 * into the Flex backend at creation time.
 */
typedef struct lexleo_flex_deps_t {
	/** Memory operations used by the backend and the Flex scanner. */
	const osal_mem_ops_t *mem;

	/** Input provider operations used to read input data. */
	const input_provider_legacy_ops_t *prov_ops;
} lexleo_flex_deps_t;

/**
 * @brief Flex-based lexer adapter context.
 *
 * This context describes how the Flex backend is wired:
 * - memory operations (OSAL)
 * - input provider operations
 * - lexer virtual table
 *
 * The context is consumed at creation time and is not retained
 * by the lexer instance afterwards.
 */
typedef struct lexleo_flex_ctx_t {
	/** Injected dependencies. */
	lexleo_flex_deps_t deps;

	/** Lexer virtual table used by the core lexer module. */
	lexer_vtbl_t lexer_vtbl;
} lexleo_flex_ctx_t;

/**
 * @brief Create a lexer instance backed by a Flex scanner.
 *
 * @param[out] out  Created lexer instance
 * @param[in]  prov Input provider
 * @param[in]  ctx  Flex adapter context (consumed at creation time)
 *
 * @return LEXER_STATUS_OK on success, LEXER_STATUS_ERROR otherwise
 */
lexer_status_t lexleo_flex_create_lexer(
	lexer_t **out,
	struct input_provider *prov,
	const lexleo_flex_ctx_t *ctx );

/**
 * @brief Create a default Flex adapter context.
 *
 * This helper initializes a context using default OSAL memory
 * operations and default input provider operations when NULL
 * arguments are provided.
 *
 * @param[in] mem_ops  Memory operations (or NULL for default)
 * @param[in] prov_ops Input provider operations (or NULL for default)
 *
 * @return Initialized Flex adapter context
 */
lexleo_flex_ctx_t lexleo_flex_default_ctx(
	const osal_mem_ops_t *mem_ops,
	const input_provider_legacy_ops_t *prov_ops );

#endif //LEXLEO_FLEX_CTX_H
