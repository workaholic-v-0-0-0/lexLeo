// src/osal/include/osal_mem_ops.h

#ifndef LEXLEO_OSAL_MEM_OPS_H
#define LEXLEO_OSAL_MEM_OPS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "osal_mem_types.h"

typedef struct osal_mem_ops_t {
	osal_malloc_fn_t malloc;
	osal_free_fn_t free;
	osal_calloc_fn_t calloc;
	osal_realloc_fn_t realloc;
	osal_strdup_fn_t strdup;
	osal_memcpy_fn_t memcpy;
	osal_memset_fn_t memset;
} osal_mem_ops_t;

const osal_mem_ops_t *osal_mem_default_ops(void);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_MEM_OPS_H
