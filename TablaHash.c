#include "TablaHash.h"
#include "ListaEnlazada.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANO_DEFAULT 128
#define FACTOR_CRECIMIENTO_DEFAULT 2

//Entrada de una tabla hash.
struct htent {
    void* llave;
    int tamano_llave;
    int llave_hash;
    void* dato;
};

//Usado para limpiar las listas enlazadas.
typedef struct carga_foreach_lista {
    void* arg;
    void (*f)(void*, void*);
}carga_foreach_lista;

//Cambia la cuenta de entrada y mantener las metricas de cargas.
void anadir_cuenta_entrada(hashtable* ht, int d){
    ht->numero_entradas += d;
    ht->carga = (float)ht->numero_entradas / ht->tamano;
}

//Funcion Hash Modular (Predeterminada).
int hashfn_predeterminada(void* dato, int tamano_dato, int conteo_cubeta){
    const int R = 31; //Numero primo.
    int h = 0;
    unsigned char *p = dato;
    int i;
    for (i = 0; i < tamano_dato; i++)
    {
        h = (R * h + p[i]) % conteo_cubeta;
    }
    return h;
}

hashtable* CrearHash(int tamano, int (*hashfn)(void*, int, int)){
    if(tamano < 1){
        tamano = TAMANO_DEFAULT;
    }
    if(hashfn == NULL){
        hashfn = hashfn_predeterminada;
    }

    hashtable* ht = malloc(sizeof *ht);

    if(ht == NULL)
        return NULL;

    ht->tamano = tamano;
    ht->numero_entradas = 0;
    ht->carga = 0;
    ht->cubeta = malloc(tamano * sizeof(Lista*));
    ht->hashf = hashfn;

    int i;
    for(i = 0; i < tamano; i++){
        ht->cubeta[i] = CrearLista();
    }
    return ht;
}

//Liberar htent.
void liberar_htent(void* htent, void* arg){
    (void)arg;
    free(htent);
}

//Destruir tabla hash.
void DestruirHash(hashtable* ht){
    int i;
    for(i = 0; i < ht->tamano; i++){
        Lista*  lista = ht->cubeta[i];
        Foreach_lista(lista, liberar_htent, NULL);
        DestruirLista(lista);
    }
    free(ht);
}