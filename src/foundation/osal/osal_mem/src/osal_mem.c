// src/foundation/osal/osal_mem/src/osal_mem.c

#include "osal/mem/osal_mem.h"

#include "policy/lexleo_cstd_lib.h"
#include "policy/lexleo_cstring.h"

void *osal_malloc(size_t size) { return malloc(size); }
void *osal_calloc(size_t nmemb, size_t size) { return calloc(nmemb, size); }
void *osal_realloc(void *ptr, size_t size) { return realloc(ptr, size); }
void  osal_free(void *ptr) { free(ptr); }

char *osal_strdup(const char *s)
{
	if (!s) return NULL;

	size_t n = strlen(s) + 1;
	char *p = osal_malloc(n);
	if (!p) return NULL;

	memcpy(p, s, n);
	return p;
}

void *osal_memcpy(void *dest, const void *src, size_t n)
{
	return memcpy(dest, src, n);
}

void *osal_memset(void *s, int c, size_t n)
{
	return memset(s, c, n);
}

size_t osal_strlen(const char *s)
{
	return strlen(s);
}

int osal_strcmp(const char *s1, const char *s2) {
	return strcmp(s1, s2);
}

void *osal_memmove(void *dest, const void *src, size_t n) {
	return memmove(dest, src, n);
}
