// tools/update_docs_on_droplet.c

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    char cmd[1024];
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <build_directory>\n", argv[0]);
        return 1;
    }
    printf("scp -r %s/docs/html/. root@64.23.187.50:/var/www/html/lexleo\n", argv[1]);
    snprintf(cmd, sizeof(cmd), "scp -r '%s/docs/html/.' root@64.23.187.50:/var/www/html/lexleo", argv[1]);
    system(cmd);
    return 0;
}
