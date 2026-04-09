// src/foundation/osal/osal_mem/include/osal/mem/osal_mem_ops.h

#ifndef LEXLEO_OSAL_MEM_OPS_H
#define LEXLEO_OSAL_MEM_OPS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "policy/lexleo_cstd_types.h"

typedef struct osal_mem_ops_t {
	void *(*malloc) (size_t size);
	void (*free) (void *ptr);
	void *(*calloc) (size_t nmemb, size_t size);
	void *(*realloc) (void *ptr, size_t size);
	char *(*strdup) (const char *s);
} osal_mem_ops_t;

const osal_mem_ops_t *osal_mem_default_ops(void);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_MEM_OPS_H
