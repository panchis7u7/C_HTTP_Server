#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

typedef struct HashTable {
    int size;             //Solo lectura.
    int entryCount;    //Solo lectura.
    float payLoad;            //Solo lectura. ->Numero de llaves almacenadas en la tabla, dividida por la capacidad.
    struct List** bucket; 
    unsigned long long (*hashfn)(void* data, unsigned long long dataSize, int bucketcount);
} HashTable;

extern HashTable* createHash(int size, unsigned long long (*hashf)(void*, unsigned long long, int));
extern void destroyHash(HashTable* ht);
extern void* putHash(HashTable* ht, char* key, void* data);
extern void* putHashBin(HashTable* ht, void* key, int keySize, void* data);
extern void* getHash(HashTable* ht, char* key);
extern void* getHashBin(HashTable* ht, void* key, int keySize);
extern void* deleteHash(HashTable* ht, char* key);
extern void* deleteHashBin(HashTable* ht, void* key, int keySize);
extern void forEachHash(HashTable* ht, void (*f)(void*, void*), void* arg);

#endif // !_HASHTABLE_H_