#include <stdio.h>
// #include "lib/core.h"
#include "lib/tnetwork.h"
// #include "lib/application.h"
// #include "lib/hashmap.h"

int main() {
    TNET_Init();
    SOCKET sock = TNET_Socket("127.0.0.1", "5000");
    char buf[100];

    while ( strncmp(buf, "quit", 5) ) {
        printf("> ");
        fgets(buf, 100, stdin);
        buf[strlen(buf)-1] = 0;
        TNET_Send(sock, buf);
    }

    TNET_Close(sock);
    TNET_Free();

    return 0;
}