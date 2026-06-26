#include "../include/libOskServer.h"
#include "../include/InternalUtilityMacros.h"

#include <stdio.h>

#define MAX_BUFFER_SIZE 32768
#define KB 8192

typedef struct 
{
    char* Content;
    size_t Count;
    size_t Capacity;
} TextBuffer;

/// @brief Starts the server listening ona specific port 
/// @param server the server tointialise
/// @param port the prt
/// @return 0 on succes, -1 on failure
int InitiateListeningPort(HTTPServer* server, uint16_t port) {
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        return -1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        return -1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(serverSocket);
        return -1;
    }
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(serverSocket);
        return -1;
    }

    server->ServerSocket = serverSocket;

    return 0;
}

int AllocateBufferMemory(TextBuffer* buffer, uint16_t bytes) {
    if (buffer->Capacity + bytes > MAX_BUFFER_SIZE) {
        return -1;
    }

    buffer->Content = realloc(buffer->Content, buffer->Capacity + bytes);
    buffer->Capacity += bytes;

    return 0;
}

/// @brief Reads the preamble from the buffer
/// @param buffer the buffer to read from
/// @param clientSocket the socket assigned to the client
/// @return size of preamble on succes, -1 otherwise 
int ReadPreamble(TextBuffer* buffer, SOCKET clientSocket) {
    ASSERT_SUCCESS(AllocateBufferMemory(buffer, KB));

    while (1) {
        int bytes = recv(clientSocket, buffer->Content + buffer->Count, buffer->Capacity - buffer->Count, 0);
        if (bytes == SOCKET_ERROR) {
            return -1;
        }

        printf("recv returned: %d\n", bytes);

        buffer->Count += bytes;
        if (buffer->Count + 1 == buffer->Capacity) {
            ASSERT_SUCCESS(AllocateBufferMemory(buffer, KB));
        }

        buffer->Content[buffer->Count] = '\0';
        char* endOfPreamble = strstr(buffer->Content, "\r\n\r\n");
        if (endOfPreamble) {
            return (int)(endOfPreamble - buffer->Content) + 4;
        } 
        else if (bytes == 0) {
            return -1;
        }
    }

    return -1;
}