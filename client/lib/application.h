#ifndef APPLICATION_H
#define APPLICATION_H

#include "core.h"
#include "sha-256.h"
#include "tnetwork.h"
#include "hashmap.h"

#define APP_STORAGE_FILENAME "storage"
#define APP_STORAGE_SIZE 16 // Storage will probably operate most efficiently with X number of pairs; this does not limit the number of pairs to X.
#define APP_INBUF_SIZE 32
#define APP_PORT "6410"

typedef unsigned int u32;

struct {
    SOCKET sock;    // Client socket
    HM     storage; // Locally stored data
    u32    gui;     // 0 = init, 1 = login
} app;

int APP_Init() {
    int ret = 0;

    ret |= TNET_Init();

    app.storage = HM_init(APP_STORAGE_SIZE);
    HM_load(&app.storage, APP_STORAGE_FILENAME);

    app.gui = 0;

    return ret;
}


int APP_AuthMe() {
    char inbuf[APP_INBUF_SIZE];
    bool running = true;

    printf("Welcome to the Katternet!\n\n");

    do { printf("Username: "); CORE_input(inbuf, APP_INBUF_SIZE); } while ( strlen(inbuf) < 2 && strlen(inbuf) > 16 );
    HM_set_str(&app.storage, "USERNAME", inbuf, true);

    do { printf("Password: "); CORE_input(inbuf, APP_INBUF_SIZE); } while ( strlen(inbuf) < 8 && strlen(inbuf) > 16 );
    char result_token[65];
    calc_sha_256_s(result_token, inbuf);
    HM_set_str(&app.storage, "TOKEN", result_token, true);

    HM_dump(&app.storage, APP_STORAGE_FILENAME); // Needs error checking
    
    return 0;
}

int APP_Connect() {
    char coords[16];
    do { printf("Depot coordinates: "); CORE_input(coords, 16); } while ( strlen(coords) < 7 && strlen(coords) > 15 );
    app.sock = TNET_Socket(coords, APP_PORT);
    if ( app.sock == INVALID_SOCKET ) printf("Travel failed!\n");
}


int APP_Run() {
    printf(" _____ _            _   __      _   _                       _   \n|_   _| |          | | / /     | | | |                     | |  \n  | | | |__   ___  | |/ /  __ _| |_| |_ ___ _ __ _ __   ___| |_ \n  | | | '_ \\ / _ \\ |    \\ / _` | __| __/ _ \\ '__| '_ \\ / _ \\ __|\n  | | | | | |  __/ | |\\  \\ (_| | |_| ||  __/ |  | | | |  __/ |_ \n  \\_/ |_| |_|\\___| \\_| \\_/\\__,_|\\__|\\__\\___|_|  |_| |_|\\___|\\__|\n\n");
    
    if ( !strncmp( HM_get(&app.storage, "TOKEN"), "NULL", 60 ) ) APP_AuthMe();

    bool running = true;
    char inbuf[APP_INBUF_SIZE];

    while (running) {
        printf("> ");
        CORE_input(inbuf, APP_INBUF_SIZE);
        switch (*inbuf) {
            case 'h': printf("h = help, q = quit, g = get, c = connect\n"); break;
            case 'q': running = false; break;
            case 'c': APP_Connect(); break;
            default: break;
        }
    }

    return 0;
}

void APP_Free() {
    HM_free(&app.storage, true);
    TNET_Free();
}


#endif