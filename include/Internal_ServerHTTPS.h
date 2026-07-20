#define SCHANNEL_USE_BLACKLISTS

#include "libOskServer.h"
#include "Internal_HTTPListener.h"
#include <schannel.h>
#include <wincrypt.h>

/// @brief Loads a certificate form the windows cert store
/// @param certificate the outputted cert context
/// @param thumbprint the thumbprint of the certificate to load
/// @param store the name of the sotre to load from
/// @return 0 on success, -1 on failure
int LoadCertificateFromStore(PCCERT_CONTEXT* certificate, const char* thumbprint, const char* store);

/// @brief Gets the credential handle for SChannel and a specified certificate 
/// @param handle the reutnred handle 
/// @param certificate the certiifcate to associate
/// @return 0 on success, -1 on failure
int GetCredHandle(CredHandle* handle, PCCERT_CONTEXT certificate);

/// @brief Does the TLS handshake with the client 
/// @param context the returned security context
/// @param buffer text buffer to write into - will potentially contain the start of the HTTP message on output 
/// @param socket the socket to recieve data on 
/// @param credentials
/// @return 0 on success, -1 on failure
int DoTLSHandshake(PCtxtHandle context, TextBuffer* buffer, SOCKET socket, CredHandle credentials);

/// @brief Decrypts a message given a security context
/// @param buffer the encrypted bytes - unencrypted message will appear here
/// @param securityContext the security context
/// @return -1 on failure, 0 on incomleted message or 1 on success
int DecryptBytes(TextBuffer* buffer, PCtxtHandle securityContext);