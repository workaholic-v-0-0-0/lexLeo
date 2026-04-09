

#ifndef LEXLEO_OSAL_TIME_OPS_H
#define LEXLEO_OSAL_TIME_OPS_H

#include "osal/time/osal_time_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct osal_time_ops_t {
	osal_time_status_t (*now)(osal_time_t *out);
} osal_time_ops_t;

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_TIME_OPS_H
