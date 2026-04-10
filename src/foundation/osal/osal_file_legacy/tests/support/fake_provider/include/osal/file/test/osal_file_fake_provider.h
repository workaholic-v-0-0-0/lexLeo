// src/foundation/osal/osal_file/tests/support/fake_provider/include/osal/file/test/osal_file_fake_provider.h

#ifndef LEXLEO_OSAL_FILE_FAKE_PROVIDER_H
#define LEXLEO_OSAL_FILE_FAKE_PROVIDER_H

#include "osal/file/osal_file_ops.h"

#include "lexleo/test/fake_file.h"

#ifdef __cplusplus
extern "C" {
#endif

const osal_file_ops_t *osal_file_test_fake_ops(void);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_FILE_FAKE_PROVIDER_H
