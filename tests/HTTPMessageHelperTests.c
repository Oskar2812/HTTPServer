#include <stdio.h>

#include "../src/HTTPMessageHelpers.c"

int GetHeaderValue_ValidSearch_ReturnsHeader() {
    HTTPRequest request = {0};

    request.Headers.FieldLines = malloc(3 * sizeof(FieldLine));
    request.Headers.Capacity = 3;
    
    request.Headers.FieldLines[0].FieldName.Content = "Host";
    request.Headers.FieldLines[0].FieldName.Count = 4;

    request.Headers.FieldLines[0].FieldValue.Content = "www.google.com";
    request.Headers.FieldLines[0].FieldValue.Count = 14;

    request.Headers.FieldLines[1].FieldName.Content = "Content-Type";
    request.Headers.FieldLines[1].FieldName.Count = 12;
    request.Headers.FieldLines[1].FieldValue.Content = "application/json";
    request.Headers.FieldLines[1].FieldValue.Count = 16;

    request.Headers.FieldLines[2].FieldName.Content = "Content-Length";
    request.Headers.FieldLines[2].FieldName.Count = 14;
    request.Headers.FieldLines[2].FieldValue.Content = "1234";
    request.Headers.FieldLines[2].FieldValue.Count = 4;

    request.Headers.Count = 3;

    FieldLine* header = GetHeaderValue(&request.Headers, "Content-Type", 12);

    if (header == NULL) {
        printf("Header not found\n");
        return -1;
    }
    if (strncmp("Content-Type", header->FieldName.Content, header->FieldName.Count) != 0) {
        printf("Incorrect header name\n");
        return -1;
    }
    if (strncmp("application/json", header->FieldValue.Content, header->FieldValue.Count) != 0) {
        printf("Incorrect header name\n");
        return -1;
    }

    free(request.Headers.FieldLines);

    return 0;
}

int GetHeaderValue_InvalidSearch_ReturnsHeader() {
    HTTPRequest request = {0};

    request.Headers.FieldLines = malloc(3 * sizeof(FieldLine));
    request.Headers.Capacity = 3;
    
    request.Headers.FieldLines[0].FieldName.Content = "Host";
    request.Headers.FieldLines[0].FieldName.Count = 4;

    request.Headers.FieldLines[0].FieldValue.Content = "www.google.com";
    request.Headers.FieldLines[0].FieldValue.Count = 14;

    request.Headers.FieldLines[1].FieldName.Content = "Content-Type";
    request.Headers.FieldLines[1].FieldName.Count = 12;
    request.Headers.FieldLines[1].FieldValue.Content = "application/json";
    request.Headers.FieldLines[1].FieldValue.Count = 16;

    request.Headers.FieldLines[2].FieldName.Content = "Content-Length";
    request.Headers.FieldLines[2].FieldName.Count = 14;
    request.Headers.FieldLines[2].FieldValue.Content = "1234";
    request.Headers.FieldLines[2].FieldValue.Count = 4;

    request.Headers.Count = 3;

    FieldLine* header = GetHeaderValue(&request.Headers, "haha", 12);

    if (header != NULL) {
        printf("Header found\n");
        return -1;
    }

    free(request.Headers.FieldLines);

    return 0;
}

int AddHeader_ValidHeader_Success() {
    HTTPResponse response = {0};

    StringView headerName = {
        .Content = "Host",
        .Count = 4,
    };
    StringView headerValue = {
        .Content = "www.google.com",
        .Count = 4,
    };

    FieldLine header = {
        .FieldName = headerName,
        .FieldValue = headerValue,
    };

    if (AddHeader(&response.Headers, header) == -1) {
        printf("Failed to add header\n");
        return -1;
    }
    else if (response.Headers.Capacity != 4) {
        printf("Incorrect header capacity\n");
        return -1;
    }
    else if (response.Headers.Count != 1) {
        printf("Incorrect header count\n");
        return -1;
    }
    else if (strncmp("Host", response.Headers.FieldLines[0].FieldName.Content, response.Headers.FieldLines[0].FieldName.Count) != 0) {
        printf("Incorrect header name\n");
        return -1;
    }
    else if (strncmp("www.google.com", response.Headers.FieldLines[0].FieldValue.Content, response.Headers.FieldLines[0].FieldValue.Count) != 0) {
        printf("Incorrect header value\n");
        return -1;
    }

    return 0;
}


