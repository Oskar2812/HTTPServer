#include "../include/libOskServer.h"
#include "../include/Internal_HTTPListener.h"
#include "../include/Internal_HTTPSender.h"
#include "../include/Internal_HTTPRequestParsing.h"
#include "../include/Internal_HTTPResponseSerialisation.h"
#include "../include/Internal_UtilityMacros.h"
#include "../include/Internal_HTTPServerUtility.h"
#include "../include/Internal_ServerMultithreading.h"
#include "../include/Internal_ServerHTTPS.h"

#include <time.h>

#define MAX_LOG_MESSAGE_LENGTH 256

void GetTimestamp(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm local;
    
    localtime_s(&local, &now);

    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", &local);
}

void SetLogging(HTTPServer* server, FILE* stream, LogLevel minimumLogLevel) {
    server->Logging = (LogDetail){0};

    server->Logging.MinimumLogLevel = minimumLogLevel;
    server->Logging.Stream = stream;
}

void Log(HTTPServer* server, LogLevel level, const char* format, ...) {
    if (server->Logging.Stream == NULL) {
        return;
    }

    if (level >= server->Logging.MinimumLogLevel) {
        char* levelText;
        switch(level) {
            case LOG_DEBUG:
                levelText = "DEBUG";
                break;
            case LOG_INFO:
                levelText = "INFO";
                break;
            case LOG_WARNING:
                levelText = "WARNING";
                break;
            case LOG_ERROR:
                levelText = "ERROR";
                break;
            default:
                levelText = "UNKOWN";
                break;
        }

        char timestamp[32];
        GetTimestamp(timestamp, sizeof(timestamp));

        char message[MAX_LOG_MESSAGE_LENGTH];
        va_list args;
        va_start(args, format);
        vsnprintf(message, sizeof(message), format, args);
        va_end(args);

        EnterCriticalSection(&server->Lock);

        fprintf(server->Logging.Stream, "[%s]: %s: %.*s\n", timestamp, levelText, MAX_LOG_MESSAGE_LENGTH, message);
        fflush(server->Logging.Stream);

        LeaveCriticalSection(&server->Lock);
    }
}

int InitialiseServer(HTTPServer* server, uint16_t port) {
    InitializeCriticalSection(&server->Lock);

    if(InitiateListeningPort(server, port) == -1) {
        Log(server, LOG_ERROR, "Failed to start listening");
        return -1;
    }

    Log(server, LOG_INFO, "Server started listening on port %u", port);

    return 0;
}

int RouteRequest(Endpoint** result, HTTPServer* server, StringView target, HTTPMethod method) {
    for (size_t i = 0; i < server->Endpoints.Count; i++) {
        Endpoint* endpoint = &server->Endpoints.Content[i];
        size_t pathLength = strlen(endpoint->Path);

        if (endpoint->Method != method) {
            continue;
        }

        switch (endpoint->Type) {
            case ROUTE_PREFIX:
                if (target.Count < pathLength) {
                    break;
                }
                if (strncmp(target.Content, endpoint->Path, pathLength) == 0) {
                    if (target.Count == pathLength || target.Content[pathLength] == '/') {
                        *result = endpoint;
                        return 0;    
                    }
                }
            break;
            default:
                if (target.Count != pathLength) {
                    break;
                }
                if (strncmp(target.Content, endpoint->Path, target.Count) == 0) {
                    *result = endpoint;
                    return 0;
                }
            break;
        }
    }

    return -1;
}

int SendNotFound(HTTPResponse* response, SOCKET client, PCtxtHandle securityHandle) {
    SetStatus(response, NOT_FOUND);

    TextBuffer buffer = {0};
    ASSERT_SUCCESS(SerialiseResponse(&buffer, response));

    ASSERT_SUCCESS(SendData(client, &buffer, securityHandle));

    free(buffer.Content);

    return 0;
}

int SendBadRequest(HTTPResponse* response, SOCKET client, PCtxtHandle securityHandle) {
    SetStatus(response, BAD_REQUEST);

    TextBuffer buffer = {0};
    ASSERT_SUCCESS(SerialiseResponse(&buffer, response));

    ASSERT_SUCCESS(SendData(client, &buffer, securityHandle));

    free(buffer.Content);

    return 0;
}

int SendInternalServerError(HTTPResponse* response, SOCKET client, PCtxtHandle securityHandle) {
    SetStatus(response, INTERNAL_SERVER_ERROR);

    TextBuffer buffer = {0};
    ASSERT_SUCCESS(SerialiseResponse(&buffer, response));

    ASSERT_SUCCESS(SendData(client, &buffer, securityHandle));

    free(buffer.Content);

    return 0;
}

