#include "libOskServer.h"

#include "Internal_HTTPListener.h"

/// @brief Serialises the HTTP response struct
/// @param result the resulting text
/// @param response the response to serialise
/// @return the size of buffer on success, -1 on failure
int SerialiseResponse(TextBuffer* result, HTTPResponse* response);