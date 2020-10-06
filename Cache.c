#include "Cache.h"
#include "TablaHash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Prototipos de funciones.
int remover_cache(cache*, char*);
void insertar_ec_lista(cache*, entrada_cache*);     
void mover_cabeza_lista(cache*, entrada_cache*);   
void limpiar_lru(cache*);
entrada_cache* remover_cola_lista(cache* cache);   
cache* crear_cache(int, int); 

//Asignar entrada de Cache.
entrada_cache* asignar_entrada(char* ruta, char* tipo_contenido, void* contenido, unsigned long long tamano_contenido){
    entrada_cache* entrada = malloc(sizeof(entrada_cache));
    entrada->ruta = malloc(strlen(ruta) + 1); //+1 => terminador de cadena \r.
    //strcpy(entrada->ruta, ruta);    //strcpy puede llevar a un desbordamiento y puedes ser hackeado..., mejor usar strncpy!.
    //Porque? -> https://www.youtube.com/watch?v=7mKfWrNQcj0
    //strcpy(entrada->ruta, ruta);
    strncpy(entrada->ruta, ruta, strlen(ruta)+1);
    entrada->tipo_contenido = malloc(strlen(tipo_contenido) + 1);
    strncpy(entrada->tipo_contenido, tipo_contenido, strlen(tipo_contenido));
    //strcpy(entrada->tipo_contenido, tipo_contenido);
    entrada->contenido = malloc(tamano_contenido);
    memcpy(entrada->contenido, contenido, tamano_contenido);
    return entrada;
}

//Liberar memoria ocupada por la entradad de cache.
void liberar_entrada(entrada_cache* entrada){
    free(entrada->ruta);
    free(entrada->contenido);
    free(entrada->tipo_contenido);
    free(entrada);
}

//Insertar una entrada de cache (ec) a la cabeza de la lista enlazada.
void insertar_ec_lista(cache* cache, entrada_cache* ec){
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
void mover_cabeza_lista(cache* cache, entrada_cache* ec){
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
entrada_cache* remover_cola_lista(cache* cache){
    entrada_cache* cola_prev = cache->cola; //Cola antigua.
    cache->cola = cola_prev->ant;
    cache->cola->sig = NULL;
    cache->tamano_actual--; //Se decrementa el tamano_actual del cache.
    return cola_prev;
}

//Limpiar entradas LRU si el cache se satura (oversize).
void limpiar_lru(cache* cache){
    while (cache->tamano_actual > cache->tamano_maximo)
    {
        entrada_cache* cola_prev = remover_cola_lista(cache);
        eliminar_hash(cache->indice, cola_prev->ruta);  //Elimiar dato de la tabla hash, arg -> llave.
        liberar_entrada(cola_prev);
    }
}

//Crear nueva cache... tamano_maximo = numero de entradas en la cache, tamano_hash (0 predeterminado).
cache* crear_cache(int tamano_maximo, int tamano_hash){
    cache* cache = malloc(sizeof(cache));
    cache->cabeza = cache->cola = NULL;
    cache->indice = crear_hash(tamano_hash, NULL);  //NULL -> funcion hash predeterminada.
    cache->tamano_maximo = tamano_maximo;
    cache->tamano_actual = 0;
    return cache;
}

//Liberar memoira de la cache.
void liberar_cache(cache* cache){
    entrada_cache* entrada_actual = cache->cabeza;
    destruir_hash(cache->indice);
    while(entrada_actual != NULL){
        entrada_cache* sig_entrada = entrada_actual->sig;
        liberar_entrada(entrada_actual);
        entrada_actual = sig_entrada;
    }
    free(cache);
}

//Almacenar un elemento en la cache... tambien removera el item ultimamente utilizado.
void put_cache(cache* cache, char* ruta, char* tipo_contenido, void* contenido, unsigned long long tamano_contenido){
    entrada_cache* ec = asignar_entrada(ruta, tipo_contenido, contenido, tamano_contenido);
    insertar_ec_lista(cache, ec);
    put_hash(cache->indice, ruta, ec);
    cache->tamano_actual++;
    limpiar_lru(cache); //Remover el elemento ultimamente utilizado almacenado en el cache.
}

//Obtener una entrada de la cache.
entrada_cache* get_cache(cache* cache, char* ruta){
    entrada_cache* ec;
    ec = get_hash(cache->indice, ruta);
    if(ec == NULL){
        return NULL;
    }

    mover_cabeza_lista(cache, ec);
    return ec;
}

int remover_cache(cache* cache, char* ruta){
    (void)cache;
    (void)ruta;
    return 0;
}