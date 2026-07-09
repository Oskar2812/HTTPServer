#include <stdio.h>
#include "../include/libOskServer.h"

int Callback(HTTPServer* server, HTTPResponse* response , HTTPRequest* request, void* context) {
    (void)context;
    (void)request;

    Log(server, LOG_INFO, "Ok endpoint hit");

    SetStatus(response, OK);

    return 0;
}

int HTMLCallback(HTTPServer* server, HTTPResponse* response , HTTPRequest* request, void* context) {
    (void)context;
    (void)request;
    (void)server;

    char* html =
        "<!DOCTYPE html>\r\n"
        "<html>\r\n"
        "<head>\r\n"
        "    <title>HTTP Server</title>\r\n"
        "</head>\r\n"
        "<body>\r\n"
        "    <h1>Hello Lewis from my C HTTP Server!</h1>\r\n"
        "    <p>If you can see this page, it works.</p>\r\n"
        "</body>\r\n"
        "</html>\r\n";

    SetBody(response, html, strlen(html));

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

    SetStatus(response, OK);

    return 0;
}

int PostCallback(HTTPServer* server, HTTPResponse* response , HTTPRequest* request, void* context) {
    (void)context;
    Log(server, LOG_INFO, "Endpoint hit!!!!!!");

    SetBody(response, request->Body.Content, request->Body.Count);

    SetStatus(response, OK);

    return 0;
}

int main() {
    HTTPServer server = {0};

    SetLogging(&server, stdout, LOG_DEBUG);
    InitialiseServer(&server, 8080);

    AddEndpoint(&server, "/echo", POST ,PostCallback);
    AddEndpoint(&server, "/ok", GET , Callback);
    AddEndpoint(&server, "/html", GET ,HTMLCallback);

    StartServer(&server);
    StopServer(&server);

    return 0;
}