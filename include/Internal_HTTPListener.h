#include "libOskServer.h"

typedef struct 
{
    char* Content;
    size_t Count;
    size_t Capacity;
} TextBuffer;


/// @brief Starts the server listening ona specific port 
/// @param server the server tointialise
/// @param port the prt
/// @return 0 on succes, -1 on failure
int InitiateListeningPort(HTTPServer* server, uint16_t port);

/// @brief Reads the preamble from the buffer
/// @param buffer the buffer to read from
/// @param clientSocket the socket assigned to the client
/// @return size of preamble on succes, -1 otherwise 
int ReadPreamble(TextBuffer* buffer, SOCKET clientSocket);

/// @brief Reads the body into the buffer and points the string view in the requets struct at it. 
/// Note: I don't know if it's better to simply have this method read the body into the TextBuffer and the have a seperate method to add it into the request struct
/// as that keeps things consistent between the networking and parsing but we dont do any actual parsing here and we pass in the request struct anyway as we need to lok at the headers
/// to read thie body so we might as well?
/// @todo: Support Transfer-Encoding
/// @param buffer The buffer to read into 
/// @param request the underlying request
/// @param clientSocket the socket assigned to the client 
/// @return size of body on success, -1 otherwise 
int ReadBody(TextBuffer* buffer, size_t preambleOffset, HTTPRequest* request, SOCKET clientSocket);