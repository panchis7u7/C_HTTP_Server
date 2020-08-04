#ifndef _TABLAHASH_H_
#define _TABLAHASH_H_

typedef struct hashtable {
    int tamano;             //Solo lectura.
    int numero_entradas;    //Solo lectura.
    float carga;            //Solo lectura. ->Numero de llaves almacenadas en la tabla, dividida por la capacidad.
    struct Lista** cubeta; 
    int (*hashf)(void* dato, int tamano_dato, int cuenta_cubeta);
}hashtable;

extern hashtable* Crear_hash(int, int (*hashf)(void*, int, int));
extern void Destruir_hash(hashtable*);
extern void* Put_hash(hashtable*, char*, void*);
extern void* Put_hash_bin(hashtable*, void*, int, void*);
extern void* Get_hash(hashtable*, char*);
extern void* Get_hash_bin(hashtable*, void*, int);
extern void* Eliminar_hash(hashtable*, char*);
extern void* Eliminar_hash_bin(hashtable*, void*, int);
extern void Foreach_hash(hashtable*, void (*f)(void*, void*), void* arg);

#endif // !_TABLAHASH_H_