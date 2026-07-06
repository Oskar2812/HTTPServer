#include "../include/libOskServer.h"
#include "../include/InternalUtilityMacros.h"

#define MAX_BUFFER_SIZE 32768 + 1048576
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

        buffer->Count += bytes;
        if (buffer->Count + 1 >= buffer->Capacity) {
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

/// @brief Reads the body into the buffer and points the string view in the requets struct at it. 
/// Note: I don't know if it's better to simply have this method read the body into the TextBuffer and the have a seperate method to add it into the request struct
/// as that keeps things consistent between the networking and parsing but we dont do any actual parsing here and we pass in the request struct anyway as we need to lok at the headers
/// to read thie body so we might as well?
/// @todo: Support Transfer-Encoding
/// @param buffer The buffer to read into 
/// @param request the underlying request
/// @param clientSocket the socket assigned to the client 
/// @return size of body on success, -1 otherwise 
int ReadBody(TextBuffer* buffer, size_t preambleOffset, HTTPRequest* request, SOCKET clientSocket) {
    FieldLine* bodyHeader = GetHeaderValue(request, "Content-Length", 14);
    if (bodyHeader == NULL) {
        return -1;
    }

    size_t bodySize;
    ASSERT_SUCCESS(ParseSizeT(bodyHeader->FieldValue.Content, bodyHeader->FieldValue.Count, &bodySize));

    if (bodySize > buffer->Count - preambleOffset) {
        int bytesRemaining = bodySize - (buffer->Count - preambleOffset);

        while (1) {
            int bytes = recv(clientSocket, buffer->Content + buffer->Count, buffer->Capacity - buffer->Count, 0);
            if (bytes == SOCKET_ERROR) {
                return -1;
            }

            buffer->Count += bytes;
            if (buffer->Count + 1 >= buffer->Capacity) {
                ASSERT_SUCCESS(AllocateBufferMemory(buffer, KB));
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