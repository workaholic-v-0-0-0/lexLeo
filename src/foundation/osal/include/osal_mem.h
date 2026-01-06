// src/osal/include/osal_mem.h

#ifndef LEXLEO_OSAL_MEM_H
#define LEXLEO_OSAL_MEM_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------------
// PUBLIC API
// ----------------------------------------------------------------------------

void *osal_malloc(size_t size);
void osal_free(void *ptr);
void *osal_calloc(size_t nmemb, size_t size);
void *osal_realloc(void *ptr, size_t size);

char *osal_strdup(const char *s);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_MEM_H

// <here> a handle per module
/*
typedef struct <module>_ctx_t {
	<module>_deps_t deps;
	const <module>_backends_t *backends;
	const <module>_ops_t ops;
	<module>_callbacks_t callbacks;
	void *user_data;
} <module>_ctx_t;
typedef struct <module>_ctx_t {
	<module>_ctx_t ctx;
	<dep1>_t *dep1;
} <module>_t;
*/
// with singleton pattern?
// do lexleo_bootstrap module to manage all initialisation

