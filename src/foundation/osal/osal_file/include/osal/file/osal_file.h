#ifndef LEXLEO_OSAL_FILE_H
#define LEXLEO_OSAL_FILE_H

#include "osal/file/osal_file_types.h"

#include "osal/mem/osal_mem_ops.h"

#include "policy/lexleo_cstd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

osal_file_status_t osal_file_open(
	OSAL_FILE** out,
	const char* pathname,
	const char* mode,
	const osal_mem_ops_t* mem_ops);

size_t osal_file_read(
	void* ptr,
	size_t size,
	size_t nmemb,
	OSAL_FILE* stream,
	osal_file_status_t* st);

size_t osal_file_write(
	const void* ptr,
	size_t size,
	size_t nmemb,
	OSAL_FILE* stream,
	osal_file_status_t* st);

osal_file_status_t osal_file_flush(OSAL_FILE* stream);

osal_file_status_t osal_file_close(OSAL_FILE* stream);

char* osal_file_gets(
	char* out,
	size_t out_size,
	OSAL_FILE* stream,
	osal_file_status_t* st);

osal_file_status_t osal_file_mkdir(const char *pathname);

#ifdef __cplusplus
}
#endif

#endif //LEXLEO_OSAL_FILE_H
