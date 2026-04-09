

#ifndef LEXLEO_OSAL_TIME_H
#define LEXLEO_OSAL_TIME_H

#include "osal/time/osal_time_types.h"
#include "osal/time/osal_time_ops.h"

#ifdef __cplusplus
extern "C" {
#endif

const osal_time_ops_t *osal_time_default_ops(void);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_TIME_H
