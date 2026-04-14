/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#include "tools/osal/osal_tools.h"

#include "policy/lexleo_cstd_io.h"
#include "policy/lexleo_cstd_lib.h"

void osal_open_in_web_browser(const char *filepath) {
	int ret = system("command -v firefox > /dev/null 2>&1");
	char cmd[1024];
	if (ret == 0) {
		snprintf(cmd, sizeof(cmd), "firefox --new-window \"%s\" 2>/dev/null &", filepath);
	} else {
		snprintf(cmd, sizeof(cmd), "xdg-open \"%s\" 2>/dev/null &", filepath);
	}
	system(cmd);
}
