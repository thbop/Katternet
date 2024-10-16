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

#define TNET_BUF_LEN 1024

struct WSAData TNET_wsaData;



void TNET_PError0( const char* message, int code ) {
    if ( code == -1 ) printf("TNET ERROR: %s\n", message);
    else                printf("TNET ERROR: %s CODE: %d\n", message, code);
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
SOCKET TNET_Socket( const char* address, const char* port ) {
    struct addrinfo
        *result = NULL,
        *ptr = NULL,
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

    SOCKET sock = INVALID_SOCKET;

    if ( address == NULL ) {
        // Create a SOCKET for the server to listen for client connections.
        sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (sock == INVALID_SOCKET) {
            TNET_PError("Socket failed!");
            freeaddrinfo(result);
            WSACleanup();
            return INVALID_SOCKET;
        }

        // Setup the TCP listening socket
        iResult = bind( sock, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            TNET_PError("Socket bind error!");
            freeaddrinfo(result);
            closesocket(sock);
            WSACleanup();
            return 1;
        }

        freeaddrinfo(result);
    }
    else {
        // Attempt to connect to an address until one succeeds
        for (ptr=result; ptr != NULL; ptr=ptr->ai_next) {

            // Create a SOCKET for connecting to server
            sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
            if (sock == INVALID_SOCKET) {
                TNET_PError("Error at socket()");
                freeaddrinfo(result);
                WSACleanup(); // keep these
                return INVALID_SOCKET;
            }

            // Connect to server.
            iResult = connect( sock, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (iResult == SOCKET_ERROR) {
                closesocket(sock);
                sock = INVALID_SOCKET;
                continue;
            }
            break;
        }

        freeaddrinfo(result);

        if (sock == INVALID_SOCKET) {
            TNET_PError0("Unable to connect to server!", -1 );
            WSACleanup();
            return INVALID_SOCKET;
        }
    }
    
    return sock;
}

int TNET_Send( SOCKET sock, const char *buffer ) {
    int iResult = send(sock, buffer, strlen(buffer), 0);
    if ( iResult == SOCKET_ERROR ) {
        TNET_PError("Failure to send!");
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    return 0;
}


// Closes connection and cleans up socket
int TNET_Close( SOCKET sock ) {
    int iResult = shutdown(sock, SD_SEND);
    closesocket(sock);
    WSACleanup();
    if ( iResult == SOCKET_ERROR ) {
        TNET_PError("Failure when closing connection!");
        return 1;
    }
    return 0;
}

typedef enum TNET_Recv_Res {
    TNET_RR_CONNECTED,
    TNET_RR_DISCONNECTED,
    TNET_RR_ERROR
} TNET_Recv_Res;

TNET_Recv_Res TNET_Recv( SOCKET sock, char *buf ) {
    int iResult = recv( sock, buf, TNET_BUF_LEN, 0 );
    if      ( iResult > 0  )                     return TNET_RR_CONNECTED;
    else if ( iResult == 0 ) { TNET_Close(sock); return TNET_RR_DISCONNECTED; }
    else {
        TNET_PError("Recv failed!");
        closesocket(sock);
        WSACleanup();
        return TNET_RR_ERROR;
    }
}


#endif