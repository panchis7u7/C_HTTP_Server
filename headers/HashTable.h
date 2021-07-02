#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

typedef struct HashTable {
    int size;           //ReadOnly.
    int entryCount;     //ReadOnly.
    float payLoad;      //ReadOnly. -> Numero de llaves almacenadas en la tabla, dividida por la capacidad.
    struct List** bucket; 
    unsigned long long (*hashfn)(void* data, unsigned long long dataSize, int bucketcount);
} HashTable;

extern HashTable* create_hash(int size, unsigned long long (*hashf)(void*, unsigned long long, int));
extern void destroy_hash(HashTable* ht);
extern void* put_hash(HashTable* ht, char* key, void* data);
extern void* put_hash_bin(HashTable* ht, void* key, int keySize, void* data);
extern void* get_hash(HashTable* ht, char* key);
extern void* get_hash_bin(HashTable* ht, void* key, int keySize);
extern void* delete_hash(HashTable* ht, char* key);
extern void* delete_hash_bin(HashTable* ht, void* key, int keySize);
extern void for_each_hash(HashTable* ht, void (*f)(void*, void*), void* arg);

#endif // !_HASHTABLE_H_