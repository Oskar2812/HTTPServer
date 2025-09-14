#ifndef RESULTCODES_H
#define RESULTCODES_H

#define MAX_ERROR_MESSAGE_LENGTH 512

typedef enum {
    Success = 0,
    InsufficientMemory
} ResultCode;

void GetErrorMessage(ResultCode resultCode, char* buffer);

#endif