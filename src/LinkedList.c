#include <stdlib.h>
#include <stdio.h>
#include "LinkedList.h"

//Crea una nueva lista.
List* createList(void){
    return (List*)calloc(1, sizeof(List));
}

//Destruye una lista.
void destroyList(List* list){
    Node* tmp = list->root, *next; 
    while(tmp != NULL){
        next = tmp->next;
        free(tmp);
        tmp = next;
    } 
    free(list);
}

//insertar un nodo al principio de la lista.
void* insertList(List* list, void* data){
    Node* new = (Node*)calloc(1,sizeof *new);
    if(new == NULL){
        return NULL;
    }
    new->data = data;
    new->next = list->root;
    list->root = new;

    list->count++;
    return data;
}

//insertar un nodo al final de la lista.
void* insertListEnd(List* list, void* data){
    Node* tail = list->root;
    if(list->root == NULL){
        return insertList(list, data);
    }
    Node* new = (Node*)calloc(1, sizeof *new);
    if(new == NULL){
        return NULL;
    }
    
    while(tail->next != NULL){
        tail = tail->next;
    }

    new->data = data;
    tail->next = new;
    list->count++;
    return data;
}

//regresa el primer elemento de la lista.
void* firstElementList(List* list){
    if(list->root== NULL){
    return NULL;
    }
    return list->root->data;
}

//regresa el ultimo elemento de una lista.
void* ultimo_elemento_lista(List* list){
    Node* tmp = list->root;
    if(tmp == NULL){
        return NULL;
    }
    
    while(tmp->next != NULL){
        tmp = tmp->next;
    }
    return tmp->data;
}

//Emcontrar un elemento en la lista, int (*cmpfn)(void*, void*) es un apuntador a una funcion.
void* findList(List* list, void* data, int (*cmpfn)(void*, void*)){
    Node* tmp = list->root;
    if(tmp == NULL){
        return NULL;
    }
    while (tmp != NULL){
        if(cmpfn(data, tmp->data) == 0){
            break;
        }
        tmp = tmp->next;
    }
    if(tmp == NULL){
        return NULL;
    }
    return tmp->data;
}

//Elimina un elemento en la lista.
void* deleteList(List* list, void* data, int (*cmpfn)(void*, void*)){
    Node* tmp = list->root, *prev = NULL;
    while(tmp != NULL){
        if(cmpfn(data, tmp->data) == 0){
            void* data = tmp->data;
            //Liberar la cabeza.
            if(prev == NULL){
                list->root = tmp->next;
                free(tmp);
            } else {
               prev->next = tmp->next;
                free(tmp);
            }
            list->count--;
            return data;
        }
        prev = tmp;
        tmp = tmp->next;
    }
    return NULL;   
}

//Regresa el numero de nodos en la lista.
int listCount(List* list) { return list->count; }

//Para cada item/nodo de la lista, corre una funcion.
void forEachList(List* list, void (*f)(void*, void*), void* arg){
    Node* tmp = list->root, *next;
    while(tmp != NULL){
        next = tmp->next;
        f(tmp->data, arg);
        tmp = next;
    }
}

//Regresa un arreglo en base a la informacion almacenada en la lista enlazada.
void** getListArray(List* list){
    if(list->root == NULL)
        return NULL;
    void** a = malloc(sizeof *a * list->count + 1);
    Node* tmp;
    int i = 0;
    for (i = 0, tmp = list->root; tmp != NULL; i++, tmp = tmp->next)
    {
        a[i] = tmp->data;
    }
    a[i] = NULL;
    return a;
}

//Libera version estatica de la lista enlazada.
void freeListArray(void** a){ free(a); }

void imprimir_sig_lista(List* list){
    Node* tmp = list->root;
    printf("|-");
    while(tmp->next != NULL){

        printf("|%d|-", *((int*)tmp->data));
        tmp = tmp->next;
    }
    printf("|%d|-", *((int*)tmp->data));
    printf(">\n");
}