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

extern Lista* Crear_lista(void);
extern void* Destruir_lista(Lista* lista);
extern void* Insertar_lista(Lista* lista, void* dato);
extern void* Insertar_final_lista(Lista* lista, void* dato);
extern void* Primer_elemeto_lista(Lista* lista);
extern void* Ultimo_elemento_lista(Lista* lista);
extern void* Encontrar_lista(Lista* lista, void* dato, int(*cmpfn)(void*, void*));
extern void* Eliminar_lista(Lista* lista, void* dato, int(*cmpfn)(void*, void*));
extern void** Obtener_arreglo_lista(Lista* lista);
extern void Foreach_lista(Lista* lista, void(*f)(void*, void*), void* arg);
extern void Liberar_arreglo_lista(void** a);
extern void Imprimir_sig_lista(Lista* lista);
extern void Imprimir_ant_lista(Lista* lista);
extern int Cuenta_lista(Lista* lista);

#endif //_LISTAENLAZADA_H_