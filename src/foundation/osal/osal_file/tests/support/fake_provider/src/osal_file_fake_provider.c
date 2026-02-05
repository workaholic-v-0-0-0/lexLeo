// src/foundation/osal/osal_file/tests/support/fake_provider/src/osal_file_fake_provider.c

#include "osal/file/test/osal_file_fake_provider.h"
#include "osal/file/osal_file_ops.h"

static const osal_file_ops_t OSAL_FILE_FAKE_OPS = {
	.open = fake_file_open,
	.read = fake_file_read,
	.write = fake_file_write,
	.flush = fake_file_flush,
	.close = fake_file_close
};

const osal_file_ops_t *osal_file_test_fake_ops(void) {
	return &OSAL_FILE_FAKE_OPS;
}
