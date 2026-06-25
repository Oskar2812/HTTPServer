#include <stdio.h>

#include "../src/HTTPRequestParsing.c"

int ParseRequestLine_ValidRequestString_GET_ParseSuccesful() {
    char* request = "GET /pub/WWW/TheProject.html HTTP/1.1\r\n";

    RequestLine requestLine = {0};

    if (ParseRequestLine(&requestLine, request, 41) == 0) {
        if (requestLine.Method != GET) {
            printf("Method not parsed correctly\n");
            return -1;
        }
        if (strncmp("/pub/WWW/TheProject.html", requestLine.Target.Content, requestLine.Target.Count) != 0) {
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

    if (ParseRequestLine(&requestLine, request, 41) == 0) {
        if (requestLine.Method != PUT) {
            printf("Method not parsed correctly\n");
            return -1;
        }
        if (strncmp("/pub/WWW/TheProject.html", requestLine.Target.Content, requestLine.Target.Count) != 0) {
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

    if (ParseRequestLine(&requestLine, request, 42) == 0) {
        if (requestLine.Method != POST) {
            printf("Method not parsed correctly\n");
            return -1;
        }
        if (strncmp("/pub/WWW/TheProject.html", requestLine.Target.Content, requestLine.Target.Count) != 0) {
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

    if (ParseRequestLine(&requestLine, request, 44) == 0) {
        if (requestLine.Method != DELETE) {
            printf("Method not parsed correctly\n");
            return -1;
        }
        if (strncmp("/pub/WWW/TheProject.html", requestLine.Target.Content, requestLine.Target.Count) != 0) {
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

    if (ParseRequestLine(&requestLine, request, 39) == 0) {
        if (requestLine.Method != GET) {
            printf("Method not parsed correctly\n");
            return -1;
        }
        if (strncmp("/pub/WWW/TheProject.html", requestLine.Target.Content, requestLine.Target.Count) != 0) {
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

    if (ParseRequestLine(&requestLine, request, 39) == 0) {
        if (requestLine.Method != GET) {
            printf("Method not parsed correctly\n");
            return -1;
        }
        if (strncmp("/pub/WWW/TheProject.html", requestLine.Target.Content, requestLine.Target.Count) != 0) {
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

    if (ParseRequestLine(&requestLine, request, 39) != -1) {
        printf("Invalid method not caught\n");
        return -1;
    }

    return 0;
}

int ParseRequestLine_InvalidRequestString_Version_ParseUnsuccesful() {
    char* request = "GETtyoy /pub/WWW/TheProject.html v7.2\r\n";

    RequestLine requestLine = {0};

    if (ParseRequestLine(&requestLine, request, 39) != -1) {
        printf("Invalid version not caught\n");
        return -1;
    }

    return 0;
}

int ParseFieldLine_ValidFieldLine_WithBlank_ParseSuccesful() {
    char* header = "Header1: with blank\r\n";

    FieldLine fieldLine = {0};

    if (ParseFieldLine(&fieldLine, header, 23) == -1) {
        printf("Parsing field line failed");
    }

    if (strncmp("Header1", fieldLine.FieldName.Content, fieldLine.FieldName.Count) != 0) {
        printf("Incorrect header name");
    }
    if (strncmp("with blank", fieldLine.FieldValue.Content, fieldLine.FieldValue.Count) != 0) {
        printf("Incorrect header value");
    }

    return 0;
}

int ParseFieldLine_ValidFieldLine_WithoutBlank_ParseSuccesful() {
    char* header = "Header1:without blank\r\n";

    FieldLine fieldLine = {0};

    if (ParseFieldLine(&fieldLine, header, 25) == -1) {
        printf("Parsing field line failed");
    }

    if (strncmp("Header1", fieldLine.FieldName.Content, fieldLine.FieldName.Count) != 0) {
        printf("Incorrect header name");
    }
    if (strncmp("without blank", fieldLine.FieldValue.Content, fieldLine.FieldValue.Count) != 0) {
        printf("Incorrect header value");
    }

    return 0;
}

int ParseFieldLine_ValidFieldLine_TrailingWhitespace_ParseSuccesful() {
    char* header = "Header1:without blank    \r\n";

    FieldLine fieldLine = {0};

    if (ParseFieldLine(&fieldLine, header, 29) == -1) {
        printf("Parsing field line failed");
    }

    if (strncmp("Header1", fieldLine.FieldName.Content, fieldLine.FieldName.Count) != 0) {
        printf("Incorrect header name");
    }
    if (strncmp("without blank", fieldLine.FieldValue.Content, fieldLine.FieldValue.Count) != 0) {
        printf("Incorrect header value");
    }

    return 0;
}

int ParseFieldLine_ValidFieldLine_NoColon_ParseUnsuccesful() {
    char* header = "Header1without blank\r\n";

    FieldLine fieldLine = {0};

    if (ParseFieldLine(&fieldLine, header, 25) != -1) {
        printf("Failed to catch invalid header");
    }

    return 0;
}