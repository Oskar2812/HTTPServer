#include "../include/libOskServer.h"

#include "../include/Internal_ServerMultithreading.h"
#include "../include/Internal_HTTPServer.h"

void QueueInit(ConnectionQueue* q) {
    q->Head = 0;
    q->Tail = 0;
    q->Count = 0;
    q->ShuttingDown = 0;
    InitializeCriticalSection(&q->Lock);
    InitializeConditionVariable(&q->NotEmpty);
    InitializeConditionVariable(&q->NotFull);
}

void QueuePush(ConnectionQueue* q, SOCKET client) {
    EnterCriticalSection(&q->Lock);

    while (q->Count == QUEUE_CAPACITY && !q->ShuttingDown) {
        SleepConditionVariableCS(&q->NotFull, &q->Lock, INFINITE);
    }

    if (q->ShuttingDown) {
        LeaveCriticalSection(&q->Lock);
        return;
    }

    q->Items[q->Tail] = client;
    q->Tail = (q->Tail + 1) % QUEUE_CAPACITY;
    q->Count++;

    LeaveCriticalSection(&q->Lock);
    WakeConditionVariable(&q->NotEmpty);
}

int QueuePop(ConnectionQueue* q, SOCKET* outClient) {
    EnterCriticalSection(&q->Lock);

    while (q->Count == 0 && !q->ShuttingDown) {
        SleepConditionVariableCS(&q->NotEmpty, &q->Lock, INFINITE);
    }

    if (q->Count == 0 && q->ShuttingDown) {
        LeaveCriticalSection(&q->Lock);
        return -1; // nothing to do, shutting down
    }

    *outClient = q->Items[q->Head];
    q->Head = (q->Head + 1) % QUEUE_CAPACITY;
    q->Count--;

    LeaveCriticalSection(&q->Lock);
    WakeConditionVariable(&q->NotFull);
    return 0;
}

DWORD WINAPI WorkerThreadFunc(LPVOID param) {
    WorkerContext* ctx = (WorkerContext*)param;

    while (1) {
        SOCKET client;
        int got = QueuePop(ctx->Queue, &client);
        if (got == -1) {
            break;
        }

        HandleConnection(ctx->Server, client);
    }

    return 0;
}