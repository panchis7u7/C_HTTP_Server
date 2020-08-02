#include "TablaHash.h"
#include "ListaEnlazada.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANO_DEFAULT 128
#define FACTOR_CRECIMIENTO_DEFAULT 2

//Entrada de una tabla hash.
typedef struct htent {
    void* llave;
    int tamano_llave;
    int llave_hashed;
    void* dato;
}htent;