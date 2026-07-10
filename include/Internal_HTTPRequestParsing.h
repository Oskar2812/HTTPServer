#ifndef INTERNAL_HTTP_REQUEST_PARSING_H
#define INTERNAL_HTTP_REQUEST_PARSING_H

#include "libOskServer.h"

/// @brief Parses full message peramble
/// @param request request to read into
/// @param buffer buffer to read from
/// @param bufferCount sbuffer
/// @return offset to end of preamble on succes, -1 on failure
int ParseMessagePreamble(HTTPRequest* request, char* buffer, size_t bufferCount);

#endif