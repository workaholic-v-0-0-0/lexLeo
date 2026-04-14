/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#include "tools/osal/osal_process.h"

#include "policy/lexleo_win32_base.h"
#include "policy/lexleo_cstring.h"

static int append_arg(
	char *dst,
	size_t cap,
	size_t *len,
	const char *arg)
{
	size_t i = 0;

	if (*len > 0) {
		if (*len + 1 >= cap) {
			return -1;
		}
		dst[(*len)++] = ' ';
	}

	if (*len + 1 >= cap) {
		return -1;
	}
	dst[(*len)++] = '"';

	while (arg[i] != '\0') {
		if (arg[i] == '"' || arg[i] == '\\') {
			if (*len + 1 >= cap) {
				return -1;
			}
			dst[(*len)++] = '\\';
		}

		if (*len + 1 >= cap) {
			return -1;
		}
		dst[(*len)++] = arg[i];
		++i;
	}

	if (*len + 1 >= cap) {
		return -1;
	}
	dst[(*len)++] = '"';
	dst[*len] = '\0';

	return 0;
}

int osal_spawn(const char *program, char *const argv[])
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	char cmdline[4096];
	size_t len = 0;
	int i = 0;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);
	cmdline[0] = '\0';

	if (!program || !argv) {
		return -1;
	}

	while (argv[i] != NULL) {
		if (append_arg(cmdline, sizeof(cmdline), &len, argv[i]) != 0) {
			return -1;
		}
		++i;
	}

	if (!CreateProcessA(
			NULL,
			cmdline,
			NULL,
			NULL,
			FALSE,
			0,
			NULL,
			NULL,
			&si,
			&pi)) {
		return -1;
			}

	WaitForSingleObject(pi.hProcess, INFINITE);

	{
		DWORD exit_code = 0;

		if (!GetExitCodeProcess(pi.hProcess, &exit_code)) {
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			return -1;
		}

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);

		return (int)exit_code;
	}
}
