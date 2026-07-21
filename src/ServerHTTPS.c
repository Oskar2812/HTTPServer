#ifndef SERVERHTTPS_C
#define SERVERHTTPS_C

#include "../include/libOskServer.h"
#include "../include/Internal_ServerHTTPS.h"
#include "../include/Internal_HTTPListener.h"
#include "../include/Internal_UtilityMacros.h"

int LoadCertificateFromStore(PCCERT_CONTEXT* certificate, const char* thumbprint, const char* store) {
    HCERTSTORE storeHandle = CertOpenStore(CERT_STORE_PROV_SYSTEM_A, 0, 0, CERT_SYSTEM_STORE_LOCAL_MACHINE, store);
    if (store == NULL) {
        return -1;
    }

    BYTE thumbprintBytes[20];
    DWORD thumbprintLen = sizeof(thumbprintBytes);

    BOOL decoded = CryptStringToBinary(thumbprint, 0, CRYPT_STRING_HEXRAW, thumbprintBytes, &thumbprintLen, NULL, NULL);
    if (!decoded) {
        return -1;
    }

    CRYPT_HASH_BLOB hashBlob;
    hashBlob.cbData = thumbprintLen;
    hashBlob.pbData = thumbprintBytes;

    PCCERT_CONTEXT certContext = CertFindCertificateInStore(storeHandle, X509_ASN_ENCODING |PKCS_7_ASN_ENCODING, 0, CERT_FIND_HASH, &hashBlob, NULL);
    CertCloseStore(storeHandle, 0);
    if (certContext == NULL) {
        return -1;
    }

    *certificate = certContext;

    return 0;
}

int GetCredHandle(CredHandle* handle, PCCERT_CONTEXT certificate) {
    SCH_CREDENTIALS credentials = {0};

    credentials.dwVersion = SCH_CREDENTIALS_VERSION;
    credentials.cCreds= 1;
    credentials.paCred = &certificate;
    credentials.dwFlags = 0; // SCH_CRED_NO_SYSTEM_MAPPER;

    SECURITY_STATUS status = AcquireCredentialsHandleA(NULL, UNISP_NAME, SECPKG_CRED_INBOUND, NULL, &credentials, NULL, NULL, handle, NULL);
    if (status != SEC_E_OK) {
        return -1;
    }

    return 0;
}

int EnableHTTPS(HTTPServer* server, const char* certificateThumbprint, const char* certificateStore) {
    server->Secure = 1;

    PCCERT_CONTEXT certificate;
    ASSERT_SUCCESS(LoadCertificateFromStore(&certificate, certificateThumbprint, certificateStore));

    ASSERT_SUCCESS(GetCredHandle(&server->HTTPSCredentials, certificate));

    return 0;
}

