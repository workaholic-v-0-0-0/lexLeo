// tools/open_doc.c

#include "osal.h"
int main(int argc, char **argv) {
    if (argc != 2) return 1;
    osal_open_in_web_browser(argv[1]);
    return 0;
}
