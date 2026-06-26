#ifndef LIB_OSK_SERVER_H
#define LIB_OSK_SERVER_H

#include <stdint.h>
//######################################## HTTP Request #######################################################################################

#define MAX_HEADERS 50

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
    DELETE
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
} HTTPRequest;

int ParseRequestLine(RequestLine* requestLine, char* buffer, size_t bufferCount);

#endif


