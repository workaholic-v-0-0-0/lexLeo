// src/foundation/osal/osal_mem/include/osal/mem/osal_mem_types.h

#ifndef LEXLEO_OSAL_MEM_TYPES_H
#define LEXLEO_OSAL_MEM_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef void *(*osal_malloc_fn_t) (size_t size);
typedef void (*osal_free_fn_t) (void *ptr);
typedef void *(*osal_calloc_fn_t) (size_t nmemb, size_t size);
typedef void *(*osal_realloc_fn_t) (void *ptr, size_t size);
typedef char *(*osal_strdup_fn_t) (const char *s);
typedef void *(*osal_memcpy_fn_t)(void *dest, const void *src, size_t n);
typedef void *(*osal_memset_fn_t)(void *s, int c, size_t n);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_MEM_TYPES_H
