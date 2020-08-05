#include "TablaHash.h"
#include "ListaEnlazada.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANO_DEFAULT 128
#define FACTOR_CRECIMIENTO_DEFAULT 2

//Prototipos de Funcion.
int htcmp(void*, void*);
void Anadir_cuenta_entrada(hashtable*, int);
void Foreach_callback(void*, void*);
void liberar_htent(void*, void*);
int hashfn_predeterminada(void*, int, int);

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
void Anadir_cuenta_entrada(hashtable* ht, int d){
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

hashtable* Crear_hash(int tamano, int (*hashfn)(void*, int, int)){
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
void Destruir_hash(hashtable* ht){
    int i;
    for(i = 0; i < ht->tamano; i++){
        Lista*  lista = ht->cubeta[i];
        Foreach_lista(lista, liberar_htent, NULL);
        DestruirLista(lista);
    }
    free(ht);
}

//Insertar en la tabla hash con una llave tipo cadena (string).
void* Put_hash(hashtable* ht, char* llave, void* dato){
    return Put_hash_bin(ht, llave, strlen(llave), dato);
}

//Insertar en la tabla hash con una llave binaria.
void* Put_hash_bin(hashtable* ht, void* llave, int tamano_llave, void* dato){
    int indice = ht->hashf(llave, tamano_llave, ht->tamano);
    Lista* lista = ht->cubeta[indice];
    struct htent* ent = malloc(sizeof *ent);
    ent->llave = malloc(tamano_llave);
    memcpy(ent->llave, llave, tamano_llave);
    ent->tamano_llave = tamano_llave;
    ent->llave_hash = indice;
    ent->dato = dato;
    if(Insertar_final(lista, ent) == NULL){
        free(ent->llave);
        free(ent);
    }
    Anadir_cuenta_entrada(ht, +1);
    return dato;
}

//Obtener valor de la tabla hash con una llave tipo cadena (string).
void* Get_hash(hashtable* ht, char* llave){
    return Get_hash_bin(ht, llave, strlen(llave));
}

//Obtener valor de la tabla hash con una llave binaria.
void* Get_hash_bin(hashtable* ht, void* llave, int tamano_llave){
    int indice = ht->hashf(llave, tamano_llave, ht->tamano);
    Lista* lista = ht->cubeta[indice];
    struct htent cmpent;
    cmpent.llave = llave;
    cmpent.tamano_llave = tamano_llave;
    struct htent *tmp = Encontrar_lista(lista, &cmpent, htcmp);
    if(tmp == NULL)
        return NULL;
    return tmp->dato;
}

//Funcion de comparacion para entradas a la tabla hash.
int htcmp(void* a, void* b){
    struct htent* entA = a, *entB = b;
    int dif_tamano = entB->tamano_llave - entA->tamano_llave;
    if(dif_tamano)
        return dif_tamano;  
    return memcpy(entA->llave, entB->llave, entA->tamano_llave);
}

//Borrar valor de la tabla hash con una llave tipo cadena (string).
void* Eliminar_hash(hashtable* ht, char* llave){
    return Eliminar_hash_bin(ht, llave, strlen(llave));
}

//Borrar valor de la tabla hash con una llave Binaria.
void* Eliminar_hash_bin(hashtable* ht, void* llave, int tamano_llave){
    int indice = ht->hashf(llave, tamano_llave, ht->tamano);
    Lista* lista = ht->cubeta[indice];
    struct htent cmpent;
    cmpent.llave = llave;
    cmpent.tamano_llave = tamano_llave;
    struct htent* ent = Eliminar_lista(lista, &cmpent, htcmp);
    if(ent == NULL)
        return NULL;    
    void* dato = ent->dato;
    free(ent);
    Anadir_cuenta_entrada(ht, -1);
    return dato;
}

//Funcion de llamada de vuelta (callback) para cada elemento (Foreach).
void Foreach_callback(void* vent, void* v_carga_util){
    struct htent* ent = vent;
    struct carga_foreach_lista* carga_util = v_carga_util;
    carga_util->f(ent->dato, carga_util->arg);
}

//Para cada elemento en la tabla hash.
void Foreach_hash(hashtable* ht, void(*f)(void*, void*), void* arg){
    carga_foreach_lista carga_util;
    carga_util.f = f;
    carga_util.arg = arg;
    size_t i;
    for (i = 0; i < ht->tamano; i++)
    {
        Lista* lista = ht->cubeta[i];    
        Foreach_lista(lista, Foreach_callback, &carga_util);
    }
    
}