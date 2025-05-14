// src/osal/src/osal_windows.c

#include "osal.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void osal_sleep(int ms) {
    Sleep(ms);
}
