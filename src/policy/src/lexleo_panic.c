/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#include "policy/lexleo_panic.h"
#include "policy/lexleo_cstd_lib.h"

#include "osal/stdio/osal_stdio_ops.h"

#include "osal/mem/osal_mem.h"

void lexleo_panic(const char *msg) {
	const osal_stdio_ops_t *stdio_ops = osal_stdio_default_ops();
	OSAL_STDIO *err = stdio_ops->stderr();
	const char *text = msg ? msg : "(no message)";
	(void)stdio_ops->write("LexLeo panic: ", 1, 15,  err);
	(void)stdio_ops->write(text, 1, osal_strlen(text),  err);
	(void)stdio_ops->write("\n", 1, 1, err);
	(void)stdio_ops->flush(err);
	abort();
}

void lexleo_panic_oom(void) {
	lexleo_panic("out of memory");
}
