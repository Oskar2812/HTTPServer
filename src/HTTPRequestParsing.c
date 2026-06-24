#include "../include/libOskServer.h"
#include <stdlib.h>

typedef struct {
    char* Content;
    size_t Count;
} MessageToken;

typedef struct {
    MessageToken* Tokens;
    size_t Count;
    size_t Capacity;
} TokenList;

/// @brief Gets a single line form the HTTP request (i.e. until a CLRF)
/// @param result the resulting token (start of buffer + count until end of line)
/// @param buffer the text buffer to read from
/// @param bufferCount  size of buffer
/// @return 0 on succes, -1 otherwise
int GetLine(MessageToken* result, char* buffer, size_t bufferCount) {
    for (size_t i = 0; i < bufferCount; i++) {
        if (buffer[i] == '\r') {
            if (i + 1 < bufferCount && buffer[i + 1] == '\n') {
                result->Content = buffer;
                result->Count = i;

                return 0;
            }
        }
    }

    return -1;
}

/// @brief Gets a single token from a line of a HTTP request (i.e. until a a SP or CRLF)
/// @param result the resulting token (start of buffer + count until SP or CRLF)
/// @param buffer the text buffer to read from
/// @param bufferCount  size of buffer
/// @return size of token on success, -1 otherwise
int GetToken(MessageToken* result, char* buffer, size_t bufferCount) {
    if (buffer[0] == '\r' || buffer[0] == '\n') {
        return 0;
    }

    for (size_t i = 0; i < bufferCount; i++) {
        if (buffer[i] == ' ') {
            result->Content = buffer;
            result->Count = i;
            
            return result->Count;
        } else  if (buffer[i] == '\r') {
            if (i + 1 < bufferCount && buffer[i + 1] == '\n') {
                result->Content = buffer;
                result->Count = i;

                return result->Count;
            }
        }
    }

    return -1;
}

void AddToken(TokenList* list, MessageToken token) {
    if (list->Count == list->Capacity) {
        size_t newCap = (list->Capacity == 0) ? 4 : list->Capacity * 2;
        list->Tokens = realloc(list->Tokens, newCap * sizeof(MessageToken));
        list->Capacity = newCap;
    }

    list->Tokens[list->Count++] = token;
}


int GetTokenList(TokenList* result, char* buffer, size_t bufferCount) {
    for (size_t i = 0; i < bufferCount;) {
        MessageToken token;
        
        int size = GetToken(&token, buffer + i, bufferCount - i);
        if (size == -1) {
            return -1;
        } else if (size == 0) {
            return result->Count;
        }
        AddToken(result, token);

        i += size + 1;
    } 

    return result->Count;
}