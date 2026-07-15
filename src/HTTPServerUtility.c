#include "../include/libOskServer.h"
#include "../include/Internal_HTTPServerUtility.h"

/// @brief Checks that a static-file request path contains no ".." segments, so it can't
/// escape the directory it's being served from. Checks both '/' and '\' separators since
/// Windows accepts either, and rejects embedded NUL bytes so a fopen() call downstream
/// can't be truncated onto an unintended path.
/// @param path the (not necessarily null-terminated) remainder of the request target
/// @param length size of path
/// @return 0 if the path is safe to append to a base directory, -1 otherwise
int IsSafeStaticPath(const char* path, size_t length) {
    size_t segmentStart = 0;

    for (size_t i = 0; i <= length; i++) {
        int atSeparator = (i == length || path[i] == '/' || path[i] == '\\');

        if (!atSeparator && path[i] == '\0') {
            return -1;
        }

        if (atSeparator) {
            size_t segmentLength = i - segmentStart;
            if (segmentLength == 2 && path[segmentStart] == '.' && path[segmentStart + 1] == '.') {
                return -1;
            }
            segmentStart = i + 1;
        }
    }

    return 0;
}

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
                    .Content = "text/css",
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

    if (IsSafeStaticPath(remainder, remainderLength) == -1) {
        SetStatus(response, NOT_FOUND);
        return 0;
    }

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