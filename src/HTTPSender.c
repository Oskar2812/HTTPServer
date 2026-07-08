#include "../include/libOskServer.h"
#include "../include/Internal_HTTPSender.h"
#include "../include/Internal_HTTPListener.h"

int SendData(SOCKET client, TextBuffer* buffer) {
    size_t totalSent = 0;

    while (totalSent < buffer->Count) {
        int sent = send(client, buffer->Content + totalSent, (int)(buffer->Count - totalSent), 0);
        if (sent == SOCKET_ERROR) {
            return -1;
        }

        totalSent += sent;
    }

    return 0;
}