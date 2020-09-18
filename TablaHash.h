#ifndef _TABLAHASH_H_
#define _TABLAHASH_H_

typedef struct hashtable {
    int tamano;             //Solo lectura.
    int numero_entradas;    //Solo lectura.
    float carga;            //Solo lectura. ->Numero de llaves almacenadas en la tabla, dividida por la capacidad.
    struct Lista** cubeta; 
    int (*hashf)(void* dato, int tamano_dato, int cuenta_cubeta);
}hashtable;

extern hashtable* crear_hash(int tamano, int (*hashf)(void*, int, int));
extern void destruir_hash(hashtable* ht);
extern void* put_hash(hashtable* ht, char* llave, void* dato);
extern void* put_hash_bin(hashtable* ht, void* llave, int tamano_llave, void* dato);
extern void* get_hash(hashtable* ht, char* llave);
extern void* get_hash_bin(hashtable* ht, void* llave, int tamano_llave);
extern void* eliminar_hash(hashtable* ht, char* llave);
extern void* eliminar_hash_bin(hashtable* ht, void* llave, int tamano_llave);
extern void foreach_hash(hashtable* ht, void (*f)(void*, void*), void* arg);

#endif // !_TABLAHASH_H_