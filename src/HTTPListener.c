#ifndef HTTPLISTENER_C
#define HTTPLISTENER_C

#include "../include/libOskServer.h"
#include "../include/Internal_UtilityMacros.h"
#include "../include/Internal_HTTPListener.h"
#include "../include/Internal_ServerHTTPS.h"

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

int ReadBytes(TextBuffer* buffer, SOCKET clientSocket) {
    int bytes = recv(clientSocket, buffer->Content + buffer->Count, buffer->Capacity - buffer->Count, 0);
    if (bytes == SOCKET_ERROR || bytes == 0) {
        return -1;
    }

    buffer->Count += bytes;
    if (buffer->Count + 1 >= buffer->Capacity) {
        ASSERT_SUCCESS(AllocateBufferMemory(buffer, KB));
    }

    buffer->Content[buffer->Count] = '\0';

    return bytes;
}

int ReadPreamble(TextBuffer* buffer, SOCKET clientSocket, PCtxtHandle securityContext) {
    if (buffer->Capacity == 0) {
        ASSERT_SUCCESS(AllocateBufferMemory(buffer, KB));
    }
    while (1) {
        if (securityContext != NULL && buffer->Count > 0) {
            int64_t decrypted = DecryptBytes(buffer, securityContext);
            ASSERT_SUCCESS(decrypted);
        }
        else {
            buffer->DecryptedCount = buffer->Count;
        }

        if (buffer->Count > 0) {
            char* endOfPreamble = strstr(buffer->Content, "\r\n\r\n");
            if (endOfPreamble) {
                return (int)(endOfPreamble - buffer->Content) + 4;
            } 
        }
        
        int bytes = ReadBytes(buffer, clientSocket);
        ASSERT_SUCCESS(bytes);
    }

    return -1;
}

int ParseSizeT(const char* str, size_t len, size_t* result) {    
    size_t value = 0;
    for (size_t i = 0; i < len; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return -1;        
        }      
        value = value * 10 + (str[i] - '0');    
    } 
    
    *result = value;    
    
    return 0;
}

int ReadBody(TextBuffer* buffer, size_t preambleOffset, HTTPRequest* request, SOCKET clientSocket, PCtxtHandle securityContext) {
    FieldLine* bodyHeader = GetHeaderValue(&request->Headers, "Content-Length", 14);
    if (bodyHeader == NULL) {
        request->Body.Content = buffer->Content + preambleOffset;
        request->Body.Count = 0;
        return 0;
    }

    size_t bodySize;
    ASSERT_SUCCESS(ParseSizeT(bodyHeader->FieldValue.Content, bodyHeader->FieldValue.Count, &bodySize));

    if (bodySize > buffer->DecryptedCount - preambleOffset) {
        int bytesRemaining = bodySize - (buffer->DecryptedCount - preambleOffset);

        while (1) {
            int bytes = ReadBytes(buffer, clientSocket);

            if (securityContext != NULL) {
                size_t decryptedBefore = buffer->DecryptedCount;

                ASSERT_SUCCESS(DecryptBytes(buffer, securityContext));
                bytes = buffer->DecryptedCount - decryptedBefore;
            }

            bytesRemaining -= bytes;

            if (bytesRemaining <= 0) {
                break;
            }
        }
    }

    request->Body.Content = buffer->Content + preambleOffset;
    request->Body.Count = bodySize;

    return bodySize;
}

#endif