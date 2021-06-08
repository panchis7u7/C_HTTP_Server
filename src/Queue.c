#include "Queue.h"
#include <stdlib.h>

NodeQueue* head = NULL;
NodeQueue* tail = NULL;

void insertQueue(int* sock){
    NodeQueue* nuevo = malloc(sizeof(NodeQueue));
    nuevo->socket_cliente = sock;
    nuevo->sig = NULL;
    if(tail == NULL){
        head = nuevo;
    } else {tail->sig = nuevo;}
    tail = nuevo;
}

int* removeQueue(){
    if(head == NULL){
        return NULL;
    } else {
        int* resultado = head->socket_cliente;
        NodeQueue* tmp = head;
        if (head == NULL) { tail = NULL; }
        free(tmp);
        return resultado;
    }
}