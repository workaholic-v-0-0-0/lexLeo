/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#include "osal.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

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