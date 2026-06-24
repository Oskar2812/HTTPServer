#ifndef LIB_OSK_SERVER_H
#define LIB_OSK_SERVER_H

#include <stdint.h>

#define CRFL "\r\n"

//######################################## HTTP Request #######################################################################################
typedef struct {
    int value;
} HTTPRequest;

typedef enum {
    GET
} HTTPMethod;

typedef enum {
    V_ONE
} HTTPVersion;

typedef struct {
    HTTPMethod Method;
    char* Target;
    HTTPVersion Version;
} RequestLine;



#endif


