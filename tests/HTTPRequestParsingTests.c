#include <stdio.h>

#include "../src/HTTPRequestParsing.c"

int ParseRequestLine_ValidRequestString_GET_ParseSuccesful() {
    char* request = "GET /pub/WWW/TheProject.html HTTP/1.1\r\n";

    RequestLine requestLine = {0};

    if (ParseRequestLine(&requestLine, request, 39) == 0) {
        if (requestLine.Method != GET) {
            printf("Method not parsed correctly\n");
            return -1;
        }
        if (strncmp("/pub/WWW/TheProject.html", requestLine.Target.Target.Content, requestLine.Target.Target.Count) != 0) {
            printf ("Target not parsed correctly\n");
            return -1;
        }
        if (requestLine.Version != V_ONE) {
            printf("Version not parsed correctly\n");
            return -1;
        }
    }
    else {
        printf("Request line parsing failed\n");
        return -1;
    }

    return 0;
}

int ParseRequestLine_ValidRequestString_PUT_ParseSuccesful() {
    char* request = "PUT /pub/WWW/TheProject.html HTTP/1.1\r\n";

    RequestLine requestLine = {0};

    if (ParseRequestLine(&requestLine, request, 39) == 0) {
        if (requestLine.Method != PUT) {
            printf("Method not parsed correctly\n");
            return -1;
        }
        if (strncmp("/pub/WWW/TheProject.html", requestLine.Target.Target.Content, requestLine.Target.Target.Count) != 0) {
            printf ("Target not parsed correctly\n");
            return -1;
        }
        if (requestLine.Version != V_ONE) {
            printf("Version not parsed correctly\n");
            return -1;
        }
    }
    else {
        printf("Request line parsing failed\n");
        return -1;
    }

    return 0;
}

int ParseRequestLine_ValidRequestString_POST_ParseSuccesful() {
    char* request = "POST /pub/WWW/TheProject.html HTTP/1.1\r\n";

    RequestLine requestLine = {0};

    if (ParseRequestLine(&requestLine, request, 40) == 0) {
        if (requestLine.Method != POST) {
            printf("Method not parsed correctly\n");
            return -1;
        }
        if (strncmp("/pub/WWW/TheProject.html", requestLine.Target.Target.Content, requestLine.Target.Target.Count) != 0) {
            printf ("Target not parsed correctly\n");
            return -1;
        }
        if (requestLine.Version != V_ONE) {
            printf("Version not parsed correctly\n");
            return -1;
        }
    }
    else {
        printf("Request line parsing failed\n");
        return -1;
    }

    return 0;
}

int ParseRequestLine_ValidRequestString_DELETE_ParseSuccesful() {
    char* request = "DELETE /pub/WWW/TheProject.html HTTP/1.1\r\n";

    RequestLine requestLine = {0};

    if (ParseRequestLine(&requestLine, request, 42) == 0) {
        if (requestLine.Method != DELETE_METHOD) {
            printf("Method not parsed correctly\n");
            return -1;
        }
        if (strncmp("/pub/WWW/TheProject.html", requestLine.Target.Target.Content, requestLine.Target.Target.Count) != 0) {
            printf ("Target not parsed correctly\n");
            return -1;
        }
        if (requestLine.Version != V_ONE) {
            printf("Version not parsed correctly\n");
            return -1;
        }
    }
    else {
        printf("Request line parsing failed\n");
        return -1;
    }

    return 0;
}

int ParseRequestLine_ValidRequestString_v2_ParseSuccesful() {
    char* request = "GET /pub/WWW/TheProject.html HTTP/2\r\n";

    RequestLine requestLine = {0};

    if (ParseRequestLine(&requestLine, request, 37) == 0) {
        if (requestLine.Method != GET) {
            printf("Method not parsed correctly\n");
            return -1;
        }
        if (strncmp("/pub/WWW/TheProject.html", requestLine.Target.Target.Content, requestLine.Target.Target.Count) != 0) {
            printf ("Target not parsed correctly\n");
            return -1;
        }
        if (requestLine.Version != V_TWO) {
            printf("Version not parsed correctly\n");
            return -1;
        }
    }
    else {
        printf("Request line parsing failed\n");
        return -1;
    }

    return 0;
}

