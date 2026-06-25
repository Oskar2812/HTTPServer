#include <stdio.h>

#include "../src/HTTPRequestParsing.c"

int ParseRequestLine_ValidRequestString_GET_ParseSuccesful() {
    printf("Running ParseRequestLine_ValidRequestString_GET_ParseSuccesful\n");

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

    printf("Test complete\n");
    return 0;
}

int ParseRequestLine_ValidRequestString_PUT_ParseSuccesful() {
    printf("Running ParseRequestLine_ValidRequestString_PUT_ParseSuccesful\n");

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

    printf("Test complete\n");
    return 0;
}

int ParseRequestLine_ValidRequestString_POST_ParseSuccesful() {
    printf("Running ParseRequestLine_ValidRequestString_POST_ParseSuccesful\n");

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

    printf("Test complete\n");
    return 0;
}

int ParseRequestLine_ValidRequestString_DELETE_ParseSuccesful() {
    printf("Running ParseRequestLine_ValidRequestString_DELETE_ParseSuccesful\n");

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

    printf("Test complete\n");
    return 0;
}

int ParseRequestLine_ValidRequestString_v2_ParseSuccesful() {
    printf("Running ParseRequestLine_ValidRequestString_V2_ParseSuccesful\n");

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

    printf("Test complete\n");
    return 0;
}

int ParseRequestLine_ValidRequestString_v3_ParseSuccesful() {
    printf("Running ParseRequestLine_ValidRequestString_V3_ParseSuccesful\n");

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

    printf("Test complete\n");
    return 0;
}

int ParseRequestLine_InvalidRequestString_Method_ParseUnsuccesful() {
    printf("Running ParseRequestLine_ValidRequestString_V3_ParseSuccesful\n");

    char* request = "GETtyoy /pub/WWW/TheProject.html HTTP/3\r\n";

    RequestLine requestLine = {0};

    if (ParseRequestLine(&requestLine, request, 39) != -1) {
        printf("Invalid method not caught\n");
        return -1;
    }

    printf("Test complete\n");
    return 0;
}

int ParseRequestLine_InvalidRequestString_Version_ParseUnsuccesful() {
    printf("Running ParseRequestLine_ValidRequestString_V3_ParseSuccesful\n");

    char* request = "GETtyoy /pub/WWW/TheProject.html v7.2\r\n";

    RequestLine requestLine = {0};

    if (ParseRequestLine(&requestLine, request, 39) != -1) {
        printf("Invalid version not caught\n");
        return -1;
    }

    printf("Test complete\n");
    return 0;
}