/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#include "osal/str/osal_str.h"
#include "osal/stdio/osal_stdio.h"

#include "tools/osal/osal_process.h"

int main(int argc, char **argv)
{
    char local_path[1024];
    int ret;

    if (argc != 2) {
        static const char usage[] =
            "Usage: update_docs_on_droplet <build_directory>\n";
        (void)osal_stdio_write(
            usage,
            1,
            sizeof(usage) - 1,
            osal_stdio_stderr()
        );
        (void)osal_stdio_flush(osal_stdio_stderr());
        return 1;
    }

    ret = osal_snprintf(
        local_path,
        sizeof(local_path),
        "%s/docs/html/.",
        argv[1]
    );
    if (ret < 0 || (size_t)ret >= sizeof(local_path)) {
        static const char msg[] =
            "Failed to build local documentation path.\n";
        (void)osal_stdio_write(
            msg,
            1,
            sizeof(msg) - 1,
            osal_stdio_stderr()
        );
        (void)osal_stdio_flush(osal_stdio_stderr());
        return 1;
    }

    {
        char *scp_argv[] = {
            "scp",
            "-r",
            local_path,
            "root@64.23.187.50:/var/www/html/lexleo",
            NULL
        };

        return osal_spawn("scp", scp_argv);
    }
}
