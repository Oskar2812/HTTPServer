#include <stdio.h>

#include "../src/HTTPResponseSerialisation.c"
#include "../src/HTTPListener.c"

int SerialiseHTTPVersion_AllVersions_SerialisedCorrectly() {
    TextBuffer buffer = {0};

    if (SerialiseHTTPVersion(&buffer, V_ONE) == -1) {
        printf("Failed to serialise HTTP/1.1\n");
        free(buffer.Content);
        return -1;
    }
    if (buffer.Count != 8 ||
        memcmp(buffer.Content, "HTTP/1.1", 8) != 0) {
        printf("HTTP/1.1 serialised incorrectly\n");
        free(buffer.Content);
        return -1;
    }

    // HTTP/2
    free(buffer.Content);
    buffer = (TextBuffer){0};

    if (SerialiseHTTPVersion(&buffer, V_TWO) == -1) {
        printf("Failed to serialise HTTP/2\n");
        free(buffer.Content);
        return -1;
    }

    if (buffer.Count != 6 ||
        memcmp(buffer.Content, "HTTP/2", 6) != 0) {
        printf("HTTP/2 serialised incorrectly\n");
        free(buffer.Content);
        return -1;
    }

    // HTTP/3
    free(buffer.Content);
    buffer = (TextBuffer){0};

    if (SerialiseHTTPVersion(&buffer, V_THREE) == -1) {
        printf("Failed to serialise HTTP/3\n");
        free(buffer.Content);
        return -1;
    }

    if (buffer.Count != 6 ||
        memcmp(buffer.Content, "HTTP/3", 6) != 0) {
        printf("HTTP/3 serialised incorrectly\n");
        free(buffer.Content);
        return -1;
    }

    free(buffer.Content);
    return 0;
}

int SerialiseHTTPVersion_InvalidVersion_SerialisationFails() {
    TextBuffer buffer = {0};

    if (SerialiseHTTPVersion(&buffer, NO_VERSION) != -1) {
        printf("Serialisation failed to catch invalid version\n");
        free(buffer.Content);
        return -1;
    }
    
    free(buffer.Content);
    return 0;
}

int SerialiseHTTPStatus_AllStatuses_SerialisedCorrectly() {
    TextBuffer buffer = {0};

    // 200 OK
    if (SerialiseHTTPStatus(&buffer, OK) == -1) {
        printf("Failed to serialise 200 OK\n");
        free(buffer.Content);
        return -1;
    }

    if (buffer.Count != 6 ||
        memcmp(buffer.Content, "200 OK", 6) != 0) {
        printf("200 OK serialised incorrectly\n");
        free(buffer.Content);
        return -1;
    }

    // 404 Not Found
    free(buffer.Content);
    buffer = (TextBuffer){0};

    if (SerialiseHTTPStatus(&buffer, NOT_FOUND) == -1) {
        printf("Failed to serialise 404 Not Found\n");
        free(buffer.Content);
        return -1;
    }

    if (buffer.Count != 13 ||
        memcmp(buffer.Content, "404 Not Found", 13) != 0) {
        printf("404 Not Found serialised incorrectly\n");
        free(buffer.Content);
        return -1;
    }

    // 400 Bad Request
    free(buffer.Content);
    buffer = (TextBuffer){0};

    if (SerialiseHTTPStatus(&buffer, BAD_REQUEST) == -1) {
        printf("Failed to serialise 400 Bad Request\n");
        free(buffer.Content);
        return -1;
    }

    if (buffer.Count != 15 ||
        memcmp(buffer.Content, "400 Bad Request", 15) != 0) {
        printf("400 Bad Request serialised incorrectly\n");
        free(buffer.Content);
        return -1;
    }

    // 500 Internal Server Error
    free(buffer.Content);
    buffer = (TextBuffer){0};

    if (SerialiseHTTPStatus(&buffer, INTERNAL_SERVER_ERROR) == -1) {
        printf("Failed to serialise 500 Internal Server Error\n");
        free(buffer.Content);
        return -1;
    }

    if (buffer.Count != 25 ||
        memcmp(buffer.Content, "500 Internal Server Error", 25) != 0) {
        printf("500 Internal Server Error serialised incorrectly\n");
        free(buffer.Content);
        return -1;
    }

    free(buffer.Content);
    return 0;
}

int SerialiseStatusLine_ValidStatusLine_SerialisedCorrectly() {
    TextBuffer buffer = {0};

    StatusLine statusLine = {
        .Version = V_ONE,
        .Status = OK
    };

    if (SerialiseStatusLine(&buffer, &statusLine) == -1) {
        printf("Failed to serialise status line\n");
        free(buffer.Content);
        return -1;
    }

    if (buffer.Count != 17 ||
        memcmp(buffer.Content, "HTTP/1.1 200 OK\r\n", 17) != 0) {
        printf("Status line serialised incorrectly\n");
        free(buffer.Content);
        return -1;
    }

    free(buffer.Content);
    return 0;
}

