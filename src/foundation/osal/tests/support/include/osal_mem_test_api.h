// src/foundation/osal/tests/support/include/osal_mem_test_api.h

#ifndef LEXLEO_OSAL_MEM_TEST_API_H
#define LEXLEO_OSAL_MEM_TEST_API_H

#include <stddef.h>
#include <stdbool.h>

#include "osal_mem_ops.h"
#include "fake_memory.h"

/*
const osal_mem_ops_t *osal_mem_test_fake_ops(void);

void fake_memory_reset(void);

void fake_memory_fail_on_calls(size_t n, const size_t *idxs);
void fake_memory_fail_only_on_call(size_t n);
void fake_memory_fail_on_all_call(void);
void fake_memory_fail_since(size_t n);

bool fake_memory_no_leak(void);
bool fake_memory_no_invalid_free(void);
bool fake_memory_no_double_free(void);
*/

const osal_mem_ops_t *osal_mem_test_fake_ops(void);

#endif //LEXLEO_OSAL_MEM_TEST_API_H
