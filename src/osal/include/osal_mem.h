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

// <here> still thinking about how to be able to double
//        anything ; maybe a handle per module
//        I think I've decided:
/*
typedef struct <module>_ctx_t {
	<module>_deps_t deps;
	const <module>_backends_t *backends;
	const <module>_ops_t ops;
	<module>_callbacks_t callbacks;
	void *user_data;
} <module>_ctx_t;
*/
// with singleton pattern?

