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
} MessageHeaders;

typedef struct {
    RequestLine RequestLine;
    MessageHeaders Headers;
    StringView Body;
} HTTPRequest;

/// @brief Retrieves the value of a specifc header
/// @param request the request to search
/// @param headerName name of the header
/// @param size of header name (do not include null terminator in count)
/// @return A FieldLine containing the header. (Note this points at the actual underlying memory stroing the requets modifiying the strings within will modify the request)
FieldLine* GetHeaderValue(HTTPRequest* request, char headerName[MAX_HTTP_HEADER_NAME_LENGTH], size_t nameCount);

//######################################## HTTP Response #########################################################################################

typedef enum {
    OK,
    NOT_FOUND,
    BAD_REQUEST,
    INTERNAL_SERVER_ERROR,
} HTTPStatus;

typedef struct {
    HTTPVersion Version;
    HTTPStatus Status;
} StatusLine;

typedef struct {
    StatusLine Status;
    MessageHeaders Headers;
    StringView Body;
} HTTPResponse;

/// @brief Sets the HTTP version of the response
/// @param response the response
/// @param version the version
/// @return 0 on success, -1 otherwise
int SetVersion(HTTPResponse* response, HTTPVersion version);

/// @brief Sets the status of the response
/// @param response the response
/// @param status the status 
/// @return 0 on success, -1 on failure
int SetStatus(HTTPResponse* response, HTTPStatus status);

/// @brief Sets the body of the response
/// @param response the response
/// @param body pointer to the body
/// @param bodySize size of the body
/// @return 0 on success, -1 on failure
int SetBody(HTTPResponse* response, char* body, size_t bodySize);

/// @brief Adds a header to the response
/// @param response the response
/// @param header header to add
/// @return 0 on success, -1 otherwise
int AddHeader(MessageHeaders* list, FieldLine header);

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

typedef struct HTTPServer HTTPServer;

typedef int (*EndpointCallback)(HTTPServer* server, HTTPResponse* response, HTTPRequest* request);

typedef struct {
    char Path[MAX_PATH_LENGTH];
    EndpointCallback Callback;
} Endpoint;

typedef struct {
    size_t Count;
    Endpoint* Content;
} EndpointList;

struct HTTPServer {
    SOCKET ServerSocket;
    LogDetail Logging;
    EndpointList Endpoints;
};

/// @brief Sets the loggin policy for the specified server
/// @param server the server to set logging for 
/// @param stream the stream to write the logs inot i.e. stdout or a specifc file of your choice
/// @param minimumLogLevel the minimum log level to include i.e. an argumetn of LOG_WARNING will include logs of warning and error
void SetLogging(HTTPServer* server, FILE* stream, LogLevel minimumLogLevel);

/// @brief Logs input
/// @param server the server to log on
/// @param level the level to log at
/// @param format the message log
/// @param extra formating arguments
void Log(HTTPServer* server, LogLevel level, const char* format, ...);

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

/// @brief Adds an endpoint to the server
/// @param server server to add too
/// @param path path to endpoint (oriign form)
/// @param callback callback to call when endpoint gets hit
/// @return 0 on success, -1 on failure
int AddEndpoint(HTTPServer* server, char path[MAX_PATH_LENGTH], EndpointCallback callback);

#endif