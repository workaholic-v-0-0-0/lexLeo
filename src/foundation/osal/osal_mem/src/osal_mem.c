/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_mem.c
 * @ingroup osal_mem_internal_group
 * @brief Default implementation of the `osal_mem` module.
 */

#include "osal/mem/osal_mem.h"
#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstring.h"

void *osal_malloc(size_t size) { return malloc(size); }
void osal_free(void *ptr) { free(ptr); }
void *osal_calloc(size_t n, size_t s) { return calloc(n, s); }
void *osal_realloc(void *ptr, size_t size) { return realloc(ptr, size); }

const osal_mem_ops_t *osal_mem_default_ops(void)
{
	static const osal_mem_ops_t OPS = {
		.malloc  = osal_malloc,
		.free    = osal_free,
		.calloc  = osal_calloc,
		.realloc = osal_realloc,
	};
	return &OPS;
}

void *osal_memcpy(void *dest, const void *src, size_t n)
{
	return memcpy(dest, src, n);
}

void *osal_memset(void *s, int c, size_t n)
{
	return memset(s, c, n);
}

void *osal_memmove(void *dest, const void *src, size_t n) {
	return memmove(dest, src, n);
}
