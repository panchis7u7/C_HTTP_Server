#include <stdlib.h>
#include <stdio.h>
#include "ListaEnlazada.h"

//Crea una nueva lista.
struct Lista* crear_lista(void){
    return (struct Lista*)calloc(1, sizeof(struct Lista));
}

//Destruye una lista.
void destruir_lista(struct Lista* lista){
    struct Nodo* tmp = lista->raiz, *sig; 
    while(tmp != NULL){
        sig = tmp->sig;
        free(tmp);
        tmp = sig;
    } 
    free(lista);
}

//insertar un nodo al principio de la lista.
void* insertar_lista(struct Lista* lista, void* dato){
    struct Nodo* nuevo = (struct Nodo*)calloc(1,sizeof *nuevo);
    if(nuevo == NULL){
        return NULL;
    }
    nuevo->dato = dato;
    nuevo->sig = lista->raiz;
    lista->raiz = nuevo;

    lista->cuenta++;
    return dato;
}

//insertar un nodo al final de la lista.
void* insertar_final_lista(struct Lista* lista, void* dato){
    struct Nodo* cola = lista->raiz;
    if(lista->raiz == NULL){
        return insertar_lista(lista, dato);
    }
    struct Nodo* nuevo = (struct Nodo*)calloc(1, sizeof *nuevo);
    if(nuevo == NULL){
        return NULL;
    }
    
    while(cola->sig != NULL){
        cola = cola->sig;
    }

    nuevo->dato = dato;
    cola->sig = nuevo;
    lista->cuenta++;
    return dato;
}

//regresa el primer elemento de la lista.
void* primer_elemeto_lista(struct Lista* lista){
    if(lista->raiz == NULL){
    return NULL;
    }
    return lista->raiz->dato;
}

//regresa el ultimo elemento de una lista.
void* ultimo_elemento_lista(struct Lista* lista){
    struct Nodo* tmp = lista->raiz;
    if(tmp == NULL){
        return NULL;
    }
    
    while(tmp->sig != NULL){
        tmp = tmp->sig;
    }
    return tmp->dato;
}

//Emcontrar un elemento en la lista, int (*cmpfn)(void*, void*) es un apuntador a una funcion.
void* encontrar_lista(struct Lista* lista, void* dato, int (*cmpfn)(void*, void*)){
    struct Nodo* tmp = lista->raiz;
    if(tmp == NULL){
        return NULL;
    }
    while (tmp != NULL){
        if(cmpfn(dato, tmp->dato) == 0){
            break;
        }
        tmp = tmp->sig;
    }
    if(tmp == NULL){
        return NULL;
    }
    return tmp->dato;
}

//Elimina un elemento en la lista.
void* eliminar_lista(struct Lista* lista, void* dato, int (*cmpfn)(void*, void*)){
    struct Nodo* tmp = lista->raiz, *prev = NULL;
    while(tmp != NULL){
        if(cmpfn(dato, tmp->dato) == 0){
            void* data = tmp->dato;
            //Liberar la cabeza.
            if(prev == NULL){
                lista->raiz = tmp->sig;
                free(tmp);
            } else {
               prev->sig = tmp->sig;
                free(tmp);
            }
            lista->cuenta--;
            return data;
        }
        prev = tmp;
        tmp = tmp->sig;
    }
    return NULL;   
}

//Regresa el numero de nodos en la lista.
int cuenta_lista(struct Lista* lista) {return lista->cuenta; }

//Para cada item/nodo de la lista, corre una funcion.
void foreach_lista(struct Lista* lista, void (*f)(void*, void*), void* arg){
    struct Nodo* tmp = lista->raiz, *siguiente;
    while(tmp != NULL){
        siguiente = tmp->sig;
        f(tmp->dato, arg);
        tmp = siguiente;
    }
}

//Regresa un arreglo en base a la informacion almacenada en la lista enlazada.
void** obtener_arreglo_lista(struct Lista* lista){
    if(lista->raiz == NULL)
        return NULL;
    void** a = malloc(sizeof *a * lista->cuenta + 1);
    struct Nodo* tmp;
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

void imprimir_sig_lista(struct Lista* lista){
    struct Nodo* tmp = lista->raiz;
    printf("|-");
    while(tmp->sig != NULL){

        printf("|%d|-", *((int*)tmp->dato));
        tmp = tmp->sig;
    }
    printf("|%d|-", *((int*)tmp->dato));
    printf(">\n");
}