
#ifndef LEXLEO_OSAL_TIME_FAKE_PROVIDER_H
#define LEXLEO_OSAL_TIME_FAKE_PROVIDER_H

#include "osal/time/osal_time_ops.h"

#include "lexleo/test/fake_time.h"

#ifdef __cplusplus
extern "C" {
#endif

const osal_time_ops_t *osal_time_test_fake_ops(void);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_TIME_FAKE_PROVIDER_H
