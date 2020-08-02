#ifndef _LISTAENLAZADA_H_
#define _LISTAENLAZADA_H_

typedef struct Lista {
    struct Nodo* raiz;
    int cuenta;
}Lista;

typedef struct Nodo {
    struct Nodo* sig;
    struct Nodo* ant;
    void* dato;
}Nodo;

extern Lista* Crear(void);
extern void* Destruir(Lista*);
extern void* insertar(Lista*, void*);
extern void* insertar_final(Lista*, void*);
extern void* primer_elemeto(Lista*);
extern void* ultimo_elemento(Lista*);
extern void* encontrar(Lista*, void*, int(*cmpfn)(void*, void*));
extern void* eliminar(Lista*, void*, int(*cmpfn)(void*, void*));
extern void** obtener_arreglo(Lista*);
extern void foreach(Lista*, void(*f)(void*, void*), void* arg);
extern void liberar_arreglo_lista(void**);
extern void imprimir(Lista*);
extern void imprimir_ant(Lista*);
extern int cuenta(Lista*);

#endif //_LISTAENLAZADA_H_