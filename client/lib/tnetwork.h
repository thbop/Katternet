#ifndef TNETWORK_H
#define TNETWORK_H

// A hopefully simple networking library to deal with TCP sockets

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

struct WSAData TNET_wsaData;

void TNET_PError0( const char* message, int code ) {
    printf("TNET ERROR: %s CODE: %d\n", message, code);
}
int TNET_PError( const char* message ) {
    int code = WSAGetLastError();
    TNET_PError0( message, code );
    return code;
}

int TNET_Init() {
    // Initialize Winsock
    int iResult = WSAStartup( MAKEWORD(2,2), &TNET_wsaData );
    if ( iResult != 0 ) {
        TNET_PError0( "WSAStartup failed!", iResult );
        return 1;
    }
}

void TNET_Free() {
    WSACleanup();
}

// If this is a server socket, set the address field as `NULL`
SOCKET TNET_socket( const char* address, const char* port ) {
    struct addrinfo
        *result = NULL,
        hints;

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    if ( address == NULL ) hints.ai_flags = AI_PASSIVE;

    int iResult;
    if ( address == NULL ) iResult = getaddrinfo(NULL, port, &hints, &result);
    else                   iResult = getaddrinfo(address, port, &hints, &result);
    if (iResult != 0) {
        TNET_PError0("getaddrinfo failed!", iResult);
        WSACleanup();
        return INVALID_SOCKET;
    }
    SOCKET newSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (newSock == INVALID_SOCKET) {
        TNET_PError0("Error at socket()", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup(); // keep these
        return INVALID_SOCKET;
    }
    return newSock;
}

#endif