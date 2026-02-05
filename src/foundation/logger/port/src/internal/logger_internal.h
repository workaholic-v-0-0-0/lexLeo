/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 *
 * File:
 * src/foundation/logger/port/src/internal
 * logger_internal.h
 */

#ifndef LEXLEO_LOGGER_INTERNAL_H
#define LEXLEO_LOGGER_INTERNAL_H

#include "internal/logger_ctx.h"

/* Concrete logger handle (internal to the port). */
struct logger_t {
	logger_vtbl_t vtbl;
	void *backend;
	const osal_mem_ops_t *mem;
};

#endif //LEXLEO_LOGGER_INTERNAL_H
