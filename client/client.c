#include <stdio.h>
// #include "lib/core.h"
#include "lib/tnetwork.h"
// #include "lib/application.h"
// #include "lib/hashmap.h"

int main() {
    TNET_Init();
    SOCKET sock = TNET_Socket("127.0.0.1", "5000");
    char buf[TNET_BUF_LEN];

    while ( 1 ) {
        printf("> ");
        fgets(buf, 100, stdin);
        buf[strlen(buf)-1] = 0;
        if ( !strncmp(buf, "quit", 5) ) break;

        TNET_Send(sock, buf);

        memset(buf, 0, TNET_BUF_LEN);
        if ( !TNET_Recv(sock, buf) ) printf("< %s\n", buf);

    }

    TNET_Close(sock);
    TNET_Free();

    return 0;
}