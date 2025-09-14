#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "../include/ResultCodes.h"

#define MAX_PATH_LENGTH 128
#define MAX_HEADER_NAME_LENGTH 32
#define MAX_HEADER_VALUE_LENGTH 256
#define MAX_BODY_LENGTH 1024
#define MAX_HEADERS 16

typedef enum {
    GET,
    POST,
    PUT,
    DELETE
} HTTPMethod;

typedef enum {
    V11,
    V2,
    V3
} HTTPVersion;

typedef struct {
    HTTPMethod Method;
    char Path[MAX_PATH_LENGTH];
    HTTPVersion Version;
} HTTPRequestLine;

typedef struct {
    char Name[MAX_HEADER_NAME_LENGTH];
    char Value[MAX_HEADER_VALUE_LENGTH];
} HTTPHeader;

typedef struct {
    HTTPRequestLine RequestLine;
    HTTPHeader Header[MAX_HEADERS];
    char Body[MAX_BODY_LENGTH];
} HTTPRequest;

ResultCode ParseHTTPHeaders(char* buffer, HTTPRequest* request);

#endif 