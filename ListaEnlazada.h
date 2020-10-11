#ifndef _LISTAENLAZADA_H_
#define _LISTAENLAZADA_H_

struct Lista {
    struct Nodo* raiz;
    int cuenta;
};//Lista;

struct Nodo {
    struct Nodo* sig;
    void* dato;
};//Nodo;

extern struct Lista* crear_lista(void);
extern void destruir_lista(struct Lista* lista);
extern void* insertar_lista(struct Lista* lista, void* dato);
extern void* insertar_final_lista(struct Lista* lista, void* dato);
extern void* primer_elemeto_lista(struct Lista* lista);
extern void* ultimo_elemento_lista(struct Lista* lista);
extern void* encontrar_lista(struct Lista* lista, void* dato, int (*cmpfn)(void*, void*));
extern void* eliminar_lista(struct Lista* lista, void* dato, int(*cmpfn)(void*, void*));
extern void** obtener_arreglo_lista(struct Lista* lista);
extern void foreach_lista(struct Lista* lista, void(*f)(void*, void*), void* arg);
extern void liberar_arreglo_lista(void** a);
extern void imprimir_sig_lista(struct Lista* lista);
extern void imprimir_ant_lista(struct Lista* lista);
extern int cuenta_lista(struct Lista* lista);

#endif //_LISTAENLAZADA_H_