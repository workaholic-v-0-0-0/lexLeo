// src/foundation/osal/src/common/osal_file.c

#include "osal/file/osal_file.h"
#include "osal/file/osal_file_ops.h"
#include "osal/mem/osal_mem_ops.h"
#include "osal/file/osal_file_env.h"
#include "internal/osal_file_internal.h"

#include <stddef.h>

const osal_file_ops_t *osal_file_default_ops(void) {
#if defined(_WIN32) || defined(_WIN64)
    return osal_file_win32_ops();
#else
    return osal_file_posix_ops();
#endif
}

static void set_status(osal_file_status_t *st, osal_file_status_t v) {
    if (st) *st = v;
}

osal_file_t *osal_file_open(
    const char *path_utf8,
    uint32_t flags,
    osal_file_status_t *status,
	const osal_file_env_t *env )
{
    const osal_file_ops_t *ops = osal_file_default_ops();
    if (!ops || !ops->open) {
        set_status(status, OSAL_FILE_NOSYS);
        return NULL;
    }
    return ops->open(path_utf8, flags, status, env);
}

size_t osal_file_read(
    osal_file_t *f,
    void *buf,
    size_t n,
    osal_file_status_t *status)
{
    const osal_file_ops_t *ops = osal_file_default_ops();
    if (!ops || !ops->read) {
        set_status(status, OSAL_FILE_NOSYS);
        return 0;
    }
    return ops->read(f, buf, n, status);
}

size_t osal_file_write(
    osal_file_t *f,
    const void *buf,
    size_t n,
    osal_file_status_t *status)
{
    const osal_file_ops_t *ops = osal_file_default_ops();
    if (!ops || !ops->write) {
        set_status(status, OSAL_FILE_NOSYS);
        return 0;
    }
    return ops->write(f, buf, n, status);
}

osal_file_status_t osal_file_flush(osal_file_t *f)
{
    const osal_file_ops_t *ops = osal_file_default_ops();
    if (!ops || !ops->flush) return OSAL_FILE_NOSYS;
    return ops->flush(f);
}

osal_file_status_t osal_file_close(osal_file_t *f)
{
    const osal_file_ops_t *ops = osal_file_default_ops();
    if (!ops || !ops->close) return OSAL_FILE_NOSYS;
    return ops->close(f);
}

osal_file_env_t osal_file_default_env(const osal_mem_ops_t *mem) {
	osal_file_env_t env;
	env.mem = mem ? mem : osal_mem_default_ops();
	return env;
}
