// src/osal/include/osal_mem_ops.h

#ifndef LEXLEO_OSAL_MEM_OPS_H
#define LEXLEO_OSAL_MEM_OPS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// OPS TABLE
// ----------------------------------------------------------------------------

typedef void *(*osal_malloc_fn_t) (size_t size);
typedef void (*osal_free_fn_t) (void *ptr);
typedef void *(*osal_calloc_fn_t) (size_t nmemb, size_t size);
typedef void *(*osal_realloc_fn_t) (void *ptr, size_t size);
typedef char *(*osal_strdup_fn_t) (const char *s);

typedef struct osal_mem_ops_t {
	osal_malloc_fn_t malloc;
	osal_free_fn_t free;
	osal_calloc_fn_t calloc;
	osal_realloc_fn_t realloc;
	osal_strdup_fn_t strdup;
} osal_mem_ops_t;

// ----------------------------------------------------------------------------
// DEFAULT OPS
// ----------------------------------------------------------------------------

const osal_mem_ops_t *osal_mem_default_ops(void);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_MEM_OPS_H
