#include "../include/libOskServer.h"

FieldLine* GetHeaderValue(MessageHeaders* headers, const char* headerName, size_t nameCount) {
    for (size_t i = 0; i < headers->Count; i++) {
        FieldLine* header = &headers->FieldLines[i];
        if (nameCount != header->FieldName.Count) {
            continue;
        }

        if (strncmp(headerName, header->FieldName.Content, nameCount) == 0) {
            return header;
        }
    }
    return NULL;
}

int SetVersion(HTTPResponse* response, HTTPVersion version) {
    response->Status.Version = version;
    return 0;
}

int SetStatus(HTTPResponse* response, HTTPStatus status) {
    response->Status.Status = status;
    return 0;
}

int SetBody(HTTPResponse* response, char* body, size_t bodySize) {
    response->Body.Content = malloc(sizeof(char) * bodySize);
    if (response->Body.Content == NULL) {
        return -1;
    }

    memcpy(response->Body.Content, body, bodySize);
    response->Body.Count = bodySize;

    return 0;
}

int AddHeader(MessageHeaders* list, FieldLine token) {
    if (list->Count == list->Capacity) {
        size_t newCap = (list->Capacity == 0) ? 4 : list->Capacity * 2;
        list->FieldLines = realloc(list->FieldLines, newCap * sizeof(FieldLine));
        list->Capacity = newCap;
    }

    list->FieldLines[list->Count++] = token;

    return 0;
}