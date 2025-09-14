#ifndef RESULTCODES_H
#define RESULTCODES_H

#define MAX_ERROR_MESSAGE_LENGTH 512

typedef enum {
    Success = 0,
    Error,
    InsufficientMemory,
    InvalidHttpMethod,
    InvalidHttpVersion,
    InvalidRequestLine,
    HeadersTooLong,
    ItemNotFound
} ResultCode;

void GetErrorMessage(ResultCode resultCode, char* buffer);
void PrintErrorMessage(ResultCode resultCode);

#endif