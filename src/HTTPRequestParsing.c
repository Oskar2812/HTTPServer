#include "../include/libOskServer.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    StringView* Tokens;
    size_t Count;
    size_t Capacity;
} TokenList;

/// @brief Gets a single line form the HTTP request (i.e. until a CLRF)
/// @param result the resulting token (start of buffer + count until end of line)
/// @param buffer the text buffer to read from
/// @param bufferCount  size of buffer
/// @return offset to start of next line on success, -1 on failure
int GetLine(StringView* result, char* buffer, size_t bufferCount) {
    for (size_t i = 0; i < bufferCount; i++) {
        if (buffer[i] == '\r') {
            if (i + 1 < bufferCount && buffer[i + 1] == '\n') {
                result->Content = buffer;
                result->Count = i;

                return result->Count + 2;
            }
        }
    }

    return -1;
}

/// @brief Gets a single token from a line of a HTTP request (i.e. until a a SP or CRLF)
/// @param result the resulting token (start of buffer + count until SP or CRLF)
/// @param buffer the text buffer to read from
/// @param bufferCount  size of buffer
/// @return offset to next token, 0 if end of line has been reached, -1 on failure
int GetToken(StringView* result, char* buffer, size_t bufferCount) {
    if (buffer[0] == '\r' || buffer[0] == '\n') {
        return 0;
    }

    for (size_t i = 0; i < bufferCount; i++) {
        if (buffer[i] == ' ') {
            result->Content = buffer;
            result->Count = i;
            
            return result->Count + 1;
        } else  if (buffer[i] == '\r') {
            if (i + 1 < bufferCount && buffer[i + 1] == '\n') {
                result->Content = buffer;
                result->Count = i;

                return 0;
            }
        }
    }

    return -1;
}

void AddToken(TokenList* list, StringView token) {
    if (list->Count == list->Capacity) {
        size_t newCap = (list->Capacity == 0) ? 4 : list->Capacity * 2;
        list->Tokens = realloc(list->Tokens, newCap * sizeof(StringView));
        list->Capacity = newCap;
    }

    list->Tokens[list->Count++] = token;
}

/// @brief Given a line creates an array of all the tokens on that line. (ALLOCATES MEMORY)
/// @param result the resulting list of tokens
/// @param buffer the text buffer to resd from
/// @param bufferCount the size of the buffer
/// @return the size of the token list, or -1 on failure
int GetTokenList(TokenList* result, char* buffer, size_t bufferCount) {
    for (size_t i = 0; i < bufferCount;) {
        StringView token;
        
        int size = GetToken(&token, buffer + i, bufferCount - i);
        if (size == -1) {
            return -1;
        } else if (size == 0) {
            if (token.Count > 0) {
                AddToken(result, token);
            }
            return result->Count;
        }
        AddToken(result, token);

        i += size;
    } 

    return result->Count;
}

HTTPMethod ParseHTTPMethod(StringView method) {
    if (method.Count == 3 && strncmp("GET", method.Content, 3) == 0) {
        return GET;
    }
    else if (method.Count == 3 && strncmp("PUT", method.Content, 3) == 0)
    {
        return PUT;
    }
    else if (method.Count == 4 && strncmp("POST", method.Content, 4) == 0) {
        return POST;
    }
    else if (method.Count == 6 && strncmp("DELETE", method.Content, 6) == 0) {
        return DELETE;
    }
    else {
        return NO_METHOD;
    }
}

HTTPVersion ParseHTTPVersion(StringView version) {
    if (version.Count == 8 && strncmp("HTTP/1.1", version.Content, 8) == 0) {
        return V_ONE;
    }
    else if (version.Count == 6 && strncmp("HTTP/2", version.Content, 6) == 0) {
        return V_TWO;
    }
    else if (version.Count == 6 && strncmp("HTTP/3", version.Content, 6) == 0) {
        return V_THREE;
    }
    else {
        return NO_VERSION;
    }
}

/// @brief Parses a line of the request into the RequestLine struct
/// @param requestLine the RequestLine to populate
/// @param buffer the text buffer to read from 
/// @param bufferCount size of the buffer
/// @return 0 on success, -1 on failure
int ParseRequestLine(RequestLine* requestLine, char* buffer, size_t bufferCount) {
    
    // Tokenise the line
    TokenList tokenList = {0};
    if (GetTokenList(&tokenList, buffer, bufferCount) != 3) {
        return -1;
    }

    // Parse into HTTPRequest struct
    HTTPMethod method = ParseHTTPMethod(tokenList.Tokens[0]);
    if (method == NO_METHOD) {
        return -1;
    }
    requestLine->Method = method;

    requestLine->Target = tokenList.Tokens[1];

    HTTPVersion version = ParseHTTPVersion(tokenList.Tokens[2]);
    if (version == NO_VERSION) {
        return -1;
    }
    requestLine->Version = version;

    free(tokenList.Tokens);

    return 0;
}

