#include "../include/libOskServer.h"

int main() {
    HTTPServer server = {0};

    SetLogging(&server, stdout, LOG_DEBUG);
    InitialiseServer(&server, 8080);

    AddStaticDirectoyEndpoint(&server, "/static", "bin/www");

    StartServer(&server);
    StopServer(&server);
}