#include "../headers/Cache.h"
#include "../headers/HashTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Function prototype.
int remove_cache(Cache*, char*);
void insert_entry_list(Cache*, struct cache_entry*);     
void move_entry_head(Cache*, struct cache_entry*);   
struct cache_entry* remove_entry_list(Cache* cache);   

//Asign cache entry.
struct cache_entry* asign_entry(char* path, char* content_type, void* content, unsigned long long content_size){
    struct cache_entry* entry = (struct cache_entry*)malloc(sizeof(struct cache_entry));
    if(entry == NULL){
        return NULL;
    }
    
    entry->path = malloc(strlen(path) + 1); //+1 => terminador de cadena \r.
    //strcpy(entry->path, path);    //strcpy puede llevar a un desbordamiento y puedes ser hackeado..., mejor usar strncpy!.
    //Porque? -> https://www.youtube.com/watch?v=7mKfWrNQcj0
    //strcpy(entry->path, path);
    strncpy(entry->path, path, strlen(path)+1);
    entry->content_type = (char*)malloc(strlen(content_type)+1);
    strncpy(entry->content_type, content_type, strlen(content_type)+1);
    entry->content = malloc(content_size);
    memcpy(entry->content, content, content_size);

    entry->path = strdup(path);
    entry->content_type = strdup(content_type);
    entry->content_len = content_size;
    //entry->content = malloc(tamano_contenido);
    //memcpy(entry->contenido, contenido, tamano_contenido);
    entry->previous = entry->next = NULL;
    return entry;
}

//Liberar memoria ocupada por la entradad de cache.
void free_entry(struct cache_entry* entry){
    free(entry->path);
    free(entry->content);
    free(entry->content_type);
    free(entry);
}

//Insertar una entry de cache (entry) a la head de la lista enlazada.
void insert_entry_list(Cache* cache, struct cache_entry* entry){
    if(cache->head == NULL){
        cache->head = cache->tail = entry;
        entry->previous = entry->next = NULL;
    } else {
        cache->head->previous = entry;
        entry->next = cache->head;
        entry->previous = NULL;
        cache->head = entry;
    }
}

//Mover una entry de cache (entry) al principio de la lista.
void move_entry_head(Cache* cache, struct cache_entry* entry){
    if(entry != cache->head){
        if(entry == cache->tail){ //Si la entry es la tail.
            cache->tail = entry->previous;
            cache->tail->next = NULL;
        } else {    //No estamos en la head ni en la tail.
            entry->previous->next = entry->next;
            entry->next->previous = entry->previous;
        }
        entry->next = cache->head;
        cache->head->previous = entry;
        entry->previous = NULL;
        cache->head = entry;
    }
}

//Remover objeto de la tail y regresarlo.
struct cache_entry* remove_entry_tail(Cache* cache){
    struct cache_entry* cola_prev = cache->tail; //Cola antigua.
    cache->tail = cola_prev->previous;
    cache->tail->next = NULL;
    cache->len--; //Se decrementa el len del cache.
    return cola_prev;
}

//Crear nueva cache... max_len = numero de entradas en la cache, tamano_hash (0 predeterminado).
Cache* create_cache(int max_len, int tamano_hash){
    Cache* cache = (Cache*)malloc(sizeof(Cache));
    if(cache == NULL){
        return NULL;
    }
    cache->index = create_hash(tamano_hash, NULL);  //NULL -> funcion hash predeterminada.
    cache->head = cache->tail = NULL;
    cache->max_len = max_len;
    cache->len = 0;
    return cache;
}

//Liberar memoria de la cache.
void free_cache(Cache* cache){
    struct cache_entry* current_entry = cache->head;
    destroy_hash(cache->index);
    while(current_entry != NULL){
        struct cache_entry* sig_entrada = current_entry->next;
        free_entry(current_entry);
        current_entry = sig_entrada;
    }
    free(cache);
}

//Almacenar un elemento en la cache... tambien removera el item ultimamente utilizado.
void put_cache(Cache* cache, char* path, char* tipo_contenido, void* contenido, unsigned long long tamano_contenido){
    struct cache_entry* entry = asign_entry(path, tipo_contenido, contenido, tamano_contenido);
    insert_entry_list(cache, entry);
    put_hash(cache->index, path, entry);
    cache->len++;
    if(cache->len > cache->max_len){
        struct cache_entry* cola_antigua = remove_entry_tail(cache);
        delete_hash(cache->index, cola_antigua->path);
        free_entry(cola_antigua);
        printf("Tamano actual: %d, Deberia de ser: %d\n", cache->len, cache->max_len - 1);
    }
}

//Obtener una entry de la cache.
struct cache_entry* get_cache(Cache* cache, char* path){
    struct cache_entry* entry = get_hash(cache->index, path);
    if(entry == NULL){
        return NULL;
    }
    move_entry_head(cache, entry);
    return entry;
}

int remove_cache(Cache* cache, char* path){
    (void)cache;
    (void)path;
    return 0;
}