int AddBodyHeader(HTTPResponse* response) {
     StringView headerName = {
        .Content = "Content-Length",
        .Count = 14,
    };

    char* buffer = malloc(sizeof(char) * 32);
    int digits = snprintf(buffer, sizeof(char) * 32, "%zu", response->Body.Count);

    StringView headerValue = {
        .Content = buffer,
        .Count = digits,
    };

    FieldLine header = {
        .FieldName = headerName,
        .FieldValue = headerValue,
    }; 

    FieldLine* contentLength = GetHeaderValue(&response->Headers, "Content-Length", 14);
    if (contentLength == NULL) {
        ASSERT_SUCCESS(AddHeader(&response->Headers, header));
    }
    else {
        contentLength->FieldValue = headerValue;
    }

    return 0;
}

int HandleConnection(HTTPServer* server, SOCKET client) {
    TextBuffer buffer = {0};
    CtxtHandle securityContext = {0};

    DWORD timeout = 5000;  // milliseconds — 30 seconds here
    int result = setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    if (result == SOCKET_ERROR) {
        printf("setsockopt failed: %d\n", WSAGetLastError());
    }

    if (server->Secure && DoTLSHandshake(&securityContext, &buffer, client, server->HTTPSCredentials) == -1) {
        Log(server, LOG_ERROR, "Socket %d - Failed to establish TLS handshake", client);
        closesocket(client);
        free(buffer.Content);
        return -1;
    }

    HTTPResponse response = {0};
    SetVersion(&response, V_ONE);

    int preambleOffset = ReadPreamble(&buffer, client, server->Secure ? &securityContext : NULL);
    if (preambleOffset == -1) {
        Log(server, LOG_ERROR, "Socket %d - Failed to read request line and headers into buffer", client);
        ASSERT_AND_LOG_SUCCESS(server, SendInternalServerError(&response, client, server->Secure ? &securityContext : NULL), "Socket %d - Failed to send internal server error response");
        closesocket(client);
        free(buffer.Content);
        return 0;
    }

    StringView preamble = {
        .Content = buffer.Content,
        .Count = preambleOffset,
    };

    HTTPRequest request = {0};
    int preambleSize = ParseMessagePreamble(&request, preamble.Content, preamble.Count);
    if (preambleSize == -1) {
        Log(server, LOG_ERROR, "Socket %d - Failed to parse message request line and headers", client);
        ASSERT_AND_LOG_SUCCESS(server, SendBadRequest(&response, client, server->Secure ? &securityContext : NULL), "Socket %d - Failed to send bad request response");
        closesocket(client);

        free(request.Headers.FieldLines);
        free(buffer.Content);
        return 0;
    }

    int bodySize = ReadBody(&buffer, preambleOffset, &request, client, server->Secure ? &securityContext : NULL);
    if (bodySize == -1) {
        Log(server, LOG_ERROR, "Socket %d - Failed to read message body into buffer", client);
        ASSERT_AND_LOG_SUCCESS(server, SendBadRequest(&response, client, server->Secure ? &securityContext : NULL), "Socket %d - Failed to send bad request response");
        closesocket(client);
        free(buffer.Content);
        return 0;
    }

    Log(server, LOG_INFO, "Socket %d - Message of %d bytes read", client, buffer.Count);

    Endpoint* endpoint = NULL;
    if (RouteRequest(&endpoint, server, request.RequestLine.Target.Target, request.RequestLine.Method) == -1) {
        Log(server, LOG_WARNING, "Socket %d - Endpoint %.*s not found", 
            client, request.RequestLine.Target.Target.Count, request.RequestLine.Target.Target.Content);
        ASSERT_AND_LOG_SUCCESS(server, SendNotFound(&response, client, server->Secure ? &securityContext : NULL), "Socket %d - Failed to send not found response");

        closesocket(client);

        free(buffer.Content);
        free(request.Headers.FieldLines);
        free(request.RequestLine.Target.QueryParameters);

        return 0;
    }
           
    if (endpoint->Callback(server, &response, &request, endpoint->Context) == -1) {
        Log(server, LOG_ERROR, "Socket %d - Endpoint callback failed", client);
        ASSERT_AND_LOG_SUCCESS(server, SendInternalServerError(&response, client, server->Secure ? &securityContext : NULL), "Socket %d - Failed to send internal server error response");
        closesocket(client);

        free(buffer.Content);
        free(request.Headers.FieldLines);
        free(request.RequestLine.Target.QueryParameters);
        free(response.Headers.FieldLines);

        return 0;
    }

    FieldLine* contentLengthHeader = NULL;
    if (response.Body.Count > 0) {
        AddBodyHeader(&response);
        contentLengthHeader = GetHeaderValue(&response.Headers, "Content-Length", 14);
    }

    TextBuffer sendBuffer = {0};
    if (SerialiseResponse(&sendBuffer, &response) == -1) {
        Log(server, LOG_ERROR, "Socket %d - Endpoint callback failed", client);
        ASSERT_AND_LOG_SUCCESS(server, SendInternalServerError(&response, client, server->Secure ? &securityContext : NULL), "Socket %d - Failed to send internal server error response");
        closesocket(client);

        free(sendBuffer.Content);
        free(buffer.Content);
        free(request.Headers.FieldLines);
        free(request.RequestLine.Target.QueryParameters);
        if (contentLengthHeader != NULL) {
            free(contentLengthHeader->FieldValue.Content);
        }
        free(response.Headers.FieldLines);

        return 0;
    }

    ASSERT_AND_LOG_SUCCESS(server, SendData(client, &sendBuffer, server->Secure ? &securityContext : NULL), "Socket %d - Failed to send response to client", client);

    closesocket(client);

    free(sendBuffer.Content);
    free(buffer.Content);
    free(request.Headers.FieldLines);
    free(request.RequestLine.Target.QueryParameters);
    free(response.Body.Content);
    if (contentLengthHeader != NULL) {
        free(contentLengthHeader->FieldValue.Content);
    }
    free(response.Headers.FieldLines);

    Log(server, LOG_INFO, "Socket %d - Connection handled", client);

    return 0;
}

