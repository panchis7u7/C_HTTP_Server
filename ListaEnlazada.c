#include <stdlib.h>
#include <stdio.h>
#include "ListaEnlazada.h"

int main(){
    int hola = 6;
    int hola2 = 2;
    Lista* l1 = Crear();
    insertar(l1, (void*)&hola);
    insertar(l1, (void*)&hola2);
    //printf("%d", l1->cuenta);
    imprimir(l1);
    imprimir_ant(l1);
    Destruir(l1);
    //imprimir(l1);
    return 0;
}

Lista* Crear(void){
    return calloc(1, sizeof(Lista));
}

void* Destruir(Lista* lista){
    Nodo* tmp = lista->raiz, *sig; 
    while(tmp != NULL){
        sig = tmp->sig;
        free(tmp);
        tmp = sig;
    } 
    free(lista);
}

//insertar un nodo al principio de la lista.
void* insertar(Lista* lista, void* dato){
    Nodo* nuevo = calloc(1, sizeof(Nodo));
    if(nuevo != NULL) {
        nuevo->dato = dato;
        if(lista->raiz == NULL){
            lista->raiz = nuevo;
            nuevo->sig = NULL;
            nuevo->ant = NULL;
            return dato;
        } else {
            nuevo->sig = lista->raiz;
            lista->raiz->ant = nuevo;
            nuevo->ant = NULL;
            lista->raiz = nuevo;
            return dato;
        }
    }
    return NULL;
}

//insertar un nodo al final de la lista.
void* insertar_final(Lista* lista, void* dato){
    Nodo* cola = lista->raiz;
    if(lista->raiz == NULL)
        return insertar(lista, dato);
    Nodo* nuevo = calloc(1, sizeof(Nodo));
    if(nuevo != NULL){
        while(cola->sig != NULL){
            cola = cola->sig;
        }
        nuevo->dato = dato;
        cola->sig = nuevo;
        nuevo->sig = NULL;
        lista->cuenta++;
        return dato;
    }
}

//regresa el primer elemento de la lista.
void* primer_elemeto(Lista* lista){
    if(lista->raiz == NULL)
    return NULL;
    return lista->raiz->dato;
}

//regresa el ultimo elemento de una lista.
void* ultimo_elemento(Lista* lista){
    Nodo* tmp = lista->raiz;
    if(tmp != NULL){
        while(tmp->sig != NULL){
            tmp = tmp->sig;
        }
        return tmp->dato;
    }
}

//Elimina un elemento en la lista, regresa 
void* eliminar(Lista* lista, void* dato, int (*cmpfn)(void*, void*)){
    Nodo* tmp = lista->raiz, *prev = NULL;
    while(tmp != NULL){
        if(cmpfn(dato, tmp->dato) == 0){
            void* data = tmp->dato;
        }
    }
}

void imprimir(Lista* lista){
    Nodo* tmp = lista->raiz;
    printf("|-");
    while(tmp->sig != NULL){
        printf("|%d|-", *((int*)tmp->dato));
        tmp = tmp->sig;
    }
    printf("|%d|-", *((int*)tmp->dato));
    printf(">\n");
}

void imprimir_ant(Lista* lista){
	Nodo* le = lista->raiz;
	printf("<-");
	while(le->sig != NULL){
		le= le->sig;
	}
	while(le != NULL){
		printf("|%d|-", *((int*)le->dato));
		le = le->ant;
	}
	printf("|\n");
}