bool IsInvalidFieldNameCharacter(char character) {
    if (character == ' ' || character == '\r' || character == '\n') {
        return true;
    }

    return false;
}

/// @brief Given a line gets the field name (i.e. the string before the :)
/// @param fieldName The result to read into
/// @param buffer the buffer to from 
/// @param bufferCount the size of the buffer
/// @return the offset to the start of the field value (accounting for an optional whitespace) or -1 on failure
int GetFieldName(StringView* fieldName, char* buffer, size_t bufferCount) {
    if (bufferCount == 0 || buffer[0] == ':' || IsInvalidFieldNameCharacter(buffer[0])) {
        return -1;
    }

    for (size_t i = 0; i < bufferCount; i++) {
        if (IsInvalidFieldNameCharacter(buffer[i])) {
            return -1;
        }
        if (buffer[i] == ':') {
            fieldName->Content = buffer;
            fieldName->Count = i;

            if (i + 1 < bufferCount && buffer[i + 1] == ' ') {
                return i + 2;
            }
            else return i + 1;
        }

    }

    return -1;
}

/// @brief Gets the field value given a string (goes until \r\n and gets rid of trailing whitespece)
/// @param fieldValue fieldValue to read into 
/// @param buffer buffer to read from
/// @param bufferCount size of buffer
/// @return 0 on succes, -1 on failure
int GetFieldValue(StringView* fieldValue, char* buffer, size_t bufferCount) {
    if (bufferCount == 0 || IsInvalidFieldNameCharacter(buffer[0])) {
        return -1;
    }

    if (GetLine(fieldValue, buffer, bufferCount) == -1) {
        return -1;
    }

    size_t newCount = fieldValue->Count;
    for (size_t i = fieldValue->Count - 1; i >= 0; i--) {
        if (fieldValue->Content[i] == ' ') {
            newCount--;
        }
        else {
            break;
        }
    }
    fieldValue->Count = newCount;

    return 0;
}

/// @brief Parse a field line
/// @param fieldLine the fieldLine to read into 
/// @param buffer the buffer to read from
/// @param bufferCount the size of the buffer
/// @return 0 on success, -1 on failure
int ParseFieldLine(FieldLine* fieldLine, char* buffer, size_t bufferCount) {
    StringView fieldName = {0};
    StringView fieldValue = {0};

    int fieldValueOffset = GetFieldName(&fieldName, buffer, bufferCount);
    if (fieldValueOffset == -1) {
        return -1;
    }

    if (GetFieldValue(&fieldValue, buffer + fieldValueOffset, bufferCount) == -1)
    {
        return -1;
    }

    fieldLine->FieldName = fieldName;
    fieldLine->FieldValue = fieldValue;

    return 0;
}

void AddHeader(RequestHeaders* list, FieldLine token) {
    if (list->Count == list->Capacity) {
        size_t newCap = (list->Capacity == 0) ? 4 : list->Capacity * 2;
        list->FieldLines = realloc(list->FieldLines, newCap * sizeof(FieldLine));
        list->Capacity = newCap;
    }

    list->FieldLines[list->Count++] = token;
}

/// @brief Parses all headers into a dynamic array (AALOCATES MEMORY)
/// @param headers The array to parse into
/// @param buffer the buffer ot read from
/// @param bufferCount the size of the buffer
/// @return size of array on succes, -1 on failure
int ParseHeaders(RequestHeaders* headers, char* buffer, size_t bufferCount) {
    int globalOffset = 0;

    for (size_t i = 0; i < MAX_HEADERS; i++) {
        StringView line = {0};
        FieldLine fieldLine = {0};

        int nextLineOffset = GetLine(&line, buffer + globalOffset, bufferCount - globalOffset);

        if (nextLineOffset == -1) {
            return -1;
        }
        else if (nextLineOffset == 2) {
            return headers->Count;
        }

        if (ParseFieldLine(&fieldLine, line.Content, line.Count) == -1) {
            return -1;
        }    

        AddHeader(headers, fieldLine);
        globalOffset += nextLineOffset;
    }

    return -1;
}

