#ifndef LIB_OSK_SERVER_H
#define LIB_OSK_SERVER_H

#include <stdint.h>
#include <winsock2.h>
#include <stdio.h>
//######################################## HTTP Request #######################################################################################

#define MAX_HEADERS 50
#define MAX_REQUESTS 10
#define MAX_HTTP_HEADER_NAME_LENGTH 256 
#define MAX_PATH_LENGTH 256

typedef struct {
    char* Content;
    size_t Count;
} StringView;

// TODO: Add all HTTP methods 
typedef enum {
    NO_METHOD = -1,
    GET,
    POST,
    PUT,
    DELETE_METHOD
} HTTPMethod;

typedef enum {
    NO_VERSION = -1,
    V_ONE,
    V_TWO,
    V_THREE 
} HTTPVersion;

typedef struct {
    StringView QueryName;
    StringView QueryValue;
} QueryParameter;

typedef struct {
    StringView Target;
    QueryParameter* QueryParameters;
    size_t QueryCount;
} RequestTarget;

typedef struct {
    HTTPMethod Method;
    RequestTarget Target;
    HTTPVersion Version;
} RequestLine;

typedef struct {
    StringView FieldName;
    StringView FieldValue;
} FieldLine;

typedef struct {
    FieldLine* FieldLines;
    size_t Count;
    size_t Capacity;
} RequestHeaders;

typedef struct {
    RequestLine RequestLine;
    RequestHeaders Headers;
    StringView Body;
} HTTPRequest;

/// @brief Retrieves the value of a specifc header
/// @param request the request to search
/// @param headerName name of the header
/// @param size of header name (do not include null terminator in count)
/// @return A FieldLine containing the header. (Note this points at the actual underlying memory stroing the requets modifiying the strings within will modify the request)
FieldLine* GetHeaderValue(HTTPRequest* request, char headerName[MAX_HTTP_HEADER_NAME_LENGTH], size_t nameCount);

//######################################## HTTP Server #########################################################################################
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
} LogLevel;

typedef struct {
    FILE* Stream;
    LogLevel MinimumLogLevel;
} LogDetail;

//typedef int (*EndpointCallback)(HTTPRequest* request, void (*Log)(HTTPServer* server, LogLevel level, const char* format, ...));

typedef struct {
    char Path[MAX_PATH_LENGTH];
   // EndpointCallback Callback;
} Endpoint;

typedef struct {
    size_t Count;
    Endpoint* Content;
} EndpointList;

typedef struct {
    SOCKET ServerSocket;
    LogDetail Logging;
    EndpointList Endpoints;
} HTTPServer;

/// @brief Sets the loggin policy for the specified server
/// @param server the server to set logging for 
/// @param stream the stream to write the logs inot i.e. stdout or a specifc file of your choice
/// @param minimumLogLevel the minimum log level to include i.e. an argumetn of LOG_WARNING will include logs of warning and error
void SetLogging(HTTPServer* server, FILE* stream, LogLevel minimumLogLevel);

/// @brief Initilaises the server on a specified port
/// @param server the server to init
/// @param port the port to listen on
/// @return 0 on success, -1 otherwise
int InitialiseServer(HTTPServer* server, uint16_t port);

/// @brief Starts the server loop of waiting for a connection and then handling it 
/// @param server the server to start
/// @return 0 on success, -1 on failure - not currently no way to exit out of this function without stopping the program (or a crash)
int StartServer(HTTPServer* server);

/// @brief Cleans up all socjets and WSA stuff
/// @param server the server to stop
/// @return 0 on succes, -1 on failure
int StopServer(HTTPServer* server);

#endif