#include <stdio.h>
#include "lib/tnetwork.h"

int main() {
    printf("Hello World!");
    TNET_Init();
    SOCKET sock = TNET_socket("127.0.0.1", "5000");

    TNET_Free();
    return 0;
}