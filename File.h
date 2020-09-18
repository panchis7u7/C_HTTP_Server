#ifndef _FILE_H_
#define _FILE_H_

typedef struct file_data{
    void* data;
    int tamano;
}file_data;

extern file_data* cargar_archivo(char* nombre_archivo);
extern void liberar_archivo(file_data* datos_archivo);

#endif //!_FILE_H_