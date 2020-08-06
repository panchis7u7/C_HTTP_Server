#include "File.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>   //Nos permite obtener informacion sobre atributos de archivos.

//Carga un archivo en memoria y regresa un apuntador a los datos, el buffer no esta terminado en nulo.
file_data* Cargar_archivo(char* nombre_archivo){
    char* buffer, *p;
    struct stat buf;
    int bytes_leidos, bytes_restantes, bytes_totales = 0;

    //Obtener el tamano del archivo.
    if(stat(nombre_archivo, &buf) == -1){ //stat, obtiene los atributos de los archivos y los mete en buf.
        return NULL;
    }

    //Asegurarse si es un archivo normal.
    if(!(buf.st_mode & __S_IFREG)){
        return NULL;
    }

    //Abrir el archivo para lectura.
    FILE* fp = fopen(nombre_archivo, "rb");
    if (fp == NULL)
        return NULL;
    
    //Reservar bytes.
    bytes_restantes = buf.st_size;
    p = buffer = malloc(bytes_restantes);
    if(buffer == NULL)
        return NULL;
    
    //Leer el archivo completo.
    while (bytes_leidos = fread(p, 1, bytes_restantes, fp), bytes_leidos != 0 && bytes_restantes > 0){
        if(bytes_leidos == -1){
            free(buffer);
            return NULL;
        }
    bytes_restantes -= bytes_leidos;
    p += bytes_leidos;
    bytes_totales += bytes_leidos;
    }

    //Asignar memoria a la estructura de datos de archivo.
    file_data* datos_archivo = malloc(sizeof(file_data));
    if(datos_archivo == NULL){
        free(buffer);
        return NULL;
    }

    datos_archivo->data = buffer;
    datos_archivo->tamano = bytes_totales;
    return datos_archivo;
}

//Liberar memoria reservada por Cargar_archivo().
void Liberar_archivo(file_data* datos_archivo){
    free(datos_archivo->data);
    free(datos_archivo);
}