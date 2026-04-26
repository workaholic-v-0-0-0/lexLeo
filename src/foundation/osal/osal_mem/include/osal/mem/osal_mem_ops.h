/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file osal_mem_ops.h
 * @ingroup osal_mem_api
 * @brief Injectable memory operations table for the `osal_mem` module.
 *
 * @details
 * This header declares the memory operations interface used to inject
 * allocation services into modules that must not depend directly on the
 * default allocator.
 *
 * Current injectable services:
 * - allocation
 * - deallocation
 * - zero-initialized allocation
 * - reallocation
 */

#ifndef LEXLEO_OSAL_MEM_OPS_H
#define LEXLEO_OSAL_MEM_OPS_H

#include "osal/mem/osal_mem_types.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct osal_mem_ops_t {
	void *(*malloc)(size_t size);
	void (*free)(void *ptr);
	void *(*calloc)(size_t nmemb, size_t size);
	void *(*realloc)(void *ptr, size_t size);
};

const osal_mem_ops_t *osal_mem_default_ops(void);

#ifdef __cplusplus
}
#endif

#endif /* LEXLEO_OSAL_MEM_OPS_H */
