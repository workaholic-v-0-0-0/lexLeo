// src/foundation/osal/osal_mem/tests/support/fake_provider/include/osal/mem/test/osal_mem_fake_provider.h

#ifndef LEXLEO_OSAL_MEM_FAKE_PROVIDER_H
#define LEXLEO_OSAL_MEM_FAKE_PROVIDER_H

#include <stddef.h>
#include <stdbool.h>

#include "osal/mem/osal_mem_ops.h"
#include "lexleo/test/fake_memory.h"

const osal_mem_ops_t *osal_mem_test_fake_ops(void);

#endif //LEXLEO_OSAL_MEM_FAKE_PROVIDER_H
