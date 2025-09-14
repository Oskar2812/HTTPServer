#include <string.h>
#include <stdlib.h>

#include "../include/HTTPRequest.h"

ResultCode ParseHTTPPath(char* buffer, HTTPRequest* request, int* offset) {
    int position = 0;
    while (1) {
        char next;
        next = buffer[position];
        if (next == ' ') {
            *offset = position + 1;
            return Success;
        }
        request->RequestLine.Path[position] = next;
        position += 1;
        if (position == MAX_PATH_LENGTH) {
            return InsufficientMemory;
        }
    }
}

ResultCode ParseHTTPMethod(char* buffer, HTTPRequest* request, int* offset) {
    if (strncmp(buffer, "GET", 3) == 0) {
        request->RequestLine.Method = GET;
        *offset = 4;
        return Success;
    }
    if (strncmp(buffer, "PUT", 3) == 0) {
        request->RequestLine.Method = PUT;
        *offset = 4;
        return Success;
    }

    if (strncmp(buffer, "POST", 4) == 0) {
        request->RequestLine.Method = POST;
        *offset = 5;
        return Success;
    }

    if (strncmp(buffer, "DELETE", 6) == 0) {
        request->RequestLine.Method = DELETE;
        *offset = 7;
        return Success;
    }

    return InvalidHttpMethod;
}

ResultCode ParseHTTPVersion(char* buffer, HTTPRequest* request, int* offset) {
    if (strncmp(buffer, "HTTP/1.1", 8) == 0) {
        request->RequestLine.Version = V11;
        *offset = 8;
        return Success;
    }
    if (strncmp(buffer, "HTTP/2", 6) == 0) {
        request->RequestLine.Version = V2;
        *offset = 6;
        return Success;
    }
    if (strncmp(buffer, "HTTP/3", 6) == 0) {
        request->RequestLine.Version = V3;
        *offset = 6;
        return Success;
    }

    return InvalidHttpVersion;
}

ResultCode ParseHTTPHeader(char* buffer, HTTPRequest* request, int nHeader, int* offset) {
    int position = 0;
    while(1) {
        char next = buffer[position];
        if (next == ':') {
            position += 2;
            break;
        }
        request->Header[nHeader].Name[position] = next;
        position += 1;
        if (position == MAX_HEADER_NAME_LENGTH) {
            return InsufficientMemory;
        }
    }
    int headerNameSize = position;
    
    while(1) {
        char next = buffer[position];
        if (strncmp(buffer + position, "\r\n", 2) == 0) {
            position += 2;
            break;
        }
        request->Header[nHeader].Value[position - headerNameSize] = next;
        position += 1;
        if (position -  headerNameSize == MAX_HEADER_VALUE_LENGTH) {
            return InsufficientMemory;
        }
    }

    *offset = position;
    return Success;
}

ResultCode ParseHTTPHeaders(char* buffer, int bufferSize, HTTPRequest* request, int* headerSize) {
    int offset = 0;
    int position = 0;
    int result = ParseHTTPMethod(buffer, request, &offset);
    if (result > 0) {
        return result;
    }
    buffer += offset;
    position += offset;

    result = ParseHTTPPath(buffer, request, &offset);
    if (result > 0) {
        return result;
    }
    buffer += offset;
    position += offset;

    result = ParseHTTPVersion(buffer, request, &offset);
    if (result > 0) {
        return result;
    }
    buffer += offset;
    if (strncmp(buffer, "\r\n", 2) != 0) {
        return InvalidRequestLine;
    }
    buffer += 2;
    position += offset;

    int nHeaders = 0;
    while (1) {
        char next = buffer[0];
        if (strncmp(buffer, "\r\n", 2) == 0 || next == 0) {
            break;
        }
        result = ParseHTTPHeader(buffer, request, nHeaders, &offset);
        if (result > 0) {
            return result;
        }
        nHeaders += 1;
        buffer += offset;
        position += offset;
        if (position >= bufferSize) {
            return HeadersTooLong;
        }
    }
    request->NHeaders = nHeaders;
    *headerSize = position;
    return Success;
}

ResultCode GetBodySize(HTTPRequest* request) {
    switch (request->RequestLine.Method) {
        case POST:
        case PUT:
            char bodySize[MAX_HEADER_VALUE_LENGTH];
            int result = GetHeaderValue(request, "Content-Length", bodySize);
            if (result > 0) {
                return result;
            }

            int size = atoi(bodySize);
            request->BodySize = size;
            break;
        default:
            request->BodySize = 0;
            break;
    }
    return Success;
}

ResultCode GetHeaderValue(HTTPRequest* request, char* headerName, char* headerValue) {
    for (int ii = 0; ii < request->NHeaders; ii++) {
        if (strcmp(request->Header[ii].Name + '\0', headerName) == 0) {
            strncpy(headerValue, request->Header[ii].Value, MAX_HEADER_VALUE_LENGTH);
            return Success;
        }
    }
    return ItemNotFound;
}

ResultCode ParseBody(char* buffer, HTTPRequest* request) {
    strncpy(request->Body, buffer + 4, request->BodySize);
    return Success;
}