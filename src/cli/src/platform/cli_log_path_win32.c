/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 Sylvain Labopin
 */

#include "internal/cli_log_path.h"

#include "osal/file/osal_file.h"

#include "osal/mem/osal_mem.h"

#include "osal/str/osal_str.h"

#include "osal/env/osal_env.h"

#define CLI_DEFAULT_CONFIG_PATH "C:\\Program Files\\LexLeo\\config\\lexleo.ini"

static bool cli_trim_in_place(char* s)
{
    char* begin;
    char* end;

    if (!s)
        return false;

    begin = s;
    while (*begin && osal_isspace((unsigned char)*begin)) begin++;

    if (begin != s) osal_memmove(s, begin, osal_strlen(begin) + 1);

    if (*s == '\0')
        return true;

    end = s + osal_strlen(s) - 1;
    while (osal_isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }

    return true;
}

static bool cli_copy_string(char* out, size_t out_size, const char* src)
{
    int ret;

    if (!out || out_size == 0 || !src)
        return false;

    ret = osal_snprintf(out, out_size, "%s", src);
    if (ret < 0 || (size_t)ret >= out_size)
        return false;

    return true;
}

static bool cli_try_read_log_path_from_config(char* out, size_t out_size)
{
    if (!out || out_size == 0)
        return false;

    OSAL_FILE *cfg_file = NULL;
    const osal_mem_ops_t *mem_ops = osal_mem_default_ops();
    osal_file_status_t st = 
        osal_file_open(
            &cfg_file,
            CLI_DEFAULT_CONFIG_PATH,
            "rb",
            mem_ops
        );
    if (st != OSAL_FILE_STATUS_OK)
        return false;

    char line[255] = { 0 };
    bool in_logger_section = false;
    while (osal_file_gets(line, sizeof(line), cfg_file, &st)) {
        if (st != OSAL_FILE_STATUS_OK) {
            (void)osal_file_close(cfg_file);
            return false;
        }
            

        if (!cli_trim_in_place(line))
            continue;

        if (line[0] == '\0' || line[0] == ';' || line[0] == '#')
            continue;

        if (osal_strcmp(line, "[logger]") == 0)
            in_logger_section = true;

        if (!in_logger_section)
            continue;

        char* eq = osal_strchr(line, '=');
        if (!eq)
            continue;

        *eq = '\0';
        char* key = line;
        char* val = eq + 1;

        cli_trim_in_place(key);
        cli_trim_in_place(val);

        if (osal_strcmp(key, "log_path") != 0)
            continue;

        if (val[0] == '\0') {
            (osal_file_status_t)osal_file_close(cfg_file);
            return false;
        }

        if (val[0] == '"' || val[0] == '\'') {
            size_t len = osal_strlen(val);
            if (len >= 2 && val[len - 1] == val[0]) {
                val[len - 1] = '\0';
                val++;
            }
        }

        (osal_file_status_t)osal_file_close(cfg_file);
        return cli_copy_string(out, out_size, val);
    }

    (void)osal_file_close(cfg_file);
    return false;
}

static bool cli_resolve_default_windows_log_path(char* out, size_t out_size)
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

/* cli_resolve_default_log_path() tries to log_path from the 
   application configuration, and falls back to the platform-default 
   path only when no valid configured path is provided */
bool cli_resolve_default_log_path(char *out, size_t out_size)
{
    if (!out || out_size == 0)
        return false;

    out[0] = '\0';

    if (cli_try_read_log_path_from_config(out, out_size))
        return true;

    return cli_resolve_default_windows_log_path(out, out_size);
}
