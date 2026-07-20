#include "../include/libOskServer.h"

int main() {
    HTTPServer server = {0};

    EnableHTTPS(&server, "FE3690A80CA297715F19A17CA987DEF1BAD17F60", "MY");

    SetLogging(&server, stdout, LOG_DEBUG);
    InitialiseServer(&server, 8080);

    AddStaticDirectoyEndpoint(&server, "/static", "bin/www");

    StartServer(&server);
    StopServer(&server);
}