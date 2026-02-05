/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/logger/adapters/logger_default/src/internal/
 * logger_default_backend.h
 */

#ifndef LEXLEO_LOGGER_DEFAULT_BACKEND_H
#define LEXLEO_LOGGER_DEFAULT_BACKEND_H

#include "mem/osal_mem_ops.h"
#include "logger_default_state.h"

typedef struct logger_default_t {
	logger_default_state_t state;
	const struct osal_mem_ops_t *mem;
} logger_default_t;

#endif //LEXLEO_LOGGER_DEFAULT_BACKEND_H
