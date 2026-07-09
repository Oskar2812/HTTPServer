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

    char* extension = strrchr(filePath, '.');
    if (extension != NULL) {
        if (strcmp(extension, ".html") == 0) {
            AddHeader(&response->Headers, (FieldLine){
                .FieldName = (StringView) {
                    .Content = "Content-Type",
                    .Count = 12
                },
                .FieldValue = (StringView) {
                    .Content = "text/html",
                    .Count = 9
                }
            });
        }
        else if (strcmp(extension, ".css") == 0) {
            AddHeader(&response->Headers, (FieldLine){
                .FieldName = (StringView) {
                    .Content = "Content-Type",
                    .Count = 12
                },
                .FieldValue = (StringView) {
                    .Content = "css/html",
                    .Count = 8
                }
            });
        }
        else if (strcmp(extension, ".js") == 0) {
            AddHeader(&response->Headers, (FieldLine){
                .FieldName = (StringView) {
                    .Content = "Content-Type",
                    .Count = 12
                },
                .FieldValue = (StringView) {
                    .Content = "application/javascript",
                    .Count = 22
                }
            });
        }
    }

    SetBody(response, buffer, fileSize);
    SetStatus(response, OK);

    return 0;
}

int DirectoryCallback(HTTPServer* server, HTTPResponse* response, HTTPRequest* request, void* context) {
    DirectoryPathMapping* mapping = context;

    StringView target = request->RequestLine.Target.Target;

    size_t prefixLength = strlen(mapping->Path);

    char* remainder = target.Content + prefixLength;
    size_t remainderLength = target.Count - prefixLength;

    size_t pathLength = strlen(mapping->Directory) + remainderLength + 1;

    char* filePath = malloc(pathLength);

    snprintf(filePath,
            pathLength,
            "%s%.*s",
            mapping->Directory,
            (int)remainderLength,
            remainder);

    if (FileCallback(server, response, request, filePath) == -1) {
        free(filePath);
        return -1;
    }

    free(filePath);
    return 0;
}