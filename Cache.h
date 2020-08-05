#ifndef _CACHE_H_
#define _CACHE_H_

//Entrada individual a cache.
typedef struct entrada_cache {
    char* ruta;     //Punto final, llave al cache.
    char* tipo_contenido;
    int tamano_contenido;
    void* contenido;
    struct entrada_cache* sig, *ant;    //Lista enlazada doble.
}entrada_cache;

typedef struct cache {
    struct hashtable* indice;
    struct entrada_cache* cabeza, *cola;  //Lista enlazada doble.
    int tamano_maximo;
    int tamano_actual;
}cache;

extern entrada_cache* Asignar_entrada(char* ruta, char* tipo_contenido, void* contenido, int tamano_contenido);
extern entrada_cache* Get_cache(cache* cache, char* ruta);
extern cache* Crear_cache(int tamano_maximo, int tamano_hash);
extern void Liberar_entrada(entrada_cache* entrada);
extern void Put_cache(cache* cache, char* ruta, char* tipo_contenido, void* contenido, int tamano_contenido);
extern void Liberar_cache(cache* cache);

#endif  //!_CACHE_H_