#include "Queue.h"
#include <stdlib.h>

node_cola* cabeza = NULL;
node_cola* cola = NULL;

void ins_queue(int* sock){
    node_cola* nuevo = malloc(sizeof(node_cola));
    nuevo->socket_cliente = sock;
    nuevo->sig = NULL;
    if(cola == NULL){
        cabeza = nuevo;
    } else {
        cola->sig = nuevo;
    }
    cola = nuevo;
}

int* del_queue(){
    if(cabeza == NULL){
        return NULL;
    } else {
        int* resultado = cabeza->socket_cliente;
        node_cola* tmp = cabeza;
        if (cabeza == NULL) {
            cola = NULL;
        }
        free(tmp);
        return resultado;
    }
}