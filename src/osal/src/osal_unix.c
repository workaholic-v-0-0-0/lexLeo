// bison/osal/bison/osal_unix.c

#include "osal.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

void osal_sleep(int ms) {
    struct timespec req = {
        .tv_sec  = ms / 1000,
        .tv_nsec = (ms % 1000) * 1000000
    };
    nanosleep(&req, NULL);
}

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
