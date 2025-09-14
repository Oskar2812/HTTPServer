#include "../include/Server.h"

ResultCode ReadIncomingData(FILE* file, char* buffer, int bufferSize) {
    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (filesize > bufferSize) {
        return InsufficientMemory;
    }

    size_t nread = fread(buffer, 1, filesize, file);
    buffer[nread] = '\0';

    return Success;
}