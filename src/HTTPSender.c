#include "../include/libOskServer.h"
#include "../include/Internal_HTTPSender.h"
#include "../include/Internal_HTTPListener.h"

int SendData(SOCKET client, TextBuffer* buffer, PCtxtHandle securityContext) {
    size_t totalSent = 0;

    SecPkgContext_StreamSizes sizes;
    if (securityContext != NULL) {
        QueryContextAttributes(securityContext, SECPKG_ATTR_STREAM_SIZES, &sizes);
    }

    while (totalSent < buffer->Count) {

        if (securityContext != NULL) {
            size_t chunkSize = 0;
            if (sizes.cbMaximumMessage > (buffer->Count - totalSent)) {
                chunkSize = buffer->Count;
            }
            else {
                chunkSize = sizes.cbMaximumMessage;
            }       

            char* messageBuffer = malloc(sizes.cbHeader + chunkSize + sizes.cbTrailer);
            memcpy(messageBuffer + sizes.cbHeader, buffer->Content + totalSent, chunkSize);

            SecBuffer buffers[4];

            buffers[0].BufferType = SECBUFFER_STREAM_HEADER;
            buffers[0].pvBuffer = messageBuffer;
            buffers[0].cbBuffer = sizes.cbHeader;

            buffers[1].BufferType = SECBUFFER_DATA;
            buffers[1].pvBuffer = messageBuffer + sizes.cbHeader;
            buffers[1].cbBuffer = chunkSize;

            buffers[2].BufferType = SECBUFFER_STREAM_TRAILER;
            buffers[2].pvBuffer = messageBuffer + sizes.cbHeader + chunkSize;
            buffers[2].cbBuffer = sizes.cbTrailer;

            buffers[3].BufferType = SECBUFFER_EMPTY;
            buffers[3].pvBuffer = NULL;
            buffers[3].cbBuffer = 0;

            SecBufferDesc bufferDesc;
            bufferDesc.ulVersion = SECBUFFER_VERSION;
            bufferDesc.cBuffers = 4;
            bufferDesc.pBuffers = buffers;

            SECURITY_STATUS status = EncryptMessage(securityContext, 0, &bufferDesc, 0);
            if (status != SEC_E_OK) {
                free(messageBuffer);
                return -1;
            }

            int sent = send(client, messageBuffer, buffers[0].cbBuffer + buffers[1].cbBuffer + buffers[2].cbBuffer, 0);
            if (sent == SOCKET_ERROR) {
                return -1;
            }

            totalSent += chunkSize;
            free(messageBuffer);
        }
        else {
            int sent = send(client, buffer->Content + totalSent, (int)(buffer->Count - totalSent), 0);
            if (sent == SOCKET_ERROR) {
                return -1;
            }

            totalSent += sent;
        }
    }

    return 0;
}