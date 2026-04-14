/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#include "tools/osal/osal_process.h"

#include "policy/lexleo_posix_sys_types.h"
#include "policy/lexleo_posix_unistd.h"
#include "policy/lexleo_posix_sys_wait.h"

int osal_spawn(const char *program, char *const argv[])
{
	pid_t pid = fork();

	if (pid < 0) {
		return -1;
	}

	if (pid == 0) {
		execvp(program, argv);
		_exit(127);
	}

	for (;;) {
		int status = 0;

		if (waitpid(pid, &status, 0) < 0) {
			return -1;
		}

		if (WIFEXITED(status)) {
			return WEXITSTATUS(status);
		}

		if (WIFSIGNALED(status)) {
			return 128 + WTERMSIG(status);
		}
	}
}
