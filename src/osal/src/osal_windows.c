// src/osal/src/osal_windows.c

#include "osal.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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
