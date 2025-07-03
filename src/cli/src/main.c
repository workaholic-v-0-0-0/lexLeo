// bison/cli/bison/main.c

#include "osal.h"
#include <stdio.h>

int main() {
#ifdef DEBUG
    osal_sleep(1000);
    printf("Hello World from debug build!\n");
#elif RELEASE
    printf("Hello World from release build!\n");
#endif
    return 0;
}
