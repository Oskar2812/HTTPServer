#include "../include/ResultCodes.h"

void GetErrorMessage(ResultCode resultCode, char* buffer) {
    switch (resultCode) {
        case Success:
            buffer = "No error\0";
            break;
        case InsufficientMemory:
            buffer = "Supplied buffer has insufficient memory\0";
            break;
        default:
            buffer = "Result code not recognised\0";
            break;
    }
}