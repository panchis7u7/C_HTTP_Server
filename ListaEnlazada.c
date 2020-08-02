#include <stdlib.h>
#include <stdio.h>
#include "ListaEnlazada.h"

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
        nuevo->ant = cola;
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

//Emcontrar un elemento en la lista.
void* encontrar(Lista* lista, void* dato, int (*cmpfn)(void*, void*)){
    Nodo* tmp = lista->raiz;
    if(tmp == NULL)
        return NULL;
    while (tmp != NULL){
        if(cmpfn(dato, tmp->dato)){
            break;
        }
        tmp = tmp->sig;
    }
    if(tmp == NULL)
        return NULL;
    return tmp->dato;
}

//Elimina un elemento en la lista.
void* eliminar(Lista* lista, void* dato, int (*cmpfn)(void*, void*)){
    Nodo* tmp = lista->raiz;
    while(tmp != NULL){
        if(cmpfn(dato, tmp->dato) == 0){
            void* data = tmp->dato;
            //Liberar la cabeza.
            if(tmp->ant == NULL){
                lista->raiz = tmp->sig;
                free(tmp);
            } else {
                tmp->ant->sig = tmp->sig;
                free(tmp);
            }
            lista->cuenta--;
            return dato;
        }
    }
    return NULL;   
}

//Regresa el numero de nodos en la lista.
int cuenta(Lista* lista) {return lista->cuenta; }

//Regresa un arreglo enbase a la informacion almacenada en la lista enlazada.
void**obtener_arreglo(Lista* lista){
    if(lista->raiz == NULL)
        return NULL;
    void** a = malloc(sizeof *a * lista->cuenta + 1);
    Nodo* tmp;
    int i = 0;
    for (i = 0, tmp = lista->raiz; tmp != NULL; i++, tmp = tmp->sig)
    {
        a[i] = tmp->dato;
    }
    a[i] = NULL;
    return a;
}

//Libera version estatica de la lista enlazada.
void liberar_arreglo_lista(void** a){ free(a); }

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