int ParseRequestLine_ValidRequestString_v3_ParseSuccesful() {
    char* request = "GET /pub/WWW/TheProject.html HTTP/3\r\n";

    RequestLine requestLine = {0};

    if (ParseRequestLine(&requestLine, request, 37) == 0) {
        if (requestLine.Method != GET) {
            printf("Method not parsed correctly\n");
            return -1;
        }
        if (strncmp("/pub/WWW/TheProject.html", requestLine.Target.Target.Content, requestLine.Target.Target.Count) != 0) {
            printf ("Target not parsed correctly\n");
            return -1;
        }
        if (requestLine.Version != V_THREE) {
            printf("Version not parsed correctly\n");
            return -1;
        }
    }
    else {
        printf("Request line parsing failed\n");
        return -1;
    }

    return 0;
}

int ParseRequestLine_InvalidRequestString_Method_ParseUnsuccesful() {
    char* request = "GETtyoy /pub/WWW/TheProject.html HTTP/3\r\n";

    RequestLine requestLine = {0};

    if (ParseRequestLine(&requestLine, request, 37) != -1) {
        printf("Invalid method not caught\n");
        return -1;
    }

    return 0;
}

int ParseRequestLine_InvalidRequestString_Version_ParseUnsuccesful() {
    char* request = "GETtyoy /pub/WWW/TheProject.html v7.2\r\n";

    RequestLine requestLine = {0};

    if (ParseRequestLine(&requestLine, request, 37) != -1) {
        printf("Invalid version not caught\n");
        return -1;
    }

    return 0;
}

int ParseRequestLine_QueryParameters_ParseSuccesful() {
    char* request = "GET /pub/WWW/TheProject.html?test=haha&test1=hoho HTTP/3\r\n";

    RequestLine requestLine = {0};

    if (ParseRequestLine(&requestLine, request, 58) == 0) {
        if (requestLine.Method != GET) {
            printf("Method not parsed correctly\n");
            return -1;
        }
        if (strncmp("/pub/WWW/TheProject.html", requestLine.Target.Target.Content, requestLine.Target.Target.Count) != 0) {
            printf ("Target not parsed correctly\n");
            return -1;
        }
        if (requestLine.Target.QueryCount != 2) {
            printf("Incorrect number of query params read\n");
            return -1;
        }
        if (strncmp("test", requestLine.Target.QueryParameters[0].QueryName.Content, requestLine.Target.QueryParameters[0].QueryName.Count) != 0
            || strncmp("haha", requestLine.Target.QueryParameters[0].QueryValue.Content, requestLine.Target.QueryParameters[0].QueryValue.Count) != 0 ) {

            printf("First query param parsed incorrectly\n");
            return -1;
        }
        if (strncmp("test1", requestLine.Target.QueryParameters[1].QueryName.Content, requestLine.Target.QueryParameters[1].QueryName.Count) != 0
            || strncmp("hoho", requestLine.Target.QueryParameters[1].QueryValue.Content, requestLine.Target.QueryParameters[1].QueryValue.Count) != 0 ) {

            printf("Second query param parsed incorrectly\n");
            return -1;
        }
        if (requestLine.Version != V_THREE) {
            printf("Version not parsed correctly\n");
            return -1;
        }
    }
    else {
        printf("Request line parsing failed\n");
        return -1;
    }

    return 0;
}

int ParseFieldLine_ValidFieldLine_WithBlank_ParseSuccesful() {
    char* header = "Header1: with blank\r\n";

    FieldLine fieldLine = {0};

    if (ParseFieldLine(&fieldLine, header, 21) == -1) {
        printf("Parsing field line failed");
        return -1;
    }

    if (strncmp("Header1", fieldLine.FieldName.Content, fieldLine.FieldName.Count) != 0) {
        printf("Incorrect header name");
        return -1;
    }
    if (strncmp("with blank", fieldLine.FieldValue.Content, fieldLine.FieldValue.Count) != 0) {
        printf("Incorrect header value");
        return -1;
    }

    return 0;
}

