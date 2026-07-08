#include "../include/libOskServer.h"
#include "../include/Internal_HTTPListener.h"
#include "../include/Internal_HTTPResponseSerialisation.h"
#include "../include/Internal_UtilityMacros.h"

#define CHAR_SIZE sizeof(char)

int WriteToBuffer(TextBuffer* buffer, char* text, size_t textSize) {
    ASSERT_SUCCESS(AllocateBufferMemory(buffer, textSize * CHAR_SIZE));

    memcpy(buffer->Content + buffer->Count, text, textSize);
    buffer->Count += textSize;

    return buffer->Count;
}
#define INSERT_WS(buffer) \
    WriteToBuffer((buffer), " ", 1);

#define INSERT_CRLF(buffer) \
    WriteToBuffer((buffer), "\r\n", 2);

/// @brief Serialises the HTTP version
/// @param result the resulting text
/// @param version the version
/// @return size of buffer on success, -1 on failure
int SerialiseHTTPVersion(TextBuffer* result, HTTPVersion version) {
    switch(version) {
        case V_ONE:
            WriteToBuffer(result, "HTTP/1.1", 8);
            return result->Count;
        case V_TWO:
            WriteToBuffer(result, "HTTP/2", 6);
            return result->Count;
        case V_THREE:
            WriteToBuffer(result, "HTTP/3", 6);
            return result->Count;
        case NO_VERSION:
        default:
            return -1;
    }
}

int SerialiseHTTPStatus(TextBuffer* result, HTTPStatus status) {
    switch (status) {
        case OK:
            WriteToBuffer(result, "200 OK", 6);
            return result->Count;
        case NOT_FOUND:
            WriteToBuffer(result, "404 Not Found", 13);
            return result->Count;
        case BAD_REQUEST:
            WriteToBuffer(result, "400 Bad Request", 15);
            return result->Count;
        case INTERNAL_SERVER_ERROR:
            WriteToBuffer(result, "500 Internal Server Error", 25);
            return result->Count;
        default:
            return -1;
    }
}

/// @brief Serialises the status line
/// @param result the resulting text
/// @param statusLine the struct to serialise
/// @return size of buffer on success, -1 on failure
int SerialiseStatusLine(TextBuffer* result, StatusLine* statusLine) {
    ASSERT_SUCCESS(SerialiseHTTPVersion(result, statusLine->Version));
    INSERT_WS(result);
    ASSERT_SUCCESS(SerialiseHTTPStatus(result, statusLine->Status));
    INSERT_CRLF(result);

    return result->Count;
}

/// @brief Serialises a field line
/// @param result  the resulting text
/// @param fieldLine the field line
/// @return size of the uffer on success, -1 otherwise
int SerialiseFieldLine(TextBuffer* result, FieldLine* fieldLine) {
    WriteToBuffer(result, fieldLine->FieldName.Content, fieldLine->FieldName.Count);
    WriteToBuffer(result, ": ", 2);
    WriteToBuffer(result, fieldLine->FieldValue.Content, fieldLine->FieldValue.Count);

    return result->Count;
} 

/// @brief Seialsies all the headers 
/// @param result the resulting text
/// @param headers the headers to serialise
/// @return size pf the buffer on success, -1 otherwise
int SerialiseHeaders(TextBuffer* result, MessageHeaders* headers) {
    for (size_t i = 0; i < headers->Count; i++) {
        ASSERT_SUCCESS(SerialiseFieldLine(result, &headers->FieldLines[i]));
        INSERT_CRLF(result);
    }

    return result->Count;
}

int SerialiseBody(TextBuffer* result, StringView body) {
    if (body.Count == 0) {
        return result->Count;
    }
    WriteToBuffer(result, body.Content, body.Count);

    return result->Count;
}

int SerialiseResponse(TextBuffer* result, HTTPResponse* response) {
    ASSERT_SUCCESS(SerialiseStatusLine(result, &response->Status));

    ASSERT_SUCCESS(SerialiseHeaders(result, &response->Headers));

    INSERT_CRLF(result);

    ASSERT_SUCCESS(SerialiseBody(result, response->Body));

    return 0;
}