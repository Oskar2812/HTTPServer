#include <stdio.h>

#include "../include/libOskServer.h"

#include <windows.h>

int PostCallback(HTTPServer* server, HTTPResponse* response , HTTPRequest* request, void* context) {
    (void)context;
    (void)request;

    Log(server, LOG_INFO, "Endpoint hit, sleeping...");

    Sleep(10 * 1000);

    SetStatus(response, OK);

    return 0;
}

int main() {
    HTTPServer server = {0};

    SetLogging(&server, stdout, LOG_DEBUG);
    InitialiseServer(&server, 8080);

    AddEndpoint(&server, "/sleep", GET ,PostCallback);

    StartServer(&server);
    StopServer(&server);

    return 0;
}