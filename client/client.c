#include <stdio.h>
#include "lib/tnetwork.h"

int main() {
    // printf("Hello World!\n");
    TNET_Init();
    SOCKET sock = TNET_socket(NULL, "5000");

    TNET_Free();
    return 0;
}