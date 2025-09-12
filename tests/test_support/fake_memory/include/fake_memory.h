// tests/test_support/fake_memory/include/fake_memory.h

#ifndef LEXLEO_FAKE_MEMORY_H
#define LEXLEO_FAKE_MEMORY_H

#include <stddef.h>
#include <stdbool.h>

void fake_memory_reset(void);
void fake_memory_fail_on_calls(size_t n, const size_t *idxs);
void fake_memory_fail_only_on_call(size_t n);
bool fake_memory_no_leak(void);
bool fake_memory_no_invalid_free(void);
bool fake_memory_no_double_free(void);

void *fake_malloc(size_t size);
void fake_free(void *ptr);
char *fake_strdup(const char *s);

#endif //LEXLEO_FAKE_MEMORY_H
