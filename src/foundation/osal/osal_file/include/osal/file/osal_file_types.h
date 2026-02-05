// src/foundation/osal/osal_file/include/osal/file/osal_file_types.h

#ifndef LEXLEO_OSAL_FILE_TYPES_H
#define LEXLEO_OSAL_FILE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct osal_file_t osal_file_t;

typedef enum {
	OSAL_FILE_OK = 0,
	OSAL_FILE_EOF,
	OSAL_FILE_ERR,
	OSAL_FILE_NOENT,
	OSAL_FILE_PERM,
	OSAL_FILE_IO,
	OSAL_FILE_NOSYS
} osal_file_status_t;

typedef enum {
	OSAL_FILE_READ   = 1u << 0,
	OSAL_FILE_WRITE  = 1u << 1,
	OSAL_FILE_APPEND = 1u << 2,
	OSAL_FILE_CREATE = 1u << 3,
	OSAL_FILE_TRUNC  = 1u << 4
} osal_file_open_flags_t;

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_FILE_TYPES_H