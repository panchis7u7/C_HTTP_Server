#include "Cache.h"
#include "TablaHash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Prototipos de funciones.
/* int remover_cache(struct cache*, char*);
void insertar_ec_lista(struct cache*, struct entrada_cache*);     
void mover_cabeza_lista(struct cache*, struct entrada_cache*);   
void limpiar_lru(struct cache*);
struct entrada_cache* remover_cola_lista(struct cache* cache);   
struct cache* crear_cache(int, int);  */

//Asignar entrada de Cache.
struct entrada_cache* asignar_entrada(char* ruta, char* tipo_contenido, void* contenido, int tamano_contenido){
    struct entrada_cache* entrada = (struct entrada_cache*)malloc(sizeof *entrada);
    if(entrada == NULL){
        return NULL;
    }
    /*entrada->ruta = malloc(strlen(ruta) + 1); //+1 => terminador de cadena \r.
    //strcpy(entrada->ruta, ruta);    //strcpy puede llevar a un desbordamiento y puedes ser hackeado..., mejor usar strncpy!.
    //Porque? -> https://www.youtube.com/watch?v=7mKfWrNQcj0
    //strcpy(entrada->ruta, ruta);
    strncpy(entrada->ruta, ruta, strlen(ruta)+1);
    entrada->tipo_contenido = malloc(strlen(tipo_contenido)+1);
    strncpy(entrada->tipo_contenido, tipo_contenido, strlen(tipo_contenido)+1);
    //strcpy(entrada->tipo_contenido, tipo_contenido);
    entrada->contenido = malloc(tamano_contenido);
    memcpy(entrada->contenido, contenido, tamano_contenido); */
    entrada->ruta = strdup(ruta);
    entrada->tipo_contenido = strdup(tipo_contenido);
    entrada->tamano_contenido = tamano_contenido;
    entrada->contenido = malloc(tamano_contenido);
    memcpy(entrada->contenido, contenido, tamano_contenido);
    entrada->ant = entrada->sig = NULL;
    return entrada;
}

//Liberar memoria ocupada por la entradad de cache.
void liberar_entrada(struct entrada_cache* entrada){
    free(entrada->ruta);
    free(entrada->contenido);
    free(entrada->tipo_contenido);
    free(entrada);
}

//Insertar una entrada de cache (ec) a la cabeza de la lista enlazada.
void insertar_ec_lista(struct cache* cache, struct entrada_cache* ec){
    if(cache->cabeza == NULL){
        cache->cabeza = cache->cola = ec;
        ec->ant = ec->sig = NULL;
    } else {
        cache->cabeza->ant = ec;
        ec->sig = cache->cabeza;
        ec->ant = NULL;
        cache->cabeza = ec;
    }
}

//Mover una entrada de cache (ec) al principio de la lista.
void mover_cabeza_lista(struct cache* cache, struct entrada_cache* ec){
    if(ec != cache->cabeza){
        if(ec == cache->cola){ //Si la entrada es la cola.
            cache->cola = ec->ant;
            cache->cola->sig = NULL;
        } else {    //No estamos en la cabeza ni en la cola.
            ec->ant->sig = ec->sig;
            ec->sig->ant = ec->ant;
        }
        ec->sig = cache->cabeza;
        cache->cabeza->ant = ec;
        ec->ant = NULL;
        cache->cabeza = ec;
    }
}

//Remover objeto de la cola y regresarlo.
struct entrada_cache* remover_cola_lista(struct cache* cache){
    struct entrada_cache* cola_prev = cache->cola; //Cola antigua.
    cache->cola = cola_prev->ant;
    cache->cola->sig = NULL;
    cache->tamano_actual--; //Se decrementa el tamano_actual del cache.
    return cola_prev;
}

//Crear nueva cache... tamano_maximo = numero de entradas en la cache, tamano_hash (0 predeterminado).
struct cache* crear_cache(int tamano_maximo, int tamano_hash){
    struct cache* cache = (struct cache*)malloc(sizeof *cache);
    if(cache == NULL){
        return NULL;
    }
    cache->indice = crear_hash(tamano_hash, NULL);  //NULL -> funcion hash predeterminada.
    cache->cabeza = cache->cola = NULL;
    cache->tamano_maximo = tamano_maximo;
    cache->tamano_actual = 0;
    return cache;
}

//Liberar memoira de la cache.
void liberar_cache(struct cache* cache){
    struct entrada_cache* entrada_actual = cache->cabeza;
    destruir_hash(cache->indice);
    while(entrada_actual != NULL){
        struct entrada_cache* sig_entrada = entrada_actual->sig;
        liberar_entrada(entrada_actual);
        entrada_actual = sig_entrada;
    }
    free(cache);
}

//Almacenar un elemento en la cache... tambien removera el item ultimamente utilizado.
void put_cache(struct cache* cache, char* ruta, char* tipo_contenido, void* contenido, int tamano_contenido){
    //printf("\nPut cache: %s\n", ruta);
    struct entrada_cache* ec = asignar_entrada(ruta, tipo_contenido, contenido, tamano_contenido);
    insertar_ec_lista(cache, ec);
    put_hash(cache->indice, ruta, ec);
    cache->tamano_actual++;
    if(cache->tamano_actual > cache->tamano_maximo){
        struct entrada_cache* cola_antigua = remover_cola_lista(cache);
        eliminar_hash(cache->indice, cola_antigua->ruta);
        liberar_entrada(cola_antigua);
        printf("Tamano actual: %d, Deberia de ser: %d\n", cache->tamano_actual, cache->tamano_maximo - 1);
    }
}

//Obtener una entrada de la cache.
struct entrada_cache* get_cache(struct cache* cache, char* ruta){
    //printf("\nGet cache: %s.\n", ruta);
    //printf("\n%s", ruta);
    struct entrada_cache* ec = get_hash(cache->indice, ruta);
    if(ec == NULL){
        return NULL;
    }
    mover_cabeza_lista(cache, ec);
    return ec;
}

int remover_cache(struct cache* cache, char* ruta){
    (void)cache;
    (void)ruta;
    return 0;
}