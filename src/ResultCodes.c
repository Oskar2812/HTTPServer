#include <stdio.h>
#include <string.h>

#include "../include/ResultCodes.h"

void GetErrorMessage(ResultCode resultCode, char* buffer) {
    switch (resultCode) {
        case Success:
            strcpy(buffer, "No error");
            break;
        case Error:
            strcpy(buffer, "Something went wrong");
            break;
        case InsufficientMemory:
            strcpy(buffer, "Supplied buffer has insufficient memory");
            break;
        case InvalidHttpMethod:
            strcpy(buffer, "Http method is not recognised");
            break;
        case InvalidHttpVersion:
            strcpy(buffer, "Http version is not recognised");
            break;
        case InvalidRequestLine:
            strcpy(buffer, "Request line is malformed");
            break;
        case HeadersTooLong:
            strcpy(buffer, "Http headers section is too long");
            break;
        case ItemNotFound:
            strcpy(buffer, "Could not find matching value");
            break;
        default:
            strcpy(buffer, "Result code not recognised");
            break;
    }
}

void PrintErrorMessage(ResultCode resultCode) {
    char errorMessage[MAX_ERROR_MESSAGE_LENGTH];
        GetErrorMessage(resultCode, errorMessage);
        printf("%s\n", errorMessage);
}