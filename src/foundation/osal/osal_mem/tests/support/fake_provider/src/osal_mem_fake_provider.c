// src/foundation/osal/osal_mem/tests/support/fake_provider/src/osal_mem_fake_provider.c

#include "osal/mem/test/osal_mem_fake_provider.h"
#include "osal/mem/osal_mem.h"

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

static const osal_mem_ops_t OSAL_MEM_FAKE_OPS = {
	.malloc = fake_malloc,
	.free = fake_free,
	.calloc = fake_calloc,
	.realloc = fake_realloc,
	.strdup = fake_strdup,
	.memcpy = fake_memcpy,
	.memset = fake_memset
};

const osal_mem_ops_t *osal_mem_test_fake_ops(void) {
  return &OSAL_MEM_FAKE_OPS;
}
