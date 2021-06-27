#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

typedef struct List {
    struct Node* root;
    int count;
} List;

typedef struct Node {
    struct Node* next;
    void* data;
} Node;

extern List* createList(void);
extern void destroyList(List* list);
extern void* insertList(List* lista, void* data);
extern void* insertListEnd(List* lista, void* data);
extern void* firstElementList(List* list);
extern void* lastElementList(List* list);
extern void* findList(List* list, void* data, int (*cmpfn)(void*, void*));
extern void* deleteList(List* list, void* data, int(*cmpfn)(void*, void*));
extern void** getListArray(List* list);
extern void forEachList(List* list, void(*f)(void*, void*), void* arg);
extern void freeListArray(void** a);
extern void printList(List* list);
extern void printListBackward(List* list);
extern int listCount(List* list);

#endif //_LISTAENLAZADA_H_