int DoTLSHandshake(PCtxtHandle context, TextBuffer* buffer, SOCKET socket, CredHandle credentials) {
    ASSERT_SUCCESS(AllocateBufferMemory(buffer, KB));

    CtxtHandle outputContext;
    BOOL hasContext = FALSE;
    ULONG contextAttribute;

    while(1) {
        int bytes = ReadBytes(buffer, socket);
        ASSERT_SUCCESS(bytes);

        SecBuffer secBuffers[2];

        secBuffers[0].BufferType = SECBUFFER_TOKEN;
        secBuffers[0].cbBuffer = buffer->Count;
        secBuffers[0].pvBuffer = buffer->Content;

        secBuffers[1].BufferType = SECBUFFER_EMPTY;
        secBuffers[1].cbBuffer = 0;
        secBuffers[1].pvBuffer = NULL;

        SecBufferDesc inputDesc = {
            .ulVersion = SECBUFFER_VERSION,
            .cBuffers = 2,
            .pBuffers = secBuffers
        };

        SecBuffer outputBuffers[1];

        outputBuffers[0].BufferType = SECBUFFER_TOKEN;
        outputBuffers[0].cbBuffer = 0;
        outputBuffers[0].pvBuffer = NULL; 

        SecBufferDesc outputDesc;
        outputDesc.ulVersion = SECBUFFER_VERSION;
        outputDesc.cBuffers = 1;
        outputDesc.pBuffers = outputBuffers;

        SECURITY_STATUS status = AcceptSecurityContext(&credentials, hasContext ? &outputContext : NULL, &inputDesc, ASC_REQ_ALLOCATE_MEMORY, 0, &outputContext, &outputDesc, &contextAttribute, NULL);
        if (status == SEC_E_INCOMPLETE_MESSAGE) {
            continue;
        }
        hasContext = TRUE;
        if (outputBuffers[0].cbBuffer > 0) {
            int sent = send(socket, outputBuffers[0].pvBuffer, outputBuffers[0].cbBuffer, 0);
            FreeContextBuffer(outputBuffers[0].pvBuffer);
            if (sent == SOCKET_ERROR) {
                return -1;
            }
        }
        switch (status) {
            case SEC_I_CONTINUE_NEEDED:
                buffer->Count = 0;
                break;
            case SEC_E_OK:
                if (secBuffers[1].BufferType == SECBUFFER_EXTRA && secBuffers[1].cbBuffer > 0) {
                    char* temp = malloc(secBuffers[1].cbBuffer);
                    memcpy(temp, (char*)secBuffers[0].pvBuffer + (secBuffers[0].cbBuffer - secBuffers[1].cbBuffer), secBuffers[1].cbBuffer);

                    memcpy(buffer->Content, temp, secBuffers[1].cbBuffer);
                    buffer->Count = secBuffers[1].cbBuffer;
                    
                    free(temp);
                }
                else {
                    buffer->Count = 0;
                }

                *context = outputContext;

                return 0;
            default:
                return -1;
        }
    }
}

int HandleDecryptedBytes(SecBuffer* secBuffers, TextBuffer* textBuffer) {
    int readDecryptedBytes = 0;
    for (int i = 0; i < 4; i++) {
        if (secBuffers[i].BufferType == SECBUFFER_DATA) {
            char* temp = malloc(sizeof(char) * secBuffers[i].cbBuffer);
            memcpy(temp, secBuffers[i].pvBuffer, secBuffers[i].cbBuffer);

            memcpy(textBuffer->Content + textBuffer->DecryptedCount, temp, secBuffers[i].cbBuffer);
            textBuffer->Count = secBuffers[i].cbBuffer;
            textBuffer->DecryptedCount = secBuffers[i].cbBuffer;

            readDecryptedBytes = 1;
            free(temp);
            break;
        }
    }

    if (!readDecryptedBytes) {
        return -1;
    }

    for (int i = 0; i < 4; i++) {
        if (secBuffers[i].BufferType == SECBUFFER_EXTRA) {
            char* temp = malloc(sizeof(char) * secBuffers[i].cbBuffer);
            memcpy(temp, secBuffers[i].pvBuffer, secBuffers[i].cbBuffer);

            memcpy(textBuffer->Content + textBuffer->Count, temp, secBuffers[i].cbBuffer);
            textBuffer->Count += secBuffers[i].cbBuffer;

            free(temp);
            return secBuffers[i].cbBuffer;
        }
    }

    return 0;
}

int DecryptBytes(TextBuffer* buffer, PCtxtHandle securityContext) {
    SecBuffer secBuffers[4];

    secBuffers[0].BufferType = SECBUFFER_DATA;
    secBuffers[0].cbBuffer = buffer->Count - buffer->DecryptedCount;
    secBuffers[0].pvBuffer = buffer->Content + buffer->DecryptedCount;
    for (int i = 1; i < 4; i++) {
        secBuffers[i].BufferType = SECBUFFER_EMPTY;
        secBuffers[i].cbBuffer = 0;
        secBuffers[i].pvBuffer = NULL;
    }

    SecBufferDesc secbufferDesc = {0};
    secbufferDesc.ulVersion = SECBUFFER_VERSION;
    secbufferDesc.cBuffers = 4;
    secbufferDesc.pBuffers = secBuffers;

    SECURITY_STATUS status = DecryptMessage(securityContext, &secbufferDesc, 0, NULL);

    switch(status) {
        case SEC_I_CONTINUE_NEEDED:
            return 0;
        case SEC_E_OK:
            ASSERT_SUCCESS(HandleDecryptedBytes(secBuffers, buffer));

            return 1;
        default:
            return -1;
    }
}

#endif