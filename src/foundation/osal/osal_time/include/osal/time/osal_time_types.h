

#ifndef LEXLEO_OSAL_TIME_TYPES_H
#define LEXLEO_OSAL_TIME_TYPES_H

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct osal_time_ops_t osal_time_ops_t;

typedef struct {
    int64_t epoch_seconds;
} osal_time_t;

typedef enum osal_time_status_t {
	OSAL_TIME_STATUS_OK = 0,
	OSAL_TIME_STATUS_INVALID,
	OSAL_TIME_STATUS_ERROR
} osal_time_status_t;

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_TIME_TYPES_H
