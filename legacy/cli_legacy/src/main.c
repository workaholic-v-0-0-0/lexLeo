// src/cli/main.c

#include "cli.h"

int main() {
    return cli_run();
}

/*
int main() {
#ifdef DEBUG
    osal_sleep(1000);
    printf("Hello World from debug build!\n");
#elif RELEASE
    printf("Hello World from release build!\n");
#endif
    return 0;
}
*/