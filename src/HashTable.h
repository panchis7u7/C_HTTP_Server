#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

struct hashtable {
    int size;             //Solo lectura.
    int entryCount;    //Solo lectura.
    float payLoad;            //Solo lectura. ->Numero de llaves almacenadas en la tabla, dividida por la capacidad.
    struct Lista** bucket; 
    unsigned long long (*hashfn)(void* data, unsigned long long dataSize, int bucketcount);
};//hashtable;

extern struct hashtable* createHash(int size, unsigned long long (*hashf)(void*, unsigned long long, int));
extern void destroyHash(struct hashtable* ht);
extern void* putHash(struct hashtable* ht, char* key, void* data);
extern void* putHashBin(struct hashtable* ht, void* key, int keySize, void* data);
extern void* getHash(struct hashtable* ht, char* key);
extern void* getHashBin(struct hashtable* ht, void* key, int keySize);
extern void* deleteHash(struct hashtable* ht, char* key);
extern void* deleteHashBin(struct hashtable* ht, void* key, int keySize);
extern void forEach(struct hashtable* ht, void (*f)(void*, void*), void* arg);

#endif // !_HASHTABLE_H_