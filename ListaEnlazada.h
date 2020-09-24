#ifndef _LISTAENLAZADA_H_
#define _LISTAENLAZADA_H_

typedef struct Lista {
    struct Nodo* raiz;
    int cuenta;
}Lista;

typedef struct Nodo {
    struct Nodo* sig;
    void* dato;
}Nodo;

extern Lista* crear_lista(void);
extern void destruir_lista(Lista* lista);
extern void* insertar_lista(Lista* lista, void* dato);
extern void* insertar_final_lista(Lista* lista, void* dato);
extern void* primer_elemeto_lista(Lista* lista);
extern void* ultimo_elemento_lista(Lista* lista);
extern void* encontrar_lista(Lista* lista, void* dato, int(*cmpfn)(void*, void*));
extern void* eliminar_lista(Lista* lista, void* dato, int(*cmpfn)(void*, void*));
extern void** obtener_arreglo_lista(Lista* lista);
extern void foreach_lista(Lista* lista, void(*f)(void*, void*), void* arg);
extern void liberar_arreglo_lista(void** a);
extern void imprimir_sig_lista(Lista* lista);
extern void imprimir_ant_lista(Lista* lista);
extern int cuenta_lista(Lista* lista);

#endif //_LISTAENLAZADA_H_