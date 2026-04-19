/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

/**
 * @file cli_log_path_win32.c
 * @ingroup cli_internal_group
 * @brief Win32-specific CLI log-path services.
 *
 * @details
 * This translation unit provides the Win32 implementation of the internal
 * CLI log-path services, including:
 * - the default application configuration path,
 * - the default log-file path resolution,
 * - and creation of the parent directory hierarchy for the target log file.
 */

#include "internal/cli_log_path.h"

#include "osal/stdio/osal_stdio.h"
#include "osal/str/osal_str.h"
#include "osal/file/osal_file.h"
#include "osal/env/osal_env.h"

#define CLI_DEFAULT_CONFIG_PATH "C:\\Program Files\\LexLeo\\config\\lexleo.ini"

bool cli_platform_ensure_log_parent_dir_exists(const char *log_path)
{
    char dir[1024];
    char *p;
    char *scan;
    int ret;
    osal_file_status_t st;

    if (!log_path || log_path[0] == '\0')
        return false;

    ret = osal_snprintf(dir, sizeof(dir), "%s", log_path);
    if (ret < 0 || (size_t)ret >= sizeof(dir))
        return false;

    p = osal_strrchr(dir, '\\');
    if (!p)
        return false;

    *p = '\0';

    /*
     * Minimal Win32 packaging support:
     * handle drive-letter absolute paths such as
     * "C:\Users\...\LexLeo\logs\lexleo.log".
     */
    if (
        ((dir[0] >= 'A' && dir[0] <= 'Z') ||
         (dir[0] >= 'a' && dir[0] <= 'z'))
        && dir[1] == ':'
        && dir[2] == '\\'
    ) {
        scan = dir + 3;
    } else {
        /*
         * For now, reject unsupported forms such as relative paths
         * or UNC paths. The current resolver does not produce them.
         */
        return false;
    }

    for (; *scan; ++scan) {
        if (*scan != '\\')
            continue;

        *scan = '\0';

        st = osal_file_mkdir(dir);
        if (st != OSAL_FILE_STATUS_OK && st != OSAL_FILE_STATUS_EXISTS) {
            *scan = '\\';
            return false;
        }

        *scan = '\\';
    }

    st = osal_file_mkdir(dir);
    if (st != OSAL_FILE_STATUS_OK && st != OSAL_FILE_STATUS_EXISTS)
        return false;

    return true;
}

const char *cli_platform_default_config_path(void)
{
    return CLI_DEFAULT_CONFIG_PATH;
}

bool cli_platform_resolve_default_log_path(char* out, size_t out_size)
{
    const char* base;
    int ret;

    if (!out || out_size == 0)
        return false;

    base = osal_getenv("LOCALAPPDATA");
    if (!base || !*base)
        return false;

    ret = osal_snprintf(
        out,
        out_size,
        "%s\\LexLeo\\logs\\lexleo.log",
        base
    );

    if (ret < 0 || (size_t)ret >= out_size)
        return false;

    return true;
}
