#ifndef _CACHE_H_
#define _CACHE_H_

//Entrada individual a cache.
struct entrada_cache {
    char* ruta;     //Punto final, llave al cache.
    char* tipo_contenido;
    unsigned long long tamano_contenido;
    void* contenido;
    struct entrada_cache* sig, *ant;    //Lista enlazada doble.
};//entrada_cache;

struct cache {
    struct hashtable* indice;
    struct entrada_cache* cabeza, *cola;  //Lista enlazada doble.
    int tamano_maximo;
    int tamano_actual;
};//cache;

extern struct entrada_cache* asignar_entrada(char* ruta, char* tipo_contenido, void* contenido, unsigned long long tamano_contenido);
extern struct entrada_cache* get_cache(struct cache* cache, char* ruta);
extern struct cache* crear_cache(int tamano_maximo, int tamano_hash);
extern void liberar_entrada(struct entrada_cache* entrada);
extern void put_cache(struct cache* cache, char* ruta, char* tipo_contenido, void* contenido, unsigned long long tamano_contenido);
extern void liberar_cache(struct cache* cache);

#endif  //!_CACHE_H_