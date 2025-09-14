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
        char errorMessage[MAX_ERROR_MESSAGE_LENGTH];
        GetErrorMessage(result, errorMessage);
        printf("%s", errorMessage);
        return 1;
    }

    printf("%s", buffer);

    return 0;
}