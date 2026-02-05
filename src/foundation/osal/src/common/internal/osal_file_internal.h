// src/foundation/osal/src/common/internal/osal_file_internal.h

#ifndef LEXLEO_OSAL_FILE_INTERNAL_H
#define LEXLEO_OSAL_FILE_INTERNAL_H

#if defined(_WIN32) || defined(_WIN64)
const osal_file_ops_t *osal_file_win32_ops(void);
#else
const osal_file_ops_t *osal_file_posix_ops(void);
#endif

#endif //LEXLEO_OSAL_FILE_INTERNAL_H