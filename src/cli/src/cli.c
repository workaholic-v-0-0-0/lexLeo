// src/cli/src/cli.c

// ----------------------------------------------------------------------------
// ----------------------------- PLACEHOLDER FILE -----------------------------
// ----------------------------------------------------------------------------

/* Resolved log_path and write it in standard output stream */

#include "internal/cli_log_path.h"

#include "cli.h"

#define CLI_LOG_PATH_BUFFER_SIZE 1024

#include "osal/stdio/osal_stdio_ops.h"

#include "osal/mem/osal_mem.h"

int cli_run() {

	const osal_stdio_ops_t *stdio_ops = osal_stdio_default_ops();
	OSAL_STDIO *out = stdio_ops->stdout();

	char buf[CLI_LOG_PATH_BUFFER_SIZE] = { 0 };
	bool ok = cli_resolve_default_log_path(buf, CLI_LOG_PATH_BUFFER_SIZE);
	if (ok) {
		(void)stdio_ops->write("log_path resolved with: ", 1, 24, out);
		(void)stdio_ops->write(buf, 1, osal_strlen(buf), out);
		(void)stdio_ops->write("\n", 1, 1, out);
		(void)stdio_ops->flush(out);
	}

	return 0;
}
//<here> upgrade cli_resolve_default_log_path() so that it first tries to
// read an explicit log_path from the application configuration, and falls
// back to the platform-default path only when no valid configured path is
// provided
// so do osal_file before
