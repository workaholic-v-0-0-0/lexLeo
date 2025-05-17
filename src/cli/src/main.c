// src/cli/src/main.c

#include "osal.h"
#include <stdio.h>

int main() {
#ifdef DEBUG_BUILD
    osal_sleep(1000);
    printf("Hello World from debug build!\n");
#elif RELEASE_BUILD
    printf("Hello World from release build!\n");
#endif
    return 0;
}
