#ifndef _QUEUE_H_
#define _QUEUE_H

typedef struct node_cola {
    struct node_cola* sig;
    int* socket_cliente;
}node_cola;

void ins_queue(int*);
int* del_queue();

#endif  //_QUEUE_H_