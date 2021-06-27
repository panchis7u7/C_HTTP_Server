#ifndef _CACHE_H_
#define _CACHE_H_

#include "../headers/HashTable.h"

//Entrada individual a cache.
struct cache_entry {
    char* path;     //Punto final, llave al cache.
    char* content_type;
    unsigned long long content_len;
    void* content;
    struct cache_entry* next, *previous;    //Lista enlazada doble.
};//entrada_cache;

typedef struct Cache {
    HashTable* index;
    struct cache_entry* head, *tail;  //Lista enlazada doble.
    int max_len;
    int len;
} Cache;

extern struct cache_entry* asign_entry(char* path, char* content_type, void* content, unsigned long long content_size);
extern struct cache_entry* get_cache(Cache* cache, char* path);
extern Cache* create_cache(int max_len, int hash_size);
extern void free_entry(struct cache_entry* entry);
extern void put_cache(Cache* cache, char* path, char* content_type, void* content, unsigned long long content_len);
extern void free_cache(Cache* cache);

#endif  //!_CACHE_H_