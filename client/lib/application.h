#ifndef APPLICATION_H
#define APPLICATION_H

#include "core.h"
#include "tnetwork.h"
#include "hashmap.h"

struct {
    SOCKET sock;    // Client socket
    HM     storage; // Locally stored data
} APP_state;

int APP_Init() {
    return
        TNET_Init();
}

void APP_Free() {
    TNET_Free();
}


#endif