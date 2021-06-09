#include "HashTable.h"
#include "ListaEnlazada.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANO_DEFAULT 128
#define FACTOR_CRECIMIENTO_DEFAULT 2

//Prototipos de Funcion.
/*
int htcmp(void*, void*);
int htcmp2(void*, void*);
void anadir_cuenta_entrada(hashtable*, int);
void foreach_callback(void*, void*);
void liberar_htent(void*, void*);
int hashfn_predeterminada(void*, int, int);*/

//Entrada de una tabla hash.
struct htent {
    void* key;
    int keySize;
    int hashKey;
    void* data;
};

//Usado para limpiar las listas enlazadas.
typedef struct forEachListPayload {
    void* arg;
    void (*f)(void*, void*);
} forEachListPayload;

//Cambia la cuenta de entrada y mantener las metricas de cargas.
void addEntryCount(struct hashtable* ht, int d){
    ht->entryCount += d;
    ht->payLoad = (float)ht->entryCount / ht->size;
}

//Funcion Hash Modular (Predeterminada).
unsigned long long defaultHashFuntion(void* data, unsigned long long dataSize, int bucketCount){
    const unsigned long long R = 31; //Numero primo.
    unsigned long long h = 0;
    unsigned char *p = data;
    for (unsigned long long i = 0; i < dataSize; i++)
    {
        h = (R * h + p[i]) % (unsigned long long)bucketCount;
    }
    return h;
}

struct hashtable* createHash(int size, unsigned long long (*hashfn)(void*, unsigned long long, int)){
    if(size < 1){
        size = TAMANO_DEFAULT;
    }
    if(hashfn == NULL){
        hashfn = defaultHashFuntion;
    }

    struct hashtable* ht = (struct hashtable*)malloc(sizeof *ht);

    if(ht == NULL)
        return NULL;

    ht->size = size;
    ht->entryCount = 0;
    ht->payLoad = 0;
    ht->bucket = (struct Lista**)malloc(size * sizeof(struct Lista*));
    ht->hashfn = hashfn;

    for(int i = 0; i < size; i++){
        ht->bucket[i] = crear_lista();
    }
    return ht;
}

//Liberar htent.
void freeHtent(void* htent, void* arg){
    (void)arg;
    free(htent);
}

//Destruir tabla hash.
void destroyHash(struct hashtable* ht){
    for(int i = 0; i < ht->size; i++){
        struct Lista*  lista = ht->bucket[i];
        foreach_lista(lista, freeHtent, NULL);
        destruir_lista(lista);
    }
    free(ht);
}

//Insertar en la tabla hash con una llave tipo cadena (string).
void* putHash(struct hashtable* ht, char* key, void* data){
    return putHashBin(ht, key, strlen(key), data);
}

//Insertar en la tabla hash con una llave binaria.
void* putHashBin(struct hashtable* ht, void* key, int keySize, void* data){
    //printf("Put Hash => %s.\n", (char*)llave);
    int index = ht->hashfn(key, keySize, ht->size);
    struct Lista* list = ht->bucket[index];
    struct htent* ent = (struct htent*)malloc(sizeof *ent);
    ent->key = malloc(keySize);
    memcpy(ent->key, key, keySize);
    ent->keySize = keySize;
    ent->hashKey = index;
    ent->data = data;
    if(insertar_final_lista(list, ent) == NULL){
        free(ent->key);
        free(ent);
        return NULL;
    }
    addEntryCount(ht, +1);
    return data;
}

//Funcion de comparacion para entradas a la tabla hash.
int htcmp(void* a, void* b){
    struct htent* entA = a, *entB = b;
    int dif_tamano = entB->keySize - entA->keySize;
    if(dif_tamano){
        return dif_tamano;  
    }
    return memcmp(entA->key, entB->key, entA->keySize); 
}

//Obtener valor de la tabla hash con una llave tipo cadena (string).
void* getHash(struct hashtable* ht, char* key){
    return getHashBin(ht, key, strlen(key));
}

//Obtener valor de la tabla hash con una llave binaria.
void* getHashBin(struct hashtable* ht, void* key, int keySize){
    //printf("Get Hash => %s.\n", (char*)llave);
    int indice = ht->hashfn(key, keySize, ht->size);
    struct Lista* lista = ht->bucket[indice];
    struct htent cmpent;
    cmpent.key = key;
    cmpent.keySize = keySize;
    struct htent *tmp = encontrar_lista(lista, &cmpent, htcmp);
    if(tmp == NULL) {
        return NULL;
    }
    return tmp->data;
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
void* deleteHash(struct hashtable* ht, char* key){
    return deleteHashBin(ht, key, strlen(key));
}

//Borrar valor de la tabla hash con una llave Binaria.
void* deleteHashBin(struct hashtable* ht, void* key, int keySize){
    int index = ht->hashfn(key, keySize, ht->size);
    struct Lista* lista = ht->bucket[index];
    struct htent cmpent;
    cmpent.key = key;
    cmpent.keySize = keySize;
    struct htent* ent = eliminar_lista(lista, &cmpent, htcmp);
    if(ent == NULL){
        return NULL;
    }    
    void* dato = ent->data;
    free(ent);
    addEntryCount(ht, -1);
    return dato;
}

//Funcion de llamada de vuelta (callback) para cada elemento (Foreach).
void forEachCallback(void* vent, void* v_carga_util){
    struct htent* ent = vent;
    struct forEachListPayload* carga_util = v_carga_util;
    carga_util->f(ent->data, carga_util->arg);
}

//Para cada elemento en la tabla hash.
void forEach(struct hashtable* ht, void(*f)(void*, void*), void* arg){
    forEachListPayload carga_util;
    carga_util.f = f;
    carga_util.arg = arg;
    for (int i = 0; i < ht->size; i++)
    {
        struct Lista* lista = ht->bucket[i];    
        foreach_lista(lista, forEachCallback, &carga_util);
    }
    
}