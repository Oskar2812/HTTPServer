#include <string.h>

#include "../include/HTTPRequest.h"

void PrepareCompareString(char* compareString, char* methodString, int* offset, int n) {
    strncpy(compareString, methodString, n);
    compareString[n] = '\0';
    *offset = n + 1;
}

ResultCode ParseHTTPPath(char* buffer, HTTPRequest* request, int* offset) {
    int position = 0;
    while (1) {
        char next;
        next = buffer[position];
        if (next == '\n' || next == ' ') {
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
        *offset = 9;
        return Success;
    }
    if (strncmp(buffer, "HTTP/2", 6) == 0) {
        request->RequestLine.Version = V2;
        *offset = 7;
        return Success;
    }
    if (strncmp(buffer, "HTTP/3", 6) == 0) {
        request->RequestLine.Version = V3;
        *offset = 7;
        return Success;
    }

    return InvalidHttpVersion;
}

ResultCode ParseHTTPHeaders(char* buffer, HTTPRequest* request) {
    int offset = 0;
    int result = ParseHTTPMethod(buffer, request, &offset);
    if (result > 0) {
        return result;
    }
    buffer += offset;

    result = ParseHTTPPath(buffer, request, &offset);
    if (result > 0) {
        return result;
    }
    buffer += offset;

    result = ParseHTTPVersion(buffer, request, &offset);
    if (result > 0) {
        return result;
    }
    buffer += offset;

    return Success;
}