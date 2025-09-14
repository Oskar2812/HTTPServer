#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>

#include "ResultCodes.h"

ResultCode ReadIncomingData(FILE* file, char* buffer, int count);

#endif