int ParseFieldLine_ValidFieldLine_WithoutBlank_ParseSuccesful() {
    char* header = "Header1:without blank\r\n";

    FieldLine fieldLine = {0};

    if (ParseFieldLine(&fieldLine, header, 23) == -1) {
        printf("Parsing field line failed");
        return -1;
    }

    if (strncmp("Header1", fieldLine.FieldName.Content, fieldLine.FieldName.Count) != 0) {
        printf("Incorrect header name");
        return -1;
    }
    if (strncmp("without blank", fieldLine.FieldValue.Content, fieldLine.FieldValue.Count) != 0) {
        printf("Incorrect header value");
        return -1;
    }

    return 0;
}

int ParseFieldLine_ValidFieldLine_TrailingWhitespace_ParseSuccesful() {
    char* header = "Header1:without blank    \r\n";

    FieldLine fieldLine = {0};

    if (ParseFieldLine(&fieldLine, header, 27) == -1) {
        printf("Parsing field line failed");
        return -1;
    }

    if (strncmp("Header1", fieldLine.FieldName.Content, fieldLine.FieldName.Count) != 0) {
        printf("Incorrect header name");
        return -1;
    }
    if (strncmp("without blank", fieldLine.FieldValue.Content, fieldLine.FieldValue.Count) != 0) {
        printf("Incorrect header value");
        return -1;
    }

    return 0;
}

int ParseFieldLine_ValidFieldLine_NoColon_ParseUnsuccesful() {
    char* header = "Header1without blank\r\n";

    FieldLine fieldLine = {0};

    if (ParseFieldLine(&fieldLine, header, 23) != -1) {
        printf("Failed to catch invalid header");
        return -1;
    }

    return 0;
}

int ParseMessagePreamble_ValidPreamble_ParseSuccesful() {
    char *requestBuffer =
        "GET /index.html HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: test-client\r\n"
        "Accept: */*\r\n"
        "\r\n";

    HTTPRequest request = {0};
    if (ParseMessagePreamble(&request, requestBuffer, 93) == -1) {
        printf("Message preample parsing failed\n");
        return -1;
    }

    if (request.RequestLine.Method != GET) {
        printf("Incorrect method\n");
        return -1;
    }
    if (strncmp("/index.html", request.RequestLine.Target.Target.Content, request.RequestLine.Target.Target.Count) != 0) {
        printf("Incorrect target\n");
        return -1;
    }
    if (request.RequestLine.Version != V_ONE) {
        printf("Incorrect version\n");
        return -1;
    }

    if (request.Headers.Count != 3) {
        printf("Incorrect number of headers\n");
        return -1;
    }
    if(strncmp("Host", request.Headers.FieldLines[0].FieldName.Content, request.Headers.FieldLines[0].FieldName.Count) != 0) {
        printf("Incorrect irst header name\n");
        return -1;
    }
    if(strncmp("example.com", request.Headers.FieldLines[0].FieldValue.Content, request.Headers.FieldLines[0].FieldValue.Count) != 0) {
        printf("Incorrect irst header value\n");
        return -1;
    }
    if(strncmp("User-Agent", request.Headers.FieldLines[1].FieldName.Content, request.Headers.FieldLines[1].FieldName.Count) != 0) {
        printf("Incorrect second header name\n");
        return -1;
    }
    if(strncmp("test-client", request.Headers.FieldLines[1].FieldValue.Content, request.Headers.FieldLines[1].FieldValue.Count) != 0) {
        printf("Incorrect second header value\n");
        return -1;
    }
    if(strncmp("Accept", request.Headers.FieldLines[2].FieldName.Content, request.Headers.FieldLines[2].FieldName.Count) != 0) {
        printf("Incorrect third header name\n");
        return -1;
    }
    if(strncmp("*/*", request.Headers.FieldLines[2].FieldValue.Content, request.Headers.FieldLines[2].FieldValue.Count) != 0) {
        printf("Incorrect third header value\n");
        return -1;
    }

    free(request.Headers.FieldLines);

    return 0;
}