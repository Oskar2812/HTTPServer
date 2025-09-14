#include <stdio.h>

#include "../include/HTTPServer.h"

#define BUFFER_SIZE 1000

int main() {
    char buffer[BUFFER_SIZE];

    FILE* file = fopen("test.txt", "r");
    if (file == NULL) {
        printf("Unable to open file\n");
        return 1;
    }

    int result = ReadIncomingData(file, buffer, BUFFER_SIZE);
    if (result > 0) {
        PrintErrorMessage(result);
        return 1;
    }

    printf("%s", buffer);

    int headerSize;
    HTTPRequest request = {0};
    result = ParseHTTPHeaders(buffer, BUFFER_SIZE, &request, &headerSize);
    if (result > 0) {
        PrintErrorMessage(result);
        return 1;
    }

    GetBodySize(&request);
    if (request.BodySize == 0) {
        return 0;
    }

    if (headerSize + request.BodySize > BUFFER_SIZE) {
        ReadIncomingData(file, buffer + headerSize, headerSize + request.BodySize - BUFFER_SIZE);
    }

    ParseBody(buffer + headerSize, &request);

    return 0;
}