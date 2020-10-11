#include "TablaHash.h"
#include "ListaEnlazada.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANO_DEFAULT 128
#define FACTOR_CRECIMIENTO_DEFAULT 2

//Prototipos de Funcion.
/* int htcmp(void*, void*);
int htcmp2(void*, void*);
void anadir_cuenta_entrada(hashtable*, int);
void foreach_callback(void*, void*);
void liberar_htent(void*, void*);
int hashfn_predeterminada(void*, int, int); */

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
void anadir_cuenta_entrada(struct hashtable* ht, int d){
    ht->numero_entradas += d;
    ht->carga = (float)ht->numero_entradas / ht->tamano;
}

//Funcion Hash Modular (Predeterminada).
unsigned long long hashfn_predeterminada(void* dato, unsigned long long tamano_dato, int conteo_cubeta){
    const unsigned long long R = 31; //Numero primo.
    unsigned long long h = 0;
    unsigned char *p = dato;
    for (unsigned long long i = 0; i < tamano_dato; i++)
    {
        h = (R * h + p[i]) % (unsigned long long)conteo_cubeta;
    }
    return h;
}

struct hashtable* crear_hash(int tamano, unsigned long long (*hashfn)(void*, unsigned long long, int)){
    if(tamano < 1){
        tamano = TAMANO_DEFAULT;
    }
    if(hashfn == NULL){
        hashfn = hashfn_predeterminada;
    }

    struct hashtable* ht = (struct hashtable*)malloc(sizeof *ht);

    if(ht == NULL)
        return NULL;

    ht->tamano = tamano;
    ht->numero_entradas = 0;
    ht->carga = 0;
    ht->cubeta = (struct Lista**)malloc(tamano * sizeof(struct Lista*));
    ht->hashfn = hashfn;

    for(int i = 0; i < tamano; i++){
        ht->cubeta[i] = crear_lista();
    }
    return ht;
}

//Liberar htent.
void liberar_htent(void* htent, void* arg){
    (void)arg;
    free(htent);
}

//Destruir tabla hash.
void destruir_hash(struct hashtable* ht){
    for(int i = 0; i < ht->tamano; i++){
        struct Lista*  lista = ht->cubeta[i];
        foreach_lista(lista, liberar_htent, NULL);
        destruir_lista(lista);
    }
    free(ht);
}

//Insertar en la tabla hash con una llave tipo cadena (string).
void* put_hash(struct hashtable* ht, char* llave, void* dato){
    return put_hash_bin(ht, llave, strlen(llave), dato);
}

//Insertar en la tabla hash con una llave binaria.
void* put_hash_bin(struct hashtable* ht, void* llave, int tamano_llave, void* dato){
    //printf("Put Hash => %s.\n", (char*)llave);
    int indice = ht->hashfn(llave, tamano_llave, ht->tamano);
    struct Lista* lista = ht->cubeta[indice];
    struct htent* ent = (struct htent*)malloc(sizeof *ent);
    ent->llave = malloc(tamano_llave);
    memcpy(ent->llave, llave, tamano_llave);
    ent->tamano_llave = tamano_llave;
    ent->llave_hash = indice;
    ent->dato = dato;
    if(insertar_final_lista(lista, ent) == NULL){
        free(ent->llave);
        free(ent);
        return NULL;
    }
    anadir_cuenta_entrada(ht, +1);
    return dato;
}

//Funcion de comparacion para entradas a la tabla hash.
int htcmp(void* a, void* b){
    struct htent* entA = a, *entB = b;
    int dif_tamano = entB->tamano_llave - entA->tamano_llave;
    if(dif_tamano){
        return dif_tamano;  
    }
    return memcmp(entA->llave, entB->llave, entA->tamano_llave); 
}

//Obtener valor de la tabla hash con una llave tipo cadena (string).
void* get_hash(struct hashtable* ht, char* llave){
    return get_hash_bin(ht, llave, strlen(llave));
}

//Obtener valor de la tabla hash con una llave binaria.
void* get_hash_bin(struct hashtable* ht, void* llave, int tamano_llave){
    //printf("Get Hash => %s.\n", (char*)llave);
    int indice = ht->hashfn(llave, tamano_llave, ht->tamano);
    struct Lista* lista = ht->cubeta[indice];
    struct htent cmpent;
    cmpent.llave = llave;
    cmpent.tamano_llave = tamano_llave;
    struct htent *tmp = encontrar_lista(lista, &cmpent, htcmp);
    if(tmp == NULL) {
        return NULL;
    }
    return tmp->dato;
}


//Funcion de comparacion entre llaves.
/* int htcmp2(void* a, void* b){
    struct htent* entA = a, *entB = b;
    if(*((char*)(entA->llave)) == *((char*)(entB->llave))){
        return 1;
    }
    return 0;
} */

//Borrar valor de la tabla hash con una llave tipo cadena (string).
void* eliminar_hash(struct hashtable* ht, char* llave){
    return eliminar_hash_bin(ht, llave, strlen(llave));
}

//Borrar valor de la tabla hash con una llave Binaria.
void* eliminar_hash_bin(struct hashtable* ht, void* llave, int tamano_llave){
    int indice = ht->hashfn(llave, tamano_llave, ht->tamano);
    struct Lista* lista = ht->cubeta[indice];
    struct htent cmpent;
    cmpent.llave = llave;
    cmpent.tamano_llave = tamano_llave;
    struct htent* ent = eliminar_lista(lista, &cmpent, htcmp);
    if(ent == NULL){
        return NULL;
    }    
    void* dato = ent->dato;
    free(ent);
    anadir_cuenta_entrada(ht, -1);
    return dato;
}

//Funcion de llamada de vuelta (callback) para cada elemento (Foreach).
void foreach_callback(void* vent, void* v_carga_util){
    struct htent* ent = vent;
    struct carga_foreach_lista* carga_util = v_carga_util;
    carga_util->f(ent->dato, carga_util->arg);
}

//Para cada elemento en la tabla hash.
void foreach_hash(struct hashtable* ht, void(*f)(void*, void*), void* arg){
    carga_foreach_lista carga_util;
    carga_util.f = f;
    carga_util.arg = arg;
    for (int i = 0; i < ht->tamano; i++)
    {
        struct Lista* lista = ht->cubeta[i];    
        foreach_lista(lista, foreach_callback, &carga_util);
    }
    
}