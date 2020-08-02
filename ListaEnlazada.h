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

extern Lista* CrearLista(void);
extern void* Destruir(Lista*);
extern void* Insertar(Lista*, void*);
extern void* Insertar_final(Lista*, void*);
extern void* Primer_elemeto(Lista*);
extern void* Ultimo_elemento(Lista*);
extern void* Encontrar(Lista*, void*, int(*cmpfn)(void*, void*));
extern void* Eliminar(Lista*, void*, int(*cmpfn)(void*, void*));
extern void** Obtener_arreglo(Lista*);
extern void Foreach(Lista*, void(*f)(void*, void*), void* arg);
extern void Liberar_arreglo_lista(void**);
extern void Imprimir(Lista*);
extern void Imprimir_ant(Lista*);
extern int Cuenta(Lista*);

#endif //_LISTAENLAZADA_H_