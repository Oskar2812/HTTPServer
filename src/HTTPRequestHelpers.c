#include "../include/libOskServer.h"

FieldLine* GetHeaderValue(HTTPRequest* request, char headerName[MAX_HTTP_HEADER_NAME_LENGTH], size_t nameCount) {
    for (size_t i = 0; i < request->Headers.Count; i++) {
        FieldLine* header = &request->Headers.FieldLines[i];
        if (nameCount != header->FieldName.Count) {
            continue;
        }

        if (strncmp(headerName, header->FieldName.Content, nameCount) == 0) {
            return header;
        }
    }
    return NULL;
}