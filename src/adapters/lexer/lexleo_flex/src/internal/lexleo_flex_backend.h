/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * Internal Flex backend structure.
 *
 * This header defines the concrete backend state used by the
 * Flex-based lexer implementation.
 *
 * Not part of the public API.
 */

#ifndef LEXLEO_FLEX_BACKEND_H
#define LEXLEO_FLEX_BACKEND_H

#include "internal/lexleo_flex_state.h"
#include "osal_mem_ops.h"

struct osal_mem_ops_t;

typedef struct lexleo_flex_t {
	lexleo_flex_state_t state;
	const struct osal_mem_ops_t *mem;
} lexleo_flex_t;

#endif //LEXLEO_FLEX_BACKEND_H

