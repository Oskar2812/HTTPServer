#include "../include/libOskServer.h"
#include <stdlib.h>

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