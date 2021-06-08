#ifndef _QUEUE_H_
#define _QUEUE_H

typedef struct NodeQueue {
    struct NodeQueue* sig;
    int* socket_cliente;
}NodeQueue;

void insertQueue(int*);
int* removeQueue();

#endif  //_QUEUE_H_