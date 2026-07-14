#include "libOskServer.h"

#define QUEUE_CAPACITY 64

#define WORKER_COUNT 4

typedef struct {
    SOCKET Items[QUEUE_CAPACITY];
    int Head;
    int Tail;
    int Count;

    CRITICAL_SECTION Lock;
    CONDITION_VARIABLE NotEmpty;
    CONDITION_VARIABLE NotFull;

    int ShuttingDown;
} ConnectionQueue;

typedef struct {
    ConnectionQueue* Queue;
    HTTPServer* Server;
} WorkerContext;
void QueueInit(ConnectionQueue* q);

int QueuePop(ConnectionQueue* q, SOCKET* outClient);

void QueuePush(ConnectionQueue* q, SOCKET client);

DWORD WINAPI WorkerThreadFunc(LPVOID param);