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
extern void* Destruir_lista(Lista*);
extern void* Insertar_lista(Lista*, void*);
extern void* Insertar_final_lista(Lista*, void*);
extern void* Primer_elemeto_lista(Lista*);
extern void* Ultimo_elemento_lista(Lista*);
extern void* Encontrar_lista(Lista*, void*, int(*cmpfn)(void*, void*));
extern void* Eliminar_lista(Lista*, void*, int(*cmpfn)(void*, void*));
extern void** Obtener_arreglo_lista(Lista*);
extern void Foreach_lista(Lista*, void(*f)(void*, void*), void* arg);
extern void Liberar_arreglo_lista(void**);
extern void Imprimir_sig_lista(Lista*);
extern void Imprimir_ant_lista(Lista*);
extern int Cuenta_lista(Lista*);

#endif //_LISTAENLAZADA_H_