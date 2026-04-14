/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#include "tools/osal/osal_tools.h"

#include "policy/lexleo_win32_base.h"

void osal_open_in_web_browser(const char *filepath) {
	ShellExecuteA(
		NULL,         // no parent window
		"open",       // operation to perform
		filepath,
		NULL,         // no additional parameters
		NULL,         // default working directory
		SW_SHOWNORMAL // show window
	);
}
