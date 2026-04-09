

#ifndef LEXLEO_OSAL_STDIO_FAKE_PROVIDER_H
#define LEXLEO_OSAL_STDIO_FAKE_PROVIDER_H

#include "osal/stdio/osal_stdio_ops.h"

#include "lexleo/test/fake_stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

const osal_stdio_ops_t *osal_stdio_test_fake_ops(void);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_STDIO_FAKE_PROVIDER_H
