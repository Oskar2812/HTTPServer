#include "../include/libOskServer.h"
#include "../include/Internal_HTTPListener.h"
#include "../include/Internal_HTTPRequestParsing.h"
#include "../include/Internal_UtilityMacros.h"

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

        fprintf(server->Logging.Stream, "[%s]: %s: %.*s\n", timestamp, levelText, MAX_LOG_MESSAGE_LENGTH, message);
        fflush(server->Logging.Stream);
    }
}

int InitialiseServer(HTTPServer* server, uint16_t port) {
      if(InitiateListeningPort(server, 8080) == -1) {
        Log(server, LOG_ERROR, "Failed to start listening");
        return -1;
    }

    Log(server, LOG_INFO, "Server started listening on port %u", port);

    return 0;
}

int HandleConnection(HTTPServer* server, SOCKET client) {
    TextBuffer buffer = {0};

    ASSERT_AND_LOG_SUCCESS(server, -1, "Testing %d", 69420);

    int preambleOffset = ReadPreamble(&buffer, client);
    ASSERT_AND_LOG_SUCCESS(server, preambleOffset, "Socket %d - Failed to read request line and headers into buffer", client);

    StringView preamble = {
        .Content = buffer.Content,
        .Count = preambleOffset,
    };

    HTTPRequest request = {0};
    int preambleSize = ParseMessagePreamble(&request, preamble.Content, preamble.Count);
    ASSERT_AND_LOG_SUCCESS(server, preambleSize, "Socket %d - Failed to parse message request line and headers", client);

    int bodySize = ReadBody(&buffer, preambleOffset, &request, client);
    ASSERT_AND_LOG_SUCCESS(server, bodySize, "Socket %d - Failed to read message body into buffer", client);

    PRINT_SV(request.Body);

    Log(server, LOG_INFO, "Message of size %d bytes read on socket %d", buffer.Count, client);

    closesocket(client);

    free(buffer.Content);
    free(request.Headers.FieldLines);

    return 1;
}

int StartServer(HTTPServer* server) {
    
    while (1) {
        SOCKET clientSocket = accept(server->ServerSocket, NULL, NULL);

        Log(server, LOG_INFO, "Socket %d - Connection recieved", clientSocket);

        HandleConnection(server, clientSocket);

        Log(server, LOG_INFO, "Socket %d - Connection handeled", clientSocket);
    }
}

int StopServer(HTTPServer* server) {
    closesocket(server->ServerSocket);
    WSACleanup();

    return 0;
}