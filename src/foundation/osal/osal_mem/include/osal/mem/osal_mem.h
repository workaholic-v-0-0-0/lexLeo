/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_mem.h
 * @ingroup osal_mem_api
 * @brief Public memory primitives of the `osal_mem` module.
 *
 * @details
 * This header declares portable low-level memory services provided by
 * the Operating System Abstraction Layer.
 *
 * Current public services:
 * - allocation and deallocation
 * - raw memory copy
 * - raw memory fill
 * - raw memory move
 */

#ifndef LEXLEO_OSAL_MEM_H
#define LEXLEO_OSAL_MEM_H

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void *osal_malloc(size_t size);
void osal_free(void *ptr);
void *osal_calloc(size_t nmemb, size_t size);
void *osal_realloc(void *ptr, size_t size);

void *osal_memcpy(void *dest, const void *src, size_t n);
void *osal_memset(void *s, int c, size_t n);
void *osal_memmove(void *dest, const void *src, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_OSAL_MEM_H */