int StartServer(HTTPServer* server) {
    ConnectionQueue queue = {0};
    QueueInit(&queue);

    HANDLE workerThreads[WORKER_COUNT];
    WorkerContext ctx = {
        .Queue = &queue,
        .Server = server
    };

    (void)workerThreads;

    for (int i = 0; i < WORKER_COUNT; i++) {
        workerThreads[i] = CreateThread(NULL, 0, WorkerThreadFunc, &ctx, 0, NULL);
    }
    
    while (1) {
        SOCKET clientSocket = accept(server->ServerSocket, NULL, NULL);

        Log(server, LOG_INFO, "Socket %d - Connection recieved", clientSocket);

        QueuePush(&queue, clientSocket);
    }
}

int StopServer(HTTPServer* server) {
    for (size_t i = 0; i < server->Endpoints.Count; i++) {
        free(server->Endpoints.Content[i].Context);
    }

    free(server->Endpoints.Content);
    closesocket(server->ServerSocket);
    WSACleanup();

    return 0;
}

int AddEndpoint(HTTPServer* server, const char* path, HTTPMethod method, EndpointCallback callback) {
    server->Endpoints.Count += 1;
    server->Endpoints.Content = realloc(server->Endpoints.Content, sizeof(Endpoint) * server->Endpoints.Count);

    memcpy(server->Endpoints.Content[server->Endpoints.Count - 1].Path, path, MAX_PATH_LENGTH);
    server->Endpoints.Content[server->Endpoints.Count - 1].Callback = callback;
    server->Endpoints.Content[server->Endpoints.Count - 1].Method = method;
    server->Endpoints.Content[server->Endpoints.Count - 1].Context = NULL;
    server->Endpoints.Content[server->Endpoints.Count - 1].Type = ROUTE_EXACT;

    return 0;
}

int AddFileEndpoint(HTTPServer* server, const char* path, HTTPMethod method, char* filePath) {
    server->Endpoints.Count += 1;
    server->Endpoints.Content = realloc(server->Endpoints.Content, sizeof(Endpoint) * server->Endpoints.Count);
    memcpy(server->Endpoints.Content[server->Endpoints.Count - 1].Path, path, MAX_PATH_LENGTH);
    server->Endpoints.Content[server->Endpoints.Count - 1].Callback = FileCallback;
    server->Endpoints.Content[server->Endpoints.Count - 1].Method = method;
    server->Endpoints.Content[server->Endpoints.Count - 1].Context = filePath;
    server->Endpoints.Content[server->Endpoints.Count - 1].Type = ROUTE_EXACT;

    return 0;
}

int AddStaticDirectoyEndpoint(HTTPServer* server, const char* path, char* directory) {
    DirectoryPathMapping* mapping =
    malloc(sizeof(DirectoryPathMapping));

    if (mapping == NULL) {
        return -1;
    }

    strcpy(mapping->Directory, directory);
    strcpy(mapping->Path, path);

    server->Endpoints.Count += 1;
    server->Endpoints.Content = realloc(server->Endpoints.Content, sizeof(Endpoint) * server->Endpoints.Count);
    memcpy(server->Endpoints.Content[server->Endpoints.Count - 1].Path, path, MAX_PATH_LENGTH);
    server->Endpoints.Content[server->Endpoints.Count - 1].Callback = DirectoryCallback;
    server->Endpoints.Content[server->Endpoints.Count - 1].Context = mapping;
    server->Endpoints.Content[server->Endpoints.Count - 1].Method = GET;
    server->Endpoints.Content[server->Endpoints.Count - 1].Type = ROUTE_PREFIX;

    return 0;
}