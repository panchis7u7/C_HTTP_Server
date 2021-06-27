#ifndef _QUEUE_H_
#define _QUEUE_H

typedef struct NodeQueue {
    struct NodeQueue* next;
    int* clientSocket;
}NodeQueue;

void insertQueue(int*);
int* removeQueue();

#endif  //_QUEUE_H_