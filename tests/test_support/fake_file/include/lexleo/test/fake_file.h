// tests/test_support/fake_file/include/fake_file.h

#ifndef LEXLEO_FAKE_FILE_H
#define LEXLEO_FAKE_FILE_H

#include "osal/file/osal_file_ops.h"
#include "osal/file/osal_file_env.h"
#include "osal/file/osal_file_types.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

osal_file_t *fake_file_open(
    const char *path_utf8,
    uint32_t flags,
    osal_file_status_t *status,
    const osal_file_env_t *env );

size_t fake_file_read(
    osal_file_t *f,
    void *buf,
    size_t n,
    osal_file_status_t *status );

size_t fake_file_write(
    osal_file_t *f,
    const void *buf,
    size_t n,
    osal_file_status_t *status );

osal_file_status_t fake_file_flush(osal_file_t *f);

osal_file_status_t fake_file_close(osal_file_t *f);

typedef enum fake_file_op_t {
    FAKE_FILE_OP_OPEN = 1,
    FAKE_FILE_OP_READ,
    FAKE_FILE_OP_WRITE,
    FAKE_FILE_OP_FLUSH,
    FAKE_FILE_OP_CLOSE,
} fake_file_op_t;

typedef struct fake_file_counters_t {
    size_t open_calls;
    size_t read_calls;
    size_t write_calls;
    size_t flush_calls;
    size_t close_calls;
} fake_file_counters_t;

void fake_file_reset(void);

void fake_file_set_backing(
    uint8_t *backing_buf,
    size_t cap,
    size_t initial_len );

void fake_file_set_pos(size_t pos);

void fake_file_fail_disable(void);
void fake_file_fail_enable(
    fake_file_op_t op,
    size_t fail_at_call,
    osal_file_status_t status );

const fake_file_counters_t *fake_file_counters(void);

size_t fake_file_backing_len(void);

#ifdef __cplusplus
}
#endif

#endif // LEXLEO_FAKE_FILE_H
