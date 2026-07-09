#include "../include/libOskServer.h"

typedef struct {
    char Directory[MAX_PATH_LENGTH];
    char Path[MAX_PATH_LENGTH];
} DirectoryPathMapping;

int FileCallback(HTTPServer* server, HTTPResponse* response, HTTPRequest* request, void* context);
int DirectoryCallback(HTTPServer* server, HTTPResponse* response, HTTPRequest* request, void* context);

