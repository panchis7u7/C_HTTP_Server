#include "Queue.h"
#include <stdlib.h>
#include <unistd.h>

NodeQueue* head = NULL;
NodeQueue* tail = NULL;

void insertQueue(int* sock){
    NodeQueue* nuevo = malloc(sizeof(NodeQueue));
    nuevo->clientSocket = sock;
    nuevo->next = NULL;
    if(tail == NULL){
        head = nuevo;
    } else {tail->next = nuevo;}
    tail = nuevo;
}

int* removeQueue(){
    if(head == NULL){
        return NULL;
    } else {
        int* resultado = head->clientSocket;
        NodeQueue* tmp = head;
        head = head->next;
        if (head == NULL) { tail = NULL; }
        free(tmp);
        return resultado;
    }
}