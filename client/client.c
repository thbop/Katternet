#include <stdio.h>
#include "lib/tnetwork.h"
// #include "lib/application.h"
#include "lib/hashmap.h"

int main() {
    // printf("Hello World!\n");
    // TNET_Init();
    // SOCKET sock = TNET_socket("127.0.0.1", "5000");

    // TNET_Free();

    int a = 12;
    int b = 34;

    HM hashmap = HM_init(6);
    HM_set(&hashmap, "number", &a);
    HM_set(&hashmap, "water", &b);

    

    printf("%d\n", *(int*)HM_get(&hashmap, "water"));

    HM_free(&hashmap);

    return 0;
}