#include <stdio.h>

#include "../include/HTTPServer.h"

int main() {
    char buffer[1000];

    FILE* file = fopen("test.txt", "r");
    if (file == NULL) {
        printf("Unable to open file\n");
        return 1;
    }

    int result = ReadIncomingData(file, buffer, 1000);
    if (result > 0) {
        PrintErrorMessage(result);
        return 1;
    }

    printf("%s", buffer);

    HTTPRequest request = {0};
    result = ParseHTTPHeaders(buffer, &request);
    if (result > 0) {
        PrintErrorMessage(result);
        return 1;
    }

    return 0;
}