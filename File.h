#ifndef _FILE_H_
#define _FILE_H_

typedef struct file_data{
    void* data;
    int tamano;
}file_data;

extern file_data* Cargar_archivo(char* nombre_archivo);
extern void Liberar_archivo(file_data* datos_archivo);

#endif //!_FILE_H_