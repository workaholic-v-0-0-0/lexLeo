// src/osal/src/osal_windows.c

#include "osal.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <stdio.h>

void osal_sleep(int ms) {
    Sleep(ms);
}

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

FILE *osal_fmemopen_ro(const char *data, size_t len) {
    if (!data) return NULL;
    if (len == (size_t)-1) len = strlen(data);

    FILE *f = NULL;
#ifdef _MSC_VER
    if (tmpfile_s(&f) != 0) return NULL;
#else
    f = tmpfile();
    if (!f) return NULL;
#endif
    if (len && fwrite(data, 1, len, f) != len) { fclose(f); return NULL; }
    rewind(f);
    return f;
}