int SerialiseFieldLine_ValidFieldLine_SerialisedCorrectly() {
    TextBuffer buffer = {0};

    FieldLine fieldLine = {0};

    fieldLine.FieldName.Content = "Content-Type";
    fieldLine.FieldName.Count = 12;

    fieldLine.FieldValue.Content = "application/json";
    fieldLine.FieldValue.Count = 16;

    if (SerialiseFieldLine(&buffer, &fieldLine) == -1) {
        printf("Failed to serialise field line\n");
        free(buffer.Content);
        return -1;
    }

    if (buffer.Count != 30 ||
        memcmp(buffer.Content,
               "Content-Type: application/json",
               30) != 0) {
        printf("Field line serialised incorrectly\n");
        free(buffer.Content);
        return -1;
    }

    free(buffer.Content);
    return 0;
}

int SerialiseHeaders_ValidHeaders_SerialisedCorrectly() {
    TextBuffer buffer = {0};

    MessageHeaders headers = {0};

    headers.Count = 2;
    headers.FieldLines = malloc(sizeof(FieldLine) * headers.Count);

    headers.FieldLines[0].FieldName.Content = "Content-Type";
    headers.FieldLines[0].FieldName.Count = 12;
    headers.FieldLines[0].FieldValue.Content = "application/json";
    headers.FieldLines[0].FieldValue.Count = 16;

    headers.FieldLines[1].FieldName.Content = "Content-Length";
    headers.FieldLines[1].FieldName.Count = 14;
    headers.FieldLines[1].FieldValue.Content = "123";
    headers.FieldLines[1].FieldValue.Count = 3;

    if (SerialiseHeaders(&buffer, &headers) == -1) {
        printf("Failed to serialise headers\n");
        free(buffer.Content);
        free(headers.FieldLines);
        return -1;
    }

    const char* expected =
        "Content-Type: application/json\r\n"
        "Content-Length: 123\r\n";

    if (buffer.Count != 53 ||
        memcmp(buffer.Content, expected, 53) != 0) {
        printf("Headers serialised incorrectly\n");
        free(buffer.Content);
        free(headers.FieldLines);
        return -1;
    }

    free(buffer.Content);
    free(headers.FieldLines);

    return 0;
}

int SerialiseBody_ValidBody_SerialisedCorrectly() {
    TextBuffer buffer = {0};

    StringView body = {
        .Content = "{\"message\":\"Hello\"}",
        .Count = 19
    };

    if (SerialiseBody(&buffer, body) == -1) {
        printf("Failed to serialise body\n");
        free(buffer.Content);
        return -1;
    }

    if (buffer.Count != 19 ||
        memcmp(buffer.Content, "{\"message\":\"Hello\"}", 19) != 0) {
        printf("Body serialised incorrectly\n");
        free(buffer.Content);
        return -1;
    }

    free(buffer.Content);
    return 0;
}

int SerialiseResponse_ValidResponse_SerialisedCorrectly() {
    TextBuffer buffer = {0};

    HTTPResponse response = {0};

    response.Status.Version = V_ONE;
    response.Status.Status = OK;

    response.Headers.Count = 2;
    response.Headers.FieldLines = malloc(2 * sizeof(FieldLine));

    response.Headers.FieldLines[0].FieldName.Content = "Content-Type";
    response.Headers.FieldLines[0].FieldName.Count = 12;
    response.Headers.FieldLines[0].FieldValue.Content = "text/plain";
    response.Headers.FieldLines[0].FieldValue.Count = 10;

    response.Headers.FieldLines[1].FieldName.Content = "Content-Length";
    response.Headers.FieldLines[1].FieldName.Count = 14;
    response.Headers.FieldLines[1].FieldValue.Content = "5";
    response.Headers.FieldLines[1].FieldValue.Count = 1;

    response.Body.Content = "Hello";
    response.Body.Count = 5;

    if (SerialiseResponse(&buffer, &response) == -1) {
        printf("Failed to serialise response\n");
        free(buffer.Content);
        free(response.Headers.FieldLines);
        return -1;
    }

    const char* expected =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 5\r\n"
        "\r\n"
        "Hello";

    if (buffer.Count != 69 ||
        memcmp(buffer.Content, expected, 69) != 0) {
        printf("Response serialised incorrectly\n");
        free(buffer.Content);
        free(response.Headers.FieldLines);
        return -1;
    }

    free(buffer.Content);
    free(response.Headers.FieldLines);

    return 0;
}

