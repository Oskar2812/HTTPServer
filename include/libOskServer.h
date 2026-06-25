#ifndef LIB_OSK_SERVER_H
#define LIB_OSK_SERVER_H

#include <stdint.h>

#define CRFL "\r\n"

//######################################## HTTP Request #######################################################################################

typedef struct {
    char* Content;
    size_t Count;
} StringView;

typedef struct {
    int value;
} HTTPRequest;

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

#endif


