#include "../include/Server.h"

ResultCode ReadIncomingData(FILE* file, char* buffer, int count) {
   
    size_t nread = fread(buffer, 1, count, file);
    buffer[nread] = '\0';

    return Success;
}