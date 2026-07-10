#ifndef INTERNAL_HTTP_SENDER_H
#define INTERNAL_HTTP_SENDER_H

#include "libOskServer.h"

#include "Internal_HTTPListener.h"

/// @brief Sends data back to client
/// @param client client to send too
/// @param buffer buffer to send
/// @return 0 on success, -1 otherwise
int SendData(SOCKET client, TextBuffer* buffer);

#endif