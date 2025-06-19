// src/osal/src/osal_macos.c

#include "osal.h"
#include <time.h>

void osal_sleep(int ms) {
    struct timespec req = {
        .tv_sec  = ms / 1000,
        .tv_nsec = (ms % 1000) * 1000000
    };
    nanosleep(&req, NULL);
}

void osal_open_in_web_browser(const char *filepath) {
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "open \"%s\" &", filepath);
    system(cmd);
}
