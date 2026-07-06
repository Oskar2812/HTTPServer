#ifndef LIB_OSK_SERVER_H
#define LIB_OSK_SERVER_H

#include <stdint.h>
#include <winsock2.h>
//######################################## HTTP Request #######################################################################################

#define MAX_HEADERS 50
#define MAX_REQUESTS 10
#define MAX_HTTP_HEADER_NAME_LENGTH 256 

typedef struct {
    char* Content;
    size_t Count;
} StringView;

// TODO: Add all HTTP methods 
typedef enum {
    NO_METHOD,
    GET,
    POST,
    PUT,
    DELETE_METHOD
} HTTPMethod;

typedef enum {
    NO_VERSION,
    V_ONE,
    V_TWO,
    V_THREE 
} HTTPVersion;

typedef struct {
    HTTPMethod Method;
    StringView Target;
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

typedef struct {
    SOCKET ServerSocket;
    SOCKET ClientSockets[MAX_REQUESTS];
} HTTPServer;

/// @brief Retrieves the value of a specifc header
/// @param request the request to search
/// @param headerName name of the header
/// @param size of header name (do not include null terminator in count)
/// @return A FieldLine containing the header. (Note this points at the actual underlying memory stroing the requets modifiying the strings within will modify the request)
FieldLine* GetHeaderValue(HTTPRequest* request, char headerName[MAX_HTTP_HEADER_NAME_LENGTH], size_t nameCount);

#endif