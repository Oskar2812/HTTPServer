#include "../include/libOskServer.h"
#include "../include/Internal_HTTPServerUtility.h"

int FileCallback(HTTPServer* server, HTTPResponse* response, HTTPRequest* request, void* context) {
    (void)server;
    (void)request;

    const char* filePath = context;

    FILE* file = fopen(filePath, "rb");
    if (file == NULL) {
        SetStatus(response, NOT_FOUND);
        return 0;
    }

    fseek(file, 0, SEEK_END);

    long fileSize = ftell(file);
    if (fileSize < 0) {
        fclose(file);
        return -1;
    }

    rewind(file);

    char* buffer = malloc((size_t)fileSize);
    if (buffer == NULL) {
        fclose(file);
        return -1;
    }

    size_t bytesRead = fread(buffer, 1, (size_t)fileSize, file);
    if (bytesRead != (size_t)fileSize) {
        free(buffer);
        fclose(file);
        return -1;
    }

    fclose(file);

    SetBody(response, buffer, fileSize);
    SetStatus(response, OK);

    return 0